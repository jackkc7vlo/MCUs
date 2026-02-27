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

#include <clock_hal.h>
#include <gpio_hal.h>
#include <hw_config.h>
#include <i2c_hal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h> /*lint -e129*/
#include <tm4c123gh6pm.h>
#if HW_CONFIG_I2C == 1u

/********************************************************************************
 * Defines
 ********************************************************************************/
#define NUM_I2C_MODULES 4U /**< TM4C123G supports I2C0 – I2C3 */

/********************************************************************************
 * Typedefs & Enums
 ********************************************************************************/

/**
 * @brief Memory-mapped I2C master register block.
 * @details Offsets 0x000 – 0x020 are contiguous for the master-side registers
 *          used by this driver.  Cast the module base address to this type.
 */
typedef struct
{
    volatile uint32_t MSA;  /**< 0x000  Master Slave Address          */
    volatile uint32_t MCS;  /**< 0x004  Master Control / Status       */
    volatile uint32_t MDR;  /**< 0x008  Master Data                   */
    volatile uint32_t MTPR; /**< 0x00C  Master Timer Period            */
    volatile uint32_t MIMR; /**< 0x010  Master Interrupt Mask          */
    volatile uint32_t MRIS; /**< 0x014  Master Raw Interrupt Status    */
    volatile uint32_t MMIS; /**< 0x018  Master Masked Interrupt Status */
    volatile uint32_t MICR; /**< 0x01C  Master Interrupt Clear         */
    volatile uint32_t MCR;  /**< 0x020  Master Configuration           */
} i2c_master_regs_t;

/** Base addresses for I2C0 – I2C3 master register blocks. */
static i2c_master_regs_t *const i2c_base[NUM_I2C_MODULES] = {
    (i2c_master_regs_t *)0x40020000U, /* I2C0 */
    (i2c_master_regs_t *)0x40021000U, /* I2C1 */
    (i2c_master_regs_t *)0x40022000U, /* I2C2 */
    (i2c_master_regs_t *)0x40023000U, /* I2C3 */
};

static bool i2c_initialized = false;

// support for I2C callbacks
typedef struct i2c_callback_context_s
{                                       /**< The GPIO pin number */
    p_i2c_callback_t callback;          /**< Registered interrupt callback */
    void            *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
    void            *callback_context;  /**< User callback context */
} i2c_callback_context_t;

/********************************************************************************
 * Private I2C HAL struct (definition kept in the implementation file).
 ********************************************************************************/
