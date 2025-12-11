/******************************************************************************
 * Copyright (C) 2025 by Jack Wilson
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
 ************************(C) COPYRIGHT 2025 Jack Wilson **********************/
/**
 * @file i2c_hal.c
 * @brief Generic I2C HAL interface definition.
 * @details Outlines the portable I2C abstraction used to configure and
 * interact with I2C peripherals on any supported platform.
 *
 * @author  Jack Wilson
 * @date    September 26, 2025
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "driver/i2c_master.h"
#include <assert.h>
#include <gpio_hal.h>
#include <hw_config.h>
#include <i2c_hal.h>
#include <stdbool.h>
#include <stdlib.h> /*lint -e129*/
#include <string.h>

    void    i2c_hal_write(const void *p_i2c_device, const uint32_t cmd_or_register, const uint32_t cmd_or_register_len,
                          const uint8_t *const data, const uint_fast16_t data_len);
    uint8_t i2c_hal_read(const void *p_i2c_device, const uint32_t cmd_or_register, const uint32_t cmd_or_register_len,
                         uint8_t *const data, const uint_fast16_t data_len);
    void    i2c_hal_enable(const void *p_i2c_device, const bool turn_on, const uint32_t settle_time_ms);

    // static i2c_master_dev_handle_t i2c_master_device_handle;
    /*lint -esym(793,__*)*/
    /*lint -e749 -e754 -e751 -e750 -e9026*/
    /********************************************************************************
     * Defines
     ********************************************************************************/
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
#if HW_CONFIG_I2C == 1u
    static bool i2c_initialized = false; /*lint !e956*/

    typedef struct i2c_hal_master_def_s
    {
        uint32_t                device_id; /*!< bus id */
        uint32_t                sda_port_num;
        uint32_t                scl_port_num;
        bool                    is_initialized;
        i2c_master_bus_handle_t i2c_bus_handle; /*!< I2C configuration */
    } i2c_hal_master_def_t;

    static i2c_hal_master_def_t i2c_master_device = {
        .device_id      = I2C_BUS,
        .sda_port_num   = HW_CONFIG_I2C_SDA_PORT,
        .scl_port_num   = HW_CONFIG_I2C_SCL_PORT,
        .is_initialized = false,
    };

    /*
        typedef struct i2c_hal_device_def_s
        {
            i2c_hal_master_def_t   *p_master;       //!< pointer to master device
            uint32_t                device_address; //!< device address
            uint32_t                address_length; //!< device address length
            p_i2c_callback_t        p_callback;     //!< callback function
            i2c_master_dev_handle_t device_handle;  //!< I2C device handle
        } i2c_hal_device_def_t;
    */
#if HW_CONFIG_I2C_NUM_DEVICES >= 1
    static i2c_device_hal_t i2c_device1 = {.p_master       = &i2c_master_device,
                                           .device_address = HW_CONFIG_I2C1_ADDRESS,
                                           .address_length = I2C_ADDR_BIT_LEN_7,
                                           .p_callback     = NULL,
                                           .write          = i2c_hal_write,
                                           .read           = i2c_hal_read,
                                           .enable         = i2c_hal_enable};
#endif
#if HW_CONFIG_I2C_NUM_DEVICES >= 2
    static i2c_device_hal_t i2c_device2 = {.p_master       = &i2c_master_device,
                                           .device_address = HW_CONFIG_I2C2_ADDRESS,
                                           .address_length = I2C_ADDR_BIT_LEN_7,
                                           .p_callback     = NULL,
                                           .write          = i2c_hal_write,
                                           .read           = i2c_hal_read,
                                           .enable         = i2c_hal_enable};
