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
 * @file spi_hal.c
 * @brief Generic SPI HAL interface implementation.
 * @details Portable SPI abstraction used to configure and interact with SPI
 * peripherals on ESP32 platform.
 *
 * @author  Jack Wilson
 * @date    May 20, 2026
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
#include "spi_hal.h"
#include "device_gpio.h"
#include "driver/spi_master.h"
#include <esp_log.h>
#include <hw_config.h>
#include <spi_hal.h>
#include <stdbool.h>
#include <stdint.h> /*lint -e129*/
#include <string.h>

#if HW_CONFIG_SPI == 1u

    /********************************************************************************
     * Defines
     ********************************************************************************/
#define SPI_DMA_CHAN SPI_DMA_CH_AUTO

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    static const char *TAG                 = "spi_hal"; /*lint !e956*/
    static bool        spi_bus_initialized = false;     /*lint !e956*/

    /** @brief SPI bus configuration for ESP32 */
    typedef struct spi_hal_bus_def_s
    {
        uint32_t          bus_id;         /*!< SPI bus id (SPI2_HOST, SPI3_HOST, etc.) */
        bool              is_initialized; /*!< Bus initialization status */
        spi_host_device_t host_id;        /*!< ESP32 SPI host device */
    } spi_hal_bus_def_t;

    /** @brief SPI callback context */
    typedef struct spi_callback_context_s
    {
        p_spi_callback_t callback;         /**< Registered interrupt callback */
        void            *callback_context; /**< User callback context         */
    } spi_callback_context_t;

    /** @brief Complete SPI device structure (opaque to users) */
    struct spi_hal
    {
        uint32_t            in_use_count;   /*!< Reference count */
        uint32_t            device_id;      /**< SPI device/bus ID */
        bool                enabled;        /**< Hardware enabled state */
        spi_device_handle_t spi_dev_handle; /**< ESP32 SPI devicu
// D6 PB10e handle */
        spi_hal_bus_def_t      bus_config;  /**< SPI bus configuration */
        spi_callback_context_t callback;    /**< Registered callbacks */
        uint32_t               baud_rate;   /**< Current baud rate */
    };

    /** @brief Pool of SPI device instances */
    static spi_hal_t spi_hal_pool[SPI_HAL_MAX_BUS] = {0};

    /** @brief Track which SPI buses have been initialized */
    static bool spi_buses_init[SPI_HAL_MAX_BUS] = {false};

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    static spi_host_device_t spi_get_host_from_id(uint32_t device_id);

    /********************************************************************************
     * @brief          Get ESP32 SPI host from device ID
     * @param[in]      device_id - SPI device ID (1-4)
     * @retval         ESP32 SPI host identifier
     ********************************************************************************/
    static spi_host_device_t spi_get_host_from_id(uint32_t device_id)
    {
        switch (device_id)
        {
#ifdef SPI1_HOST
        case SPI_BUS_1:
            return SPI1_HOST;
#endif
#ifdef SPI2_HOST
        case SPI_BUS_2:
            return SPI2_HOST;
#endif
#ifdef SPI3_HOST
        case SPI_BUS_3:
            return SPI3_HOST;
#endif
        default:
            return SPI2_HOST; /* Default fallback */
        }
    }

    /********************************************************************************
     * @brief          Initialization function for the SPI driver
     * @retval         None
     ********************************************************************************/
    void spi_hal_initialize(void)
    {
        if (!spi_bus_initialized)
        {
            /* Initialize all device pools */
            memset(spi_hal_pool, 0, sizeof(spi_hal_pool));
            memset(spi_buses_init, 0, sizeof(spi_buses_init));
            spi_bus_initialized = true;
            ESP_LOGI(TAG, "SPI HAL initialized");
        }
    }

    /********************************************************************************
     * @brief          Check if the SPI driver is initialized
     * @retval         true if initialized, false otherwise
     ********************************************************************************/
    bool spi_hal_is_bus_initialized(void)
    {
        return spi_bus_initialized;
    }

    /********************************************************************************
     * @brief          Create an SPI device
     * @param[in]      spi_device_id - Which SPI device to use (1-4)
     * @param[in]      p_callback - Pointer to the callback function
     * @retval         Pointer to the SPI device
     ********************************************************************************/
    p_spi_hal_t spi_hal_create_device(const uint32_t spi_device_id, p_spi_callback_t p_callback)
    {
        /* Ensure HAL is initialized */
        if (!spi_hal_is_bus_initialized())
        {
            spi_hal_initialize();
        }

        /* Validate device ID */
        if (spi_device_id == 0u || spi_device_id > SPI_HAL_MAX_BUS)
        {
            ESP_LOGE(TAG, "Invalid SPI device ID: %lu", (unsigned long)spi_device_id);
            return NULL;
        }

        uint32_t pool_idx = spi_device_id - 1u;

        /* Check if device already exists and is in use */
        if (spi_hal_pool[pool_idx].in_use_count > 0u)
        {
            if (spi_hal_pool[pool_idx].device_id == spi_device_id)
            {
                spi_hal_pool[pool_idx].in_use_count++;
                ESP_LOGI(TAG, "Reusing existing SPI device %lu (ref count: %lu)", (unsigned long)spi_device_id,
                         (unsigned long)spi_hal_pool[pool_idx].in_use_count);
                return &spi_hal_pool[pool_idx];
            }
        }

        /* Initialize new device */
        p_spi_hal_t p_device = &spi_hal_pool[pool_idx];

        /* Get ESP32 SPI host */
        spi_host_device_t host = spi_get_host_from_id(spi_device_id);

        /* Initialize SPI bus if not already done */
        if (!spi_buses_init[pool_idx])
        {
            spi_bus_config_t bus_cfg = {
                .miso_io_num     = -1, /* To be configured by user */
                .mosi_io_num     = -1, /* To be configured by user */
                .sclk_io_num     = -1, /* To be configured by user */
                .quadwp_io_num   = -1,
                .quadhd_io_num   = -1,
                .max_transfer_sz = 4096,
            };

            /* Note: In a full implementation, GPIO pins should be passed as parameters.
             * For now, using placeholder values - user must configure externally or
             * extend the API to accept pin configurations. */

            esp_err_t ret = spi_bus_initialize(host, &bus_cfg, SPI_DMA_CHAN);
            if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
            {
                ESP_LOGE(TAG, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
                return NULL;
            }
            spi_buses_init[pool_idx] = true;
            ESP_LOGI(TAG, "SPI bus %lu initialized", (unsigned long)spi_device_id);
        }

        /* Configure SPI device */
        spi_device_interface_config_t dev_cfg = {
            .mode           = 0,                 /* SPI mode 0 */
            .clock_speed_hz = SPI_HAL_BAUD_1MHz, /* Default 1 MHz */
            .spics_io_num   = -1,                /* CS pin to be configured */
            .queue_size     = 7,
            .pre_cb         = NULL,
            .post_cb        = NULL,
        };

        esp_err_t ret = spi_bus_add_device(host, &dev_cfg, &p_device->spi_dev_handle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "spi_bus_add_device failed: %s", esp_err_to_name(ret));
            return NULL;
        }

        /* Initialize device structure */
        p_device->in_use_count              = 1u;
        p_device->device_id                 = spi_device_id;
        p_device->enabled                   = true;
        p_device->bus_config.bus_id         = spi_device_id;
        p_device->bus_config.host_id        = host;
        p_device->bus_config.is_initialized = true;
        p_device->callback.callback         = p_callback;
        p_device->callback.callback_context = NULL;
        p_device->baud_rate                 = SPI_HAL_BAUD_1MHz;

        ESP_LOGI(TAG, "SPI device %lu created successfully", (unsigned long)spi_device_id);
        return p_device;
    }

    /********************************************************************************
     * @brief          Remove an SPI device
     * @param[in]      p_handle - pointer to the SPI device
     * @retval         None
     ********************************************************************************/
    void spi_hal_remove_device(p_spi_hal_t p_handle)
    {
        if (p_handle != NULL && p_handle->in_use_count > 0u)
        {
            p_handle->in_use_count--;
            ESP_LOGI(TAG, "SPI device ref count decreased to %lu", (unsigned long)p_handle->in_use_count);

            if (p_handle->in_use_count == 0u)
            {
                /* Remove device from bus */
                if (p_handle->spi_dev_handle != NULL)
                {
                    esp_err_t ret = spi_bus_remove_device(p_handle->spi_dev_handle);
                    if (ret != ESP_OK)
                    {
                        ESP_LOGE(TAG, "spi_bus_remove_device failed: %s", esp_err_to_name(ret));
                    }
                    p_handle->spi_dev_handle = NULL;
                }

                /* Clear device state */
                p_handle->enabled                   = false;
                p_handle->callback.callback         = NULL;
                p_handle->callback.callback_context = NULL;

                ESP_LOGI(TAG, "SPI device %lu removed", (unsigned long)p_handle->device_id);
            }
        }
    }

    /********************************************************************************
     * @brief          Write function for the SPI driver
     * @param[in]      p_spi_device - pointer to the SPI device
     * @param[in]      data -  pointer to data to send
     * @param[in]      data_len - Length of data to write in bytes
     * @retval         None
     ********************************************************************************/
    void spi_hal_write(const p_spi_hal_t p_spi_device, const uint8_t *const data, const uint16_t data_len)
    {
        if (p_spi_device == NULL || data == NULL || data_len == 0u)
        {
            ESP_LOGW(TAG, "Invalid write parameters");
            return;
        }

        if (!p_spi_device->enabled || p_spi_device->spi_dev_handle == NULL)
        {
            ESP_LOGE(TAG, "SPI device not enabled or invalid handle");
            return;
        }

        spi_transaction_t trans = {
            .length    = data_len * 8u, /* Length in bits */
            .tx_buffer = data,
            .rx_buffer = NULL,
        };

        esp_err_t ret = spi_device_transmit(p_spi_device->spi_dev_handle, &trans);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "spi_device_transmit failed: %s", esp_err_to_name(ret));
        }
    }

    /********************************************************************************
     * @brief          Read function for the SPI driver
     * @param[in]      p_spi_device - SPI device pointer
     * @param[out]     data -  pointer to buffer for received data
     * @param[in]      data_len - Length of data to read in bytes
     * @retval         Number of bytes read
     ********************************************************************************/
    uint16_t spi_hal_read(const p_spi_hal_t p_spi_device, uint8_t *data, const uint16_t data_len)
    {
        if (p_spi_device == NULL || data == NULL || data_len == 0u)
        {
            ESP_LOGW(TAG, "Invalid read parameters");
            return 0u;
        }

        if (!p_spi_device->enabled || p_spi_device->spi_dev_handle == NULL)
        {
            ESP_LOGE(TAG, "SPI device not enabled or invalid handle");
            return 0u;
        }

        spi_transaction_t trans = {
            .length    = data_len * 8u, /* Length in bits */
            .tx_buffer = NULL,
            .rx_buffer = data,
        };

        esp_err_t ret = spi_device_transmit(p_spi_device->spi_dev_handle, &trans);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "spi_device_transmit (read) failed: %s", esp_err_to_name(ret));
            return 0u;
        }

        return data_len;
    }

    /********************************************************************************
     * @brief  Enable or disable SPI
     * @param[in]      p_spi_device - pointer to the SPI device
     * @param[in]      turn_on - enable or disable the SPI device
     * @retval         None
     ********************************************************************************/
    void spi_hal_enable(const p_spi_hal_t p_spi_device, const bool turn_on)
    {
        if (p_spi_device != NULL)
        {
            p_spi_device->enabled = turn_on;
            ESP_LOGI(TAG, "SPI device %lu %s", (unsigned long)p_spi_device->device_id,
                     turn_on ? "enabled" : "disabled");
        }
    }

    /********************************************************************************
     * @brief  Set the SPI baud rate
     * @param[in]      p_spi_device - pointer to the SPI device
     * @param[in]      baud_rate - baud rate to set (in Hz)
     * @retval         None
     ********************************************************************************/
    void spi_hal_set_baud(const p_spi_hal_t p_spi_device, const uint32_t baud_rate)
    {
        if (p_spi_device == NULL || p_spi_device->spi_dev_handle == NULL)
        {
            ESP_LOGE(TAG, "Invalid SPI device for baud rate change");
            return;
        }

        /* Note: ESP32 SPI driver doesn't support changing baud rate after device
         * is added. To properly support this, we would need to:
         * 1. Remove the device
         * 2. Re-add with new config
         *
         * For now, we just store the requested baud rate. In a full implementation,
         * consider recreating the device with new settings.
         */

        p_spi_device->baud_rate = baud_rate;
        ESP_LOGI(TAG, "SPI baud rate set to %lu Hz (note: requires device recreation to take effect)",
                 (unsigned long)baud_rate);

        /* TODO: Implement proper baud rate change by recreating device */
    }

#if USE_SPI_INTERRUPTS == 1
    /********************************************************************************
     * @brief  Handle an SPI interrupt
     * @retval None
     ********************************************************************************/
    void spi_hal_irqhandler(void)
    {
        /* ESP32 SPI master driver handles interrupts internally.
         * If custom interrupt handling is needed, implement here.
         * Iterate through active devices and call their callbacks if registered.
         */
        for (uint32_t i = 0u; i < SPI_HAL_MAX_BUS; i++)
        {
            if (spi_hal_pool[i].in_use_count > 0u && spi_hal_pool[i].callback.callback != NULL)
            {
                spi_hal_pool[i].callback.callback();
            }
        }
    }
#endif

#endif // HW_CONFIG_SPI

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