struct i2c_hal
{
    uint32_t               in_use_count;  /*!< Indicates if this I2C instance is in use */
    bool                   enabled;       /**< true when hardware is powered on */
    uint32_t               i2c_module_id; /**< I2C peripheral index (0 – 3) */
    i2c_master_regs_t     *regs;          /**< Pointer to memory-mapped registers */
    p_gpio_hal_t           p_scl_gpio;
    p_gpio_hal_t           p_sda_gpio; /*lint !e551*/
    uint8_t                scl_pin;    /**< SCL pin number within the port */
    uint8_t                sda_pin;    /**< SDA pin number within the port */
    i2c_callback_context_t callback;   /**< Registered interrupt callbacks */
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
 * @param[in]      i2c_board_id - I2C device ID
 * @param[in]      p_callback - Pointer to the callback function
 * @retval         Pointer to the I2C device
 ********************************************************************************/
p_i2c_hal_t i2c_hal_create_device(const uint32_t i2c_board_id, const uint32_t sda_port, const uint32_t sda_pin,
                                  const uint32_t scl_port, const uint32_t scl_pin, const p_i2c_callback_t p_callback)
{
    if (i2c_board_id >= NUM_I2C_DEVICES)
    {
        return NULL; // Invalid device ID
    }

    i2c_hal_t *p_device = &i2c_hal_devices[i2c_board_id];

    if (p_device->in_use_count)
    {
        return p_device; // Device already in use
    }

    if (i2c_board_id >= NUM_I2C_MODULES)
    {
        return NULL; // I2C module index out of range for this MCU
    }

    p_device->in_use_count++;
    p_device->i2c_module_id              = i2c_board_id;
    p_device->regs                       = i2c_base[i2c_board_id];
    p_device->enabled                    = false;
    p_device->callback.callback          = p_callback;
    p_device->callback.p_callback_handle = NULL; // Set as needed
    p_device->callback.callback_context  = NULL; // Set as needed

    /* Create GPIO handles for SCL and SDA (resource allocation only) */
    p_device->p_scl_gpio = gpio_hal_create(scl_port);
    p_device->p_sda_gpio = gpio_hal_create(sda_port);
    if (p_device->p_scl_gpio == NULL || p_device->p_sda_gpio == NULL)
    {
        p_device->in_use_count--;
        return NULL;
    }

    /* Store pin numbers for enable/disable/cleanup */
    p_device->scl_pin = (uint8_t)scl_pin;
    p_device->sda_pin = (uint8_t)sda_pin;

    return p_device;
}

void i2c_hal_remove_device(p_i2c_hal_t p_handle)
{
    if (p_handle == NULL || p_handle->in_use_count == 0u)
    {
        return; // Nothing to remove
    }

    if (--p_handle->in_use_count == 0u)
    {
        /* Power down the hardware first */
        i2c_hal_enable(p_handle, false, 0U);

        /* Release GPIO handles */
        gpio_hal_remove(p_handle->p_scl_gpio);
        gpio_hal_remove(p_handle->p_sda_gpio);
        p_handle->p_scl_gpio = NULL;
        p_handle->p_sda_gpio = NULL;

        /* Clear the callback */
        p_handle->callback.callback          = NULL;
        p_handle->callback.p_callback_handle = NULL;
        p_handle->callback.callback_context  = NULL;
    }
}

/* wait until I2C Master module is no longer busy */
/*  and if not busy and no error return 0 */
#define I2C_TIMEOUT_COUNT 10000U /* ~10ms at 16 MHz */
static int I2C_wait_till_done(const i2c_master_regs_t *regs)
{
    volatile uint32_t timeout = I2C_TIMEOUT_COUNT;
    while ((regs->MCS & 1) && (--timeout > 0))
    {
        /* wait until I2C master is not busy or timeout */
    }
    if (timeout == 0)
    {
        return -1; /* timed out — bus may be stuck */
    }
    return regs->MCS & 0xE; /* return I2C error code, 0 if no error */
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
uint32_t i2c_hal_write(const p_i2c_hal_t p_handle, const uint32_t slave_address, const uint32_t cmd_or_register,
                       const uint32_t cmd_or_register_len, const uint8_t *const data, const uint_fast16_t data_len)
{
    int                error;
    uint_fast16_t      bytes_written  = 0;
    uint_fast16_t      bytes_to_write = data_len;
    uint8_t           *pdata          = (uint8_t *)data;
    i2c_master_regs_t *regs           = p_handle->regs;

    if (bytes_to_write == 0)
        return (uint32_t)-1; /* no write was performed */
    /* send slave address and starting address */
    regs->MSA = slave_address << 1;
    regs->MDR = cmd_or_register;
    regs->MCS = 3; /* S-(saddr+w)-ACK-maddr-ACK */

    error = I2C_wait_till_done(regs); /* wait until write is complete */
    if (error)
    {
        regs->MCS = 4; /* send STOP to release the bus */
        while (regs->MCS & 0x40U)
        {
        } /* wait for BUSBSY to clear */
        return (uint32_t)error;
    }

    /* send data one byte at a time */
    while (bytes_to_write > 1)
    {
        regs->MDR = *pdata++; /* write the next byte */
        regs->MCS = 1;        /* -data-ACK- */
        error     = I2C_wait_till_done(regs);
        if (error)
        {
            regs->MCS = 4; /* send STOP to release the bus */
            while (regs->MCS & 0x40U)
            {
            } /* wait for BUSBSY to clear */
            return (uint32_t)error;
        }
        bytes_to_write--;
        bytes_written++;
    }

    /* send last byte and a STOP */
    regs->MDR = *pdata++; /* write the last byte */
    regs->MCS = 5;        /* -data-ACK-P */
    error     = I2C_wait_till_done(regs);
    while (regs->MCS & 0x40)
        ; /* wait until bus is not busy */
    if (error)
        return error;
    return 0; /* no error */
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
uint32_t i2c_hal_read(const p_i2c_hal_t p_handle, const uint32_t slave_address, const uint32_t cmd_or_register,
                      const uint32_t cmd_or_register_len, uint8_t *const data, const uint_fast16_t data_len)
{
    int                error;
    uint_fast16_t      bytes_to_read = data_len;
    uint8_t           *pdata         = data;
    i2c_master_regs_t *regs          = p_handle->regs;

    if (data_len == 0)
        return (uint32_t)-1; /* no read was performed */

    /* send slave address and starting address */
    regs->MSA = slave_address << 1;
    regs->MDR = cmd_or_register;
    regs->MCS = 3; /* S-(saddr+w)-ACK-maddr-ACK */
    error     = I2C_wait_till_done(regs);
    if (error)
    {
        regs->MCS = 4; /* send STOP to release the bus */
        while (regs->MCS & 0x40U)
        {
        } /* wait for BUSBSY to clear */
        return (uint32_t)error;
    }

    /* to change bus from write to read, send restart with slave addr */
    regs->MSA = (slave_address << 1) + 1; /* restart: -R-(saddr+r)-ACK */

    if (bytes_to_read == 1) /* if last byte, don't ack */
        regs->MCS = 7;      /* -data-NACK-P */
    else                    /* else ack */
        regs->MCS = 0xB;    /* -data-ACK- */
    error = I2C_wait_till_done(regs);
    if (error)
    {
        regs->MCS = 4; /* send STOP to release the bus */
        while (regs->MCS & 0x40U)
        {
        } /* wait for BUSBSY to clear */
        return (uint32_t)error;
    }

    *pdata++ = regs->MDR; /* store the data received */

    if (--bytes_to_read == 0) /* if single byte read, done */
    {
        while (regs->MCS & 0x40)
            ;     /* wait until bus is not busy */
        return 0; /* no error */
    }

    /* read the rest of the bytes */
    while (bytes_to_read > 1)
    {
        regs->MCS = 9; /* -data-ACK- */
        error     = I2C_wait_till_done(regs);
        if (error)
        {
            regs->MCS = 4; /* send STOP to release the bus */
            while (regs->MCS & 0x40U)
            {
            } /* wait for BUSBSY to clear */
            return (uint32_t)error;
        }
        bytes_to_read--;
        *pdata++ = regs->MDR; /* store data received */
    }

    regs->MCS = 5; /* -data-NACK-P */
    error     = I2C_wait_till_done(regs);
    *pdata    = regs->MDR; /* store data received */
    while (regs->MCS & 0x40)
        ; /* wait until bus is not busy */

    return 0; /* no error */
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
    if (p_handle == NULL || p_handle->in_use_count == 0u)
    {
        return;
    }

    if (turn_on && !p_handle->enabled)
    {
        i2c_master_regs_t *regs    = p_handle->regs;
        uint32_t           clk_bit = (1U << p_handle->i2c_module_id);

        /* Enable GPIO port clocks */
        gpio_hal_init(p_handle->p_scl_gpio);
        gpio_hal_init(p_handle->p_sda_gpio);

        /* Enable I2C peripheral clock and wait for ready */
        SYSCTL_RCGCI2C_R |= clk_bit;
        while (!(SYSCTL_PRI2C_R & clk_bit))
        {
        }

        /* Configure SCL pin: alternate function 3 (I2C) */
        gpio_hal_pin_direction(p_handle->p_scl_gpio, p_handle->scl_pin, PIN_DIRECTION_ALT);
        gpio_hal_set_alt_function(p_handle->p_scl_gpio, p_handle->scl_pin, 3U);

        /* Configure SDA pin: alternate function 3 (I2C), open drain */
        gpio_hal_pin_direction(p_handle->p_sda_gpio, p_handle->sda_pin, PIN_DIRECTION_ALT);
        gpio_hal_set_alt_function(p_handle->p_sda_gpio, p_handle->sda_pin, 3U);
        gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, OPENDRAIN);

        /* Enable I2C master and configure clock for 100 kHz */
        regs->MCR = 0x0010;
        /* (1 + TIME_PERIOD) = SYS_CLK / (2 * (SCL_LP + SCL_HP) * I2C_CLK_Freq)
           TIME_PERIOD = 16,000,000 / (2(6+4) * 100000) - 1 = 7 */
        regs->MTPR = 0x07;

        p_handle->enabled = true;

        /* Allow bus devices time to stabilize after power-on */
        if (settle_time_ms > 0u)
        {
            clock_hal_delay(settle_time_ms);
        }
    }
    else if (!turn_on && p_handle->enabled)
    {
        i2c_master_regs_t *regs    = p_handle->regs;
        uint32_t           clk_bit = (1U << p_handle->i2c_module_id);

        /* Disable I2C master */
        regs->MCR = 0x0000;

        /* Reset SCL pin back to default (input, floating) */
        if (p_handle->p_scl_gpio != NULL)
        {
            gpio_hal_set_alt_function(p_handle->p_scl_gpio, p_handle->scl_pin, 0U);
            gpio_hal_pin_direction(p_handle->p_scl_gpio, p_handle->scl_pin, PIN_DIRECTION_INPUT);
            gpio_hal_pin_mode(p_handle->p_scl_gpio, p_handle->scl_pin, FLOAT);
        }

        /* Reset SDA pin back to default (input, floating) */
        if (p_handle->p_sda_gpio != NULL)
        {
            gpio_hal_set_alt_function(p_handle->p_sda_gpio, p_handle->sda_pin, 0U);
            gpio_hal_pin_direction(p_handle->p_sda_gpio, p_handle->sda_pin, PIN_DIRECTION_INPUT);
            gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, FLOAT);
        }

        /* Disable I2C peripheral clock */
        SYSCTL_RCGCI2C_R &= ~clk_bit;

        p_handle->enabled = false;
    }
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

    p_handle->callback.callback          = callback;
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

uint32_t i2c_hal_scan(const uint32_t i2c_board_id, const uint32_t sda_port, const uint32_t sda_pin,
                      const uint32_t scl_port, const uint32_t scl_pin, uint32_t last_found_address)
{
    p_i2c_hal_t p_handle = i2c_hal_create_device(i2c_board_id, sda_port, sda_pin, scl_port, scl_pin, NULL);
    if (p_handle == NULL)
    {
        return 0; // Failed to create I2C device
    }

    i2c_hal_enable(p_handle, true, 10U);

    uint8_t  chipdata[2];
    uint32_t cmd_data      = 0u;
    uint32_t start_address = (last_found_address < 0x08U) ? 0x08U : (last_found_address + 1u);
    for (uint32_t i = start_address; i <= 0x77U; i++)
    {
        if (i2c_hal_read(p_handle, i, cmd_data, 1u, chipdata, 1u) == 0u)
        {
            i2c_hal_remove_device(p_handle);
            return i; // Found a device at this address
        }
        clock_hal_delay(1U); // Small delay between scans
    }
    i2c_hal_remove_device(p_handle); // Clean up when no device found
    return 0;
}

#endif /* USE_I2C */
