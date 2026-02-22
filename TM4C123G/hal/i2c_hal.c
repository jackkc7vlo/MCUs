/******************************************************************************
 * Copyright (C) 2026 by Jack Wilson
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 ************************(C) COPYRIGHT 2026 Jack Wilson **********************/
/**
 * @file i2c_hal.c
 * @brief Generic I2C interface implementation.
 * @details Outlines the portable I2C abstraction used to configure and
 * interact with I2C peripherals on any supported platform.
 *
 * @author  Jack Wilson
 * @date    February 12, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Includes
 ********************************************************************************/

#include <hw_config.h>
#include <stdint.h> /*lint -e129*/
#include <stdbool.h> 
#include <i2c_hal.h>
#include <stddef.h>
#include <tm4c123gh6pm.h>
#if HW_CONFIG_I2C == 1u

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    static bool i2c_initialized = false;
    /********************************************************************************
     * Defines
     ********************************************************************************/

    // support for I2C callbacks
    typedef struct i2c_callback_context_s
    {                                           /**< The GPIO pin number */
        p_i2c_callback_t callback;          /**< Registered interrupt callback */
        void                *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
        void                *callback_context;  /**< User callback context */
    } i2c_callback_context_t;

    /********************************************************************************
     * Private I2C HAL struct (definition kept in the implementation file).
     ********************************************************************************/
    struct i2c_hal
    {
        bool in_use; /*!< Indicates if this I2C instance is in use */
        // uint32_t i2c_num;         /*!< I2C index */

        i2c_callback_context_t callback; /**< Registered interrupt callbacks */
    };

    static i2c_hal_t i2c_hal_devices[NUM_I2C_DEVICES] = {0};

    /********************************************************************************
     * @brief          Initialization function for the I2C driver
     * @retval         None
     ********************************************************************************/
    void i2c_hal_initialize(void)
    {
        if (!i2c_initialized)
        {
            // perform any necessary hardware initialization here
            i2c_initialized = true;
        }
    }

    /********************************************************************************
     * @brief          Check if the I2C driver is initialized
     * @retval         true if initialized, false otherwise
     ********************************************************************************/
    bool i2c_hal_is_initialized(void)
    {
        return i2c_initialized;
    }
    /********************************************************************************
     * @brief          Create an I2C device
     * @param[in]      i2c_device_id - I2C device ID
     * @param[in]      p_callback - Pointer to the callback function
     * @retval         Pointer to the I2C device
     ********************************************************************************/
    p_i2c_hal_t i2c_hal_create_device(const uint32_t i2c_device_id, const p_i2c_callback_t p_callback)
    {
        if (i2c_device_id >= NUM_I2C_DEVICES)
        {
            return NULL; // Invalid device ID
        }

        i2c_hal_t *p_device = &i2c_hal_devices[i2c_device_id];

        if (p_device->in_use)
        {
            return p_device; // Device already in use
        }

        p_device->in_use = true;
        p_device->callback.callback = p_callback;
        p_device->callback.p_callback_handle = NULL; // Set as needed
        p_device->callback.callback_context = NULL;  // Set as needed

        SYSCTL_RCGCGPIO_R  |= 0x00000008 ; // Enable the clock for port D
        SYSCTL_RCGCI2C_R   |= 0x00000008 ; // Enable the clock for I2C 3
        GPIO_PORTD_DEN_R |= 0x03; // Assert DEN for port D
        // Configure Port D pins 0 and 1 as  I2C 3
        GPIO_PORTD_AFSEL_R |= 0x00000003 ;
        GPIO_PORTD_PCTL_R |= 0x00000033 ;
        GPIO_PORTD_ODR_R |= 0x00000002 ; // SDA (PD1 ) pin as open drain
        I2C3_MCR_R  = 0x0010 ; // Enable I2C 3 master function
        /* Configure I2C 3 clock frequency
        (1 + TIME_PERIOD ) = SYS_CLK /(2*
        ( SCL_LP + SCL_HP ) * I2C_CLK_Freq )
        TIME_PERIOD = 16 ,000 ,000/(2(6+4) *100000) - 1 = 7 */
        I2C3_MTPR_R  = 0x07 ;

        // Additional hardware-specific initialization can be done here

        return p_device;
    }

    /* wait untill I2C Master module is busy */
    /*  and if not busy and no error return 0 */
    static int I2C_wait_till_done(void)
    {
        while(I2C3_MCS_R & 1);   /* wait until I2C master is not busy */
        return I2C3_MCS_R & 0xE; /* return I2C error code, 0 if no error*/
    }


    /********************************************************************************
        * @brief          Write function for the I2C driver
        * @param[in]      p_handle - pointer to the i2c device
        * @param[in]      cmd_or_register - The register or command to write data to
        *(i.e. memory address)
        * @param[in]      cmd_or_register_len - Length of the register or command in
        *bytes
        * @param[in]      data -  pointer to data to send (may be words depending on
        *MCU)
        * @param[in]      data_len - Length of data to write in bytes
        * @retval         Number of bytes written
        ********************************************************************************/
    uint32_t i2c_hal_write(const p_i2c_hal_t p_handle, const uint32_t slave_address, const uint32_t cmd_or_register, const uint32_t cmd_or_register_len,
                    const uint8_t *const data, const uint_fast16_t data_len)
    {
        char error;
        uint_fast16_t bytes_written = 0;
        uint_fast16_t bytes_to_write = data_len;
        uint8_t *pdata = (uint8_t *)data;

        if (bytes_to_write <= 0)
            return -1;                  /* no write was performed */
        /* send slave address and starting address */
        I2C3_MSA_R = slave_address << 1;
        I2C3_MDR_R = cmd_or_register;
        I2C3_MCS_R = 3;                  /* S-(saddr+w)-ACK-maddr-ACK */

        error = I2C_wait_till_done();   /* wait until write is complete */
        if (error) return error;

        /* send data one byte at a time */
        while (data_len > 1)
        {
            I2C3_MDR_R = *pdata++;             /* write the next byte */
            I2C3_MCS_R   = 1;                   /* -data-ACK- */
            error = I2C_wait_till_done();
            if (error) return error;
            bytes_to_write--;
            bytes_written++;
        }
        
        /* send last byte and a STOP */
        I2C3_MDR_R = *pdata++;                 /* write the last byte */
        I2C3_MCS_R = 5;                       /* -data-ACK-P */
        error = I2C_wait_till_done();
        while(I2C3_MCS_R & 0x40);             /* wait until bus is not busy */
        if (error) return error;
        return 0;       /* no error */        
    }

    /********************************************************************************
        * @brief          Read function for the I2C driver
        * @param[in]      p_handle - pointer to the i2c device
        * @param[in]      cmd_or_register - The register or command to write data to
        *(i.e. memory address)
        * @param[in]      cmd_or_register_len - Length of the register or command in
        *bytes
        * @param[in]      data -  pointer to data (in bytes!) to read
        * @param[in]      data_len - Length of data to read in bytes
        * @retval         Number of bytes read
        ********************************************************************************/
    uint32_t i2c_hal_read(const p_i2c_hal_t p_handle, const uint32_t slave_address, const uint32_t cmd_or_register, const uint32_t cmd_or_register_len,
                    uint8_t *const data, const uint_fast16_t data_len)
    {
        char error;
        uint_fast16_t bytes_to_read = data_len;
        uint8_t *pdata = data;
    
        if (data_len <= 0)
            return -1;         /* no read was performed */

        /* send slave address and starting address */
        I2C3_MSA_R = slave_address << 1;
        I2C3_MDR_R = cmd_or_register;
        I2C3_MCS_R = 3;       /* S-(saddr+w)-ACK-maddr-ACK */
        error = I2C_wait_till_done();
        if (error)
            return error;

        /* to change bus from write to read, send restart with slave addr */
        I2C3_MSA_R = (slave_address << 1) + 1;   /* restart: -R-(saddr+r)-ACK */

        if (bytes_to_read == 1)             /* if last byte, don't ack */
            I2C3_MCS_R = 7;              /* -data-NACK-P */
        else                            /* else ack */
            I2C3_MCS_R = 0xB;            /* -data-ACK- */
        error = I2C_wait_till_done();
        if (error) return error;

        *pdata++ = I2C3_MDR_R;            /* store the data received */

        if (--bytes_to_read == 0)           /* if single byte read, done */
        {
            while(I2C3_MCS_R & 0x40);    /* wait until bus is not busy */
            return 0;       /* no error */
        }
    
        /* read the rest of the bytes */
        while (bytes_to_read > 1)
        {
            I2C3_MCS_R = 9;              /* -data-ACK- */
            error = I2C_wait_till_done();
            if (error) return error;
            bytes_to_read--;
            *pdata++ = I2C3_MDR_R;        /* store data received */
        }

        I2C3_MCS_R = 5;                  /* -data-NACK-P */
        error = I2C_wait_till_done();
        *pdata = I2C3_MDR_R;              /* store data received */
        while(I2C3_MCS_R & 0x40);        /* wait until bus is not busy */
        
        return 0;       /* no error */
    }

    /********************************************************************************
        * @brief  Enable (power on) or disable (power off) to the i2c devices
        * @param[in]      p_handle - pointer to the i2c device
        * @param[in]      turn_on - enable or disable the i2c devices (POWER_ON or
        *POWER_OFF)
        * @param[in]      settle_time_ms - number of milliseconds to wait for the i2c
        *                                  devices power on
        * @retval
        ********************************************************************************/
    void i2c_hal_enable(const p_i2c_hal_t p_handle, const bool turn_on, const uint32_t settle_time_ms)
    {
        
    }

    /**
        * @brief Register a callback to be invoked on GPIO interrupt events.
        * @param[in] handle Pointer to platform-specific context associated with the
        * callback.
        * @param callback Function to invoke when an interrupt fires. May be NULL to
        * clear the registration.
        *
        * @return 0 on success, negative value when registration fails.
        */
    int i2c_hal_register_callback(const p_i2c_hal_t p_handle, p_i2c_callback_t callback, void *p_callback_context)
    {
        if (p_handle == NULL)
        {
            return -1; // Invalid handle
        }

        p_handle->callback.callback = callback;
        p_handle->callback.p_callback_handle = p_callback_context;

        return 0; // Success
    }


#if USE_I2C_INTERRUPTS == 1
    /**
     * @brief  Handle an I2C interrupt
     * @retval None
     */
    void i2c_hal_irqhandler(void);
#endif

#endif /* USE_I2C */

