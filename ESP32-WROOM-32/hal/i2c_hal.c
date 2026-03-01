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
 * @file i2c_hal.h
 * @brief Generic I2C interface definition.
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
 * Multiple include protection
 ********************************************************************************/

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "driver/i2c_master.h"
#include <esp_log.h>
#include <hw_config.h>
#include <i2c_hal.h>
#include <stdbool.h>
#include <stdint.h> /*lint -e129*/
#include <string.h>

#if HW_CONFIG_I2C == 1u

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    static const char *TAG             = "i2c_hal"; /*lint !e956*/
    static bool        i2c_initialized = false;     /*lint !e956*/

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

    /** @brief I2C callback context (mirrors the pattern used in gpio_hal.c) */
    typedef struct i2c_callback_context_s
    {
        p_i2c_callback_t callback;          /**< Registered interrupt callback */
        void            *p_callback_handle; /**< Pointer to callback context   */
        void            *callback_context;  /**< User callback context         */
    } i2c_callback_context_t;

    /** @brief Platform-specific I2C device configuration */
    typedef struct i2c_device_hal_s
    {
        i2c_hal_master_def_t *p_master; /**< Pointer to master bus definition */
    } i2c_device_hal_t;
    typedef i2c_device_hal_t *p_i2c_device_hal_t;

    struct i2c_hal
    {
        uint32_t               in_use_count;  /*!< Indicates if this I2C instance is in use */
        uint32_t               board_id;      /**< I2C board id (0, 1, 2) */
        bool                   enabled;       /**< true when hardware is powered on */
        i2c_device_hal_t       device_config; /**< I2C configuration parameters */
        i2c_callback_context_t callback;      /**< Registered interrupt callbacks */
    };

    static i2c_hal_t i2c_hal_pool[NUM_I2C_DEVICES] = {0};

    /********************************************************************************
     * Device handle cache – avoids repeated add/remove on the I2C master bus.
     ********************************************************************************/