#else
#pragma message("Too Many I2C Devices Defined!")
#endif

    // static p_gpio_hal_def_t p_i2c_enable = NULL; /*lint !e956*/

    // static p_gpio_hal_def_t p_scl_gpio = NULL; /*lint !e956*/

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /********************************************************************************/

    void i2c_hal_initialize(const uint32_t speed)
    {
        if (i2c_initialized)
        {
            return;
        }

        i2c_master_bus_config_t bus_config = {
            .sda_io_num        = (gpio_num_t)i2c_master_device.sda_port_num,
            .scl_io_num        = (gpio_num_t)i2c_master_device.scl_port_num,
            .clk_source        = I2C_CLK_SRC_DEFAULT,
            .i2c_port          = I2C_NUM_0,
            .glitch_ignore_cnt = 7,
            .intr_priority     = 1,
        };

        i2c_new_master_bus(&bus_config, &i2c_master_device.i2c_bus_handle);
        i2c_initialized                  = true;
        i2c_master_device.is_initialized = true;
    }

    /********************************************************************************/
    bool i2c_hal_is_initialized(void)
    {
        return i2c_initialized;
    }

    /********************************************************************************/
    void *i2c_hal_create_device(const uint32_t i2c_device_id, const p_i2c_callback_t p_callback)
    {
        i2c_device_hal_t *p_i2c_device = NULL;
        switch (i2c_device_id)
        {
#if HW_CONFIG_I2C_NUM_DEVICES >= 1
        case 1u:
            p_i2c_device = &i2c_device1;
            break;

#endif
#if HW_CONFIG_I2C_NUM_DEVICES >= 2
        case 2u:
            p_i2c_device = &i2c_device2;
            break;
#endif
        default:

            return NULL;
        }
        if (!i2c_initialized)
        {
            i2c_hal_initialize(100000U);
        }
        i2c_hal_master_def_t *p_master         = &i2c_master_device;
        p_i2c_device->p_master                 = p_master;
        p_i2c_device->p_callback               = p_callback;
        i2c_device_config_t i2c_mst_dev_config = {
            .dev_addr_length = (i2c_addr_bit_len_t)p_i2c_device->address_length,
            .device_address  = (uint16_t)p_i2c_device->device_address,
            .scl_speed_hz    = 100000U,
            .scl_wait_us     = 0,
            .flags =
                {
                    .disable_ack_check = 0,
                },
        };

        i2c_master_bus_add_device(p_master->i2c_bus_handle, &i2c_mst_dev_config, &i2c_master_device_handle);
        p_i2c_device->i2c_device_handle = &i2c_master_device_handle;

        return p_i2c_device;
    }

    /********************************************************************************/

    void i2c_hal_write(const void *p_i2c_device, const uint32_t cmd_or_register, const uint32_t cmd_or_register_len,
                       const uint8_t *const data, const uint_fast16_t data_len)
    {

        const uint8_t           chipdata[2] = {(uint8_t)(cmd_or_register >> 8), (uint8_t)cmd_or_register};
        uint32_t                data_size   = data_len + cmd_or_register_len;
        uint8_t                 tx_data[2 + data_len];
        i2c_device_hal_t       *p_i2c_dev      = (i2c_device_hal_t *)p_i2c_device;
        i2c_master_dev_handle_t i2c_dev_handle = *(i2c_master_dev_handle_t *)p_i2c_dev->i2c_device_handle;

        /* Prepare data to send */

        if (cmd_or_register_len > 0)
        {
            tx_data[0] = chipdata[cmd_or_register_len & 1u];
            if (cmd_or_register_len == 2u)
            {
                tx_data[1] = chipdata[0];
                memcpy(&tx_data[2], data, data_len);
            }
            else
            {
                memcpy(&tx_data[1], data, data_len);
            }
        }
        else
        {
            memcpy(&tx_data[0], data, data_len);
        }

        ESP_ERROR_CHECK(i2c_master_transmit(i2c_dev_handle, tx_data, data_size, 5000)); // -1 for infinite timeout

        /*
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (p_i2c_device->device_id << 1) | WRITE_BIT, ACK_CHECK_EN);
        if (cmd_or_register_len > 0)
        {
            i2c_master_write(cmd, chipdata + (cmd_or_register_len & 1u), cmd_or_register_len, ACK_CHECK_EN);
        }
        if (data_len > 0)
        {
            i2c_master_write(cmd, data, data_len, ACK_CHECK_EN);
        }
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        */
        // return data_len;
    } /*lint !e818*/

    /********************************************************************************
     * @brief          Read function for the I2C driver
     * @param[in]      i2c_device_id - I2C device ID
     * @param[in]      cmd_or_register - The memory address in the i2c device (i.e. registers)
     * @param[in]      data -  pointer to data (in bytes!) to read
     * @param[in]      data_len - Length of data to read in bytes
     * @retval         Number of bytes read
     ********************************************************************************/

    uint8_t i2c_hal_read(const void *p_i2c_device, const uint32_t cmd_or_register, const uint32_t cmd_or_register_len,
                         uint8_t *const data, const uint_fast16_t data_len)
    {
        const uint8_t           chipdata[2]    = {(uint8_t)(cmd_or_register >> 8), (uint8_t)cmd_or_register};
        int                     ret            = 0;
        i2c_device_hal_t       *p_i2c_dev      = (i2c_device_hal_t *)p_i2c_device;
        i2c_master_dev_handle_t i2c_dev_handle = *(i2c_master_dev_handle_t *)p_i2c_dev->i2c_device_handle;
        uint8_t                 tx[2];
        // const uint8_t           tx[2]          = {(uint8_t)(cmd_or_register >> 8), (uint8_t)cmd_or_register};
        if (cmd_or_register_len > 0)
        {
            tx[0] = chipdata[cmd_or_register_len & 1u];
            if (cmd_or_register_len == 2u)
            {
                tx[1] = chipdata[0];
                // memcpy(&tx_data[2], data, data_len);
            }
            else
            {
                // memcpy(&tx_data[1], data, data_len);
            }
        }
        else
        {
            // memcpy(&tx_data[0], data, data_len);
        }
        // ESP_ERROR_CHECK(i2c_master_receive(i2c_dev_handle, data, data_len, 500));
        ESP_ERROR_CHECK(i2c_master_transmit_receive(i2c_dev_handle, tx, cmd_or_register_len, data, data_len, 500));

        /*
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (p_i2c_device->device_id << 1) | WRITE_BIT, ACK_CHECK_EN);
        if (cmd_or_register_len > 0)
        {
            i2c_master_write(cmd, chipdata + (cmd_or_register_len & 1u), cmd_or_register_len, ACK_CHECK_EN);
        }
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        if (data_len >= 1)
        {
            vTaskDelay(30 / portTICK_PERIOD_MS);
            cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (p_i2c_device->device_id << 1) | READ_BIT, ACK_CHECK_EN);
            if (data_len > 1)
            {
                i2c_master_read(cmd, data, data_len - 1, ACK_VAL);
            }
            i2c_master_read_byte(cmd, data + data_len - 1, NACK_VAL);
            i2c_master_stop(cmd);
            ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
            i2c_cmd_link_delete(cmd);
        }
        */
        if (ret == ESP_OK)
        {
            return data_len;
        }
        return 0;
    } /*lint !e818*/

    /********************************************************************************
     * @brief  Enable (power on) or disable (power off) to the i2c devices
     * @retval
     ********************************************************************************/
    void i2c_hal_enable(const void *p_i2c_device, const bool turn_on, const uint32_t settle_time_us)
    {

        // clock_hal_delay(settle_time_us);
    }

#endif // HW_CONFIG_I2C

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