#define I2C_DEV_CACHE_SIZE 4u

    typedef struct i2c_dev_cache_entry_s
    {
        uint16_t                slave_address;
        bool                    valid;
        i2c_master_dev_handle_t handle;
    } i2c_dev_cache_entry_t;

    static i2c_dev_cache_entry_t s_dev_cache[I2C_DEV_CACHE_SIZE] = {0};

    /** @brief Get (or create and cache) a device handle for the given slave address */
    static i2c_master_dev_handle_t get_or_create_dev_handle(uint32_t slave_address)
    {
        /* Search cache first */
        for (uint32_t i = 0u; i < I2C_DEV_CACHE_SIZE; i++)
        {
            if (s_dev_cache[i].valid && s_dev_cache[i].slave_address == (uint16_t)slave_address)
            {
                return s_dev_cache[i].handle;
            }
        }

        /* Not cached – create a new device handle on the master bus */
        i2c_device_config_t dev_cfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address  = (uint16_t)slave_address,
            .scl_speed_hz    = I2C_DEFAULT_SPEED,
        };
        i2c_master_dev_handle_t dev_handle = NULL;
        esp_err_t err = i2c_master_bus_add_device(i2c_master_device.i2c_bus_handle, &dev_cfg, &dev_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "i2c_master_bus_add_device failed for addr 0x%02lx: %s", (unsigned long)slave_address,
                     esp_err_to_name(err));
            return NULL;
        }

        /* Find a free cache slot */
        for (uint32_t i = 0u; i < I2C_DEV_CACHE_SIZE; i++)
        {
            if (!s_dev_cache[i].valid)
            {
                s_dev_cache[i].slave_address = (uint16_t)slave_address;
                s_dev_cache[i].handle        = dev_handle;
                s_dev_cache[i].valid         = true;
                return dev_handle;
            }
        }

        /* Cache full – evict the first entry */
        (void)i2c_master_bus_rm_device(s_dev_cache[0].handle);
        s_dev_cache[0].slave_address = (uint16_t)slave_address;
        s_dev_cache[0].handle        = dev_handle;
        s_dev_cache[0].valid         = true;
        return dev_handle;
    }

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /********************************************************************************
     * @brief          Initialization function for the I2C driver
     * @retval         None
     ********************************************************************************/
    void i2c_hal_initialize(void)
    {
        if (!i2c_initialized)
        {
            i2c_master_bus_config_t bus_config = {
                .sda_io_num        = (gpio_num_t)i2c_master_device.sda_port_num,
                .scl_io_num        = (gpio_num_t)i2c_master_device.scl_port_num,
                .clk_source        = I2C_CLK_SRC_DEFAULT,
                .i2c_port          = (i2c_port_num_t)i2c_master_device.device_id,
                .glitch_ignore_cnt = 7,
                .intr_priority     = 0,
            };
            bus_config.flags.enable_internal_pullup = true;

            esp_err_t ret = i2c_new_master_bus(&bus_config, &i2c_master_device.i2c_bus_handle);
            if (ret == ESP_OK)
            {
                i2c_master_device.is_initialized = true;
                i2c_initialized                  = true;
                ESP_LOGI(TAG, "I2C master bus initialized (SDA=%lu, SCL=%lu)",
                         (unsigned long)i2c_master_device.sda_port_num, (unsigned long)i2c_master_device.scl_port_num);
            }
            else
            {
                ESP_LOGE(TAG, "i2c_new_master_bus failed: %s", esp_err_to_name(ret));
            }
        }
    }

    /********************************************************************************
     * @brief          Check if the I2C driver is initialized
     * @retval         true if initialized, false otherwise
     ********************************************************************************/
    bool i2c_hal_is_initialized(void)
    {
        if (!i2c_initialized)
        {
            return false;
        }
        return true;
    }

    /********************************************************************************
     * @brief          Create an I2C device
     * @param[in]      i2c_board_id - The id of the I2C on the board (i.e 0, 1, 2..)
     * @param[in]      sda_port - the port used for the SDA line
     * @param[in]      sda_pin - the pin on the sda_port to use for the SDA line
     * @param[in]      scl_port - the port used for the SCL line
     * @param[in]      scl_pin - the pin on the scl_port to use for the SCL line
     * @param[in]      p_callback - Pointer to the callback function
     * @retval         Pointer to the I2C device
     ********************************************************************************/
    p_i2c_hal_t i2c_hal_create_device(const uint32_t i2c_board_id, const uint32_t sda_port, const uint32_t sda_pin,
                                      const uint32_t scl_port, const uint32_t scl_pin,
                                      const p_i2c_callback_t p_callback)
    {
        (void)sda_port;
        (void)sda_pin;
        (void)scl_port;
        (void)scl_pin;

        if (!i2c_hal_is_initialized())
        {
            i2c_hal_initialize();
            if (!i2c_hal_is_initialized())
            {
                return NULL;
            }
        }

        /* Look for an existing instance or a free slot */
        p_i2c_hal_t p_free_slot = NULL;
        for (uint32_t i = 0u; i < NUM_I2C_DEVICES; i++)
        {
            if (i2c_hal_pool[i].in_use_count > 0u)
            {
                if (i2c_hal_pool[i].board_id == i2c_board_id)
                {
                    i2c_hal_pool[i].in_use_count++;
                    return &i2c_hal_pool[i]; /* return existing */
                }
            }
            else if (p_free_slot == NULL)
            {
                p_free_slot = &i2c_hal_pool[i];
            }
        }

        if (p_free_slot != NULL)
        {
            p_free_slot->in_use_count               = 1u;
            p_free_slot->board_id                   = i2c_board_id;
            p_free_slot->enabled                    = true;
            p_free_slot->device_config.p_master     = &i2c_master_device;
            p_free_slot->callback.callback          = p_callback;
            p_free_slot->callback.p_callback_handle = NULL;
            p_free_slot->callback.callback_context  = NULL;
        }
        return p_free_slot;
    }

    /********************************************************************************
     * @brief          Remove an I2C device
     * @param[in]      p_handle - pointer to the i2c device
     * @retval         None
     ********************************************************************************/
    void i2c_hal_remove_device(p_i2c_hal_t p_handle)
    {
        if (p_handle != NULL && p_handle->in_use_count > 0u)
        {
            p_handle->in_use_count--;
            if (p_handle->in_use_count == 0u)
            {
                p_handle->enabled                    = false;
                p_handle->device_config.p_master     = NULL;
                p_handle->callback.callback          = NULL;
                p_handle->callback.p_callback_handle = NULL;
                p_handle->callback.callback_context  = NULL;
            }
        }
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
        if (p_handle == NULL || !i2c_initialized)
        {
            return 0u;
        }

        i2c_master_dev_handle_t dev_handle = get_or_create_dev_handle(slave_address);
        if (dev_handle == NULL)
        {
            return 0u;
        }

        /* Build transmit buffer: [register bytes] [data bytes] */
        uint32_t total_len = cmd_or_register_len + (uint32_t)data_len;
        if (total_len == 0u)
        {
            return 0u;
        }
        uint8_t tx_buf[2u + data_len]; /*lint !e9030 !e9033*/

        /* Pack register / command bytes (big-endian) */
        for (uint32_t i = 0u; i < cmd_or_register_len; i++)
        {
            tx_buf[i] = (uint8_t)(cmd_or_register >> (8u * (cmd_or_register_len - 1u - i)));
        }

        /* Append payload */
        if (data != NULL && data_len > 0u)
        {
            memcpy(&tx_buf[cmd_or_register_len], data, data_len);
        }

        esp_err_t err = i2c_master_transmit(dev_handle, tx_buf, total_len, -1);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "i2c_master_transmit failed (addr 0x%02lx): %s", (unsigned long)slave_address,
                     esp_err_to_name(err));
            return 0u;
        }
        return (uint32_t)data_len;
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
        if (p_handle == NULL || !i2c_initialized || data == NULL)
        {
            return 0u;
        }

        i2c_master_dev_handle_t dev_handle = get_or_create_dev_handle(slave_address);
        if (dev_handle == NULL)
        {
            return 0u;
        }

        esp_err_t err;
        if (cmd_or_register_len > 0u)
        {
            /* Write register address then read data (repeated-start) */
            uint8_t tx_buf[2];
            for (uint32_t i = 0u; i < cmd_or_register_len; i++)
            {
                tx_buf[i] = (uint8_t)(cmd_or_register >> (8u * (cmd_or_register_len - 1u - i)));
            }
            err = i2c_master_transmit_receive(dev_handle, tx_buf, cmd_or_register_len, data, (size_t)data_len, -1);
        }
        else
        {
            /* No register / command – pure read */
            err = i2c_master_receive(dev_handle, data, (size_t)data_len, -1);
        }

        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "i2c read failed (addr 0x%02lx): %s", (unsigned long)slave_address, esp_err_to_name(err));
            return 0u;
        }
        return (uint32_t)data_len;
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
        if (p_handle == NULL)
        {
            return;
        }

        if (turn_on && !i2c_initialized)
        {
            i2c_hal_initialize();
        }

        p_handle->enabled = turn_on;
        (void)settle_time_ms; /* ESP32 I2C has no power-gate; settle delay unused */
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
            return -1;
        }
        p_handle->callback.callback         = callback;
        p_handle->callback.callback_context = p_callback_context;
        return 0;
    }

#if USE_I2C_INTERRUPTS == 1
    /**
     * @brief  Handle an I2C interrupt
     * @retval None
     */
    void i2c_hal_irqhandler(void);
#endif

    /********************************************************************************
     * @brief          Scan for i2c devices on the bus
     * @param[in]      i2c_board_id - The id of the I2C on the board (i.e 0, 1, 2..)
     * @param[in]      sda_port - the port used for the SDA line
     * @param[in]      sda_pin - the pin on the sda_port to use for the SDA line
     * @param[in]      scl_port - the port used for the SCL line
     * @param[in]      scl_pin - the pin on the scl_port to use for the SCL line
     * @param[in]      last_found_address - the last address that was found (use 0 for initial scan)
     * @retval         address of the first detected device on the bus, or 0 if no devices are found
     ********************************************************************************/
    uint32_t i2c_hal_scan(const uint32_t i2c_board_id, const uint32_t sda_port, const uint32_t sda_pin,
                          const uint32_t scl_port, const uint32_t scl_pin, uint32_t last_found_address)
    {
        (void)i2c_board_id;
        (void)sda_port;
        (void)sda_pin;
        (void)scl_port;
        (void)scl_pin;

        if (!i2c_initialized)
        {
            i2c_hal_initialize();
            if (!i2c_initialized)
            {
                return 0u;
            }
        }

        /* Probe addresses starting after last_found_address up to 127 */
        for (uint32_t addr = last_found_address + 1u; addr <= 127u; addr++)
        {
            esp_err_t ret = i2c_master_probe(i2c_master_device.i2c_bus_handle, (uint16_t)addr, 50);
            if (ret == ESP_OK)
            {
                ESP_LOGI(TAG, "I2C device found at address 0x%02lx", (unsigned long)addr);
                return addr;
            }
        }
        return 0u;
    }

#endif /* USE_I2C */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
