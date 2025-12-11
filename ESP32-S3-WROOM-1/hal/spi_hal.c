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
 * @file gpio_hal.c
 * @brief Generic GPIO HAL interface definition.
 * @details Outlines the portable GPIO abstraction used to configure and
 * interact with digital pins on any supported platform.
 *
 * @author  Jack Wilson
 * @date    September 26, 2025
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Jack Wilson
 * @license MIT License
 */
#include <driver/spi_master.h>
#include <esp_log.h>
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /*lint -e129*/
#include <string.h>
#define PARALLEL_LINES 16

#if HW_CONFIG_SPI == 1u
#include "driver/spi_master.h"

#include <spi_hal.h>

void     spi_hal_write(const void *p_spi_device, const uint8_t *const data, const uint16_t data_len);
uint16_t spi_hal_read(const void *p_spi_device, uint8_t *data, const uint16_t data_len);
void     spi_hal_enable(const void *p_spi_device, const bool turn_on);
// void     spi_hal_chip_select(const void *p_spi_device, const bool turn_on);
void spi_hal_set_baud(const void *p_spi_device, const uint32_t baud_rate);

/********************************************************************************
 * Typedefs & Enums
 ********************************************************************************/

typedef struct spi_hal_bus_s
{
    uint32_t         bus_id; /*!< bus id */
    bool             is_initialized;
    spi_bus_config_t spi_bus_handle; /*!< SPI configuration */

} spi_hal_bus_t;
static spi_device_handle_t spi_device_handles[HW_CONFIG_SPI_NUM_DEVICES + 1];

static spi_hal_bus_t spi_buses[SPI_USE_BUS_1 + SPI_USE_BUS_2 + SPI_USE_BUS_3 + SPI_USE_BUS_4 + 1] = {
#if SPI_USE_BUS_1 == 1u
    {SPI_BUS_1,
     false,
     {.miso_io_num     = HW_CONFIG_SPI1_MISO_PORT,
      .mosi_io_num     = HW_CONFIG_SPI1_MOSI_PORT,
      .sclk_io_num     = HW_CONFIG_SPI1_SCLK_PORT,
      .quadwp_io_num   = -1,
      .quadhd_io_num   = -1,
      .max_transfer_sz = PARALLEL_LINES * 320 * 2 + 8}},
#endif
#if SPI_USE_BUS_2 == 1u
    {SPI_BUS_2,
     false,
     {.miso_io_num     = HW_CONFIG_SPI2_MISO_PORT,
      .mosi_io_num     = HW_CONFIG_SPI2_MOSI_PORT,
      .sclk_io_num     = HW_CONFIG_SPI2_SCLK_PORT,
      .quadwp_io_num   = -1,
      .quadhd_io_num   = -1,
      .max_transfer_sz = PARALLEL_LINES * 320 * 2 + 8}},
#endif
#if SPI_USE_BUS_3 == 1u
    {SPI_BUS_3,
     false,
     {.miso_io_num     = HW_CONFIG_SPI3_MISO_PORT,
      .mosi_io_num     = HW_CONFIG_SPI3_MOSI_PORT,
      .sclk_io_num     = HW_CONFIG_SPI3_SCLK_PORT,
      .quadwp_io_num   = -1,
      .quadhd_io_num   = -1,
      .max_transfer_sz = PARALLEL_LINES * 320 * 2 + 8}},
#endif
#if SPI_USE_BUS_4 == 1u
    {SPI_BUS_4,
     false,
     {.miso_io_num     = HW_CONFIG_SPI4_MISO_PORT,
      .mosi_io_num     = HW_CONFIG_SPI4_MOSI_PORT,
      .sclk_io_num     = HW_CONFIG_SPI4_SCLK_PORT,
      .quadwp_io_num   = -1,
      .quadhd_io_num   = -1,
      .max_transfer_sz = PARALLEL_LINES * 320 * 2 + 8}},
#endif
};

#if HW_CONFIG_SPI_NUM_DEVICES >= 1

static spi_hal_device_t *p_spi_devices[HW_CONFIG_SPI_NUM_DEVICES] = {
#if SPI_USE_DEVICE_1 == 1u
    &(spi_hal_device_t){
        .device_id       = SPI1_BUS_ID,
        .is_initialized  = false,
        .p_callback      = NULL,
        .p_spi_bus       = NULL,
        .p_device_handle = NULL,
        .write           = spi_hal_write,
        .read            = spi_hal_read,
        .enable          = spi_hal_enable,
        .set_baud        = spi_hal_set_baud,
    },
#endif
};

#endif
void *spi_hal_initialize(uint32_t bus_id)
{
    esp_err_t      ret;
    spi_hal_bus_t *p_spi_bus = NULL;
    // assert(p_spi_device->p_spi != NULL);
    // assert(p_spi_device->p_cs_gpio != NULL);
    //        assert(p_spi_device->p_callback != NULL);
    switch (bus_id)
    {
#if SPI_USE_BUS_1 == 1u
    case SPI_BUS_1:
        p_spi_bus = &spi_buses[0];
        break;
#endif
#if SPI_USE_BUS_2 == 1u
    case SPI_BUS_2:
        p_spi_bus = &spi_buses[SPI_USE_BUS_1];
        break;
#endif
#if SPI_USE_BUS_3 == 1u
    case SPI_BUS_3:
        p_spi_bus = &spi_buses[SPI_USE_BUS_1 + SPI_USE_BUS_2];
        break;
#endif
#if SPI_USE_BUS_4 == 1u
    case SPI_BUS_4:
        p_spi_bus = &spi_buses[SPI_USE_BUS_1 + SPI_USE_BUS_2 + SPI_USE_BUS_3];
        break;
#endif
    default:
        return NULL;
    }

    if (p_spi_bus->is_initialized == true)
    {
        return NULL; // already done
    }
    ESP_LOGI("SPI", "%s on core %d\n", __func__, xPortGetCoreID());

    spi_bus_config_t buscfg;
    // spi_device_interface_config_t devcfg;

    // printf("spi_hal bus config\n");
    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.miso_io_num     = p_spi_bus->spi_bus_handle.miso_io_num;
    buscfg.mosi_io_num     = p_spi_bus->spi_bus_handle.mosi_io_num;
    buscfg.sclk_io_num     = p_spi_bus->spi_bus_handle.sclk_io_num;
    buscfg.quadwp_io_num   = p_spi_bus->spi_bus_handle.quadwp_io_num;
    buscfg.quadhd_io_num   = p_spi_bus->spi_bus_handle.quadhd_io_num;
    buscfg.max_transfer_sz = p_spi_bus->spi_bus_handle.max_transfer_sz;
    buscfg.flags           = 0;
    // buscfg.flags = ESP_INTR_FLAG_SHARED;

    // ret = spi_bus_initialize(p_spi_bus->bus_id, &buscfg, SPI_DMA_CH_AUTO);

    // ret = spi_bus_initialize(bus_id, &buscfg, SPI_DMA_CH_AUTO);
    ret = spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_LOGI("SPI", "%s bus_init %d\n", __func__, ret);

    p_spi_bus->is_initialized = true;
    return p_spi_bus;
}

/********************************************************************************
 * @brief          Check if the SPI driver is initialized
 * @retval         true if initialized, false otherwise
 ********************************************************************************/
bool spi_hal_is_bus_initialized(const uint32_t bus_id)
{
    spi_hal_bus_t *p_spi_bus = NULL;

    switch (bus_id)
    {
#if SPI_USE_BUS_1 == 1u
    case SPI_BUS_1:
        p_spi_bus = &spi_buses[0];
        break;
#endif
#if SPI_USE_BUS_2 == 1u
    case SPI_BUS_2:
        p_spi_bus = &spi_buses[SPI_USE_BUS_1];
        break;
#endif
#if SPI_USE_BUS_3 == 1u
    case SPI_BUS_3:
        p_spi_bus = &spi_buses[SPI_USE_BUS_1 + SPI_USE_BUS_2];
        break;
#endif
#if SPI_USE_BUS_4 == 1u
    case SPI_BUS_4:
        p_spi_bus = &spi_buses[SPI_USE_BUS_1 + SPI_USE_BUS_2 + SPI_USE_BUS_3];
        break;
#endif
    default:
        return false;
    }

    return p_spi_bus->is_initialized;
}

/********************************************************************************
 * @brief          Create an SPI device
 * @param[in]      spi_device_id - Which SPI device to use
 * @param[in]      p_callback - Pointer to the callback function
 * @retval         Pointer to the I2C device
 ********************************************************************************/
spi_hal_device_t *spi_hal_create_device(const uint32_t spi_device_id, void *spi_bus, p_spi_callback_t p_callback)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI("SPI", "%s\n", __func__);

    spi_hal_device_t             *p_spi_device = NULL;
    spi_device_interface_config_t i2c_mst_dev_config;
    switch (spi_device_id)
    {
#if SPI_USE_DEVICE_1 == 1u
    case SPI_DEVICE_1:
        p_spi_device = p_spi_devices[0];
        memset(&i2c_mst_dev_config, 0, sizeof(i2c_mst_dev_config));
        i2c_mst_dev_config.clock_speed_hz = SPI1_BAUD_RATE;
        i2c_mst_dev_config.mode           = SPI1_MODE;
#if AUTO_CS == 1u
        i2c_mst_dev_config.spics_io_num = HW_CONFIG_SPI1_CS_PORT;
#else
        i2c_mst_dev_config.spics_io_num = -1;
#endif
        i2c_mst_dev_config.queue_size = SPI1_QUEUE_SIZE;
        i2c_mst_dev_config.pre_cb     = NULL;
        // i2c_mst_dev_config.post_cb        = NULL;
        break;
#endif
#if SPI_USE_DEVICE_2 == 1u
    case SPI_DEVICE_2:
        p_spi_device                      = &p_spi_devices[1];
        i2c_mst_dev_config.clock_speed_hz = SPI2_BAUD_RATE;
        i2c_mst_dev_config.mode           = 0;
        i2c_mst_dev_config.spics_io_num   = SPI2_CS_PORT;
        i2c_mst_dev_config.queue_size     = SPI2_QUEUE_SIZE;
        break;
#endif
#if SPI_USE_DEVICE_3 == 1u
    case SPI_DEVICE_3:
        p_spi_device                      = &p_spi_devices[2];
        i2c_mst_dev_config.clock_speed_hz = SPI3_BAUD_RATE;
        i2c_mst_dev_config.mode           = 0;
        i2c_mst_dev_config.spics_io_num   = SPI3_CS_PORT;
        i2c_mst_dev_config.queue_size     = SPI3_QUEUE_SIZE;
        break;
#endif
#if SPI_USE_DEVICE_4 == 1u
    case SPI_DEVICE_4:
        p_spi_device                      = &p_spi_devices[3];
        i2c_mst_dev_config.clock_speed_hz = SPI4_BAUD_RATE;
        i2c_mst_dev_config.mode           = 0;
        i2c_mst_dev_config.spics_io_num   = SPI4_CS_PORT;
        i2c_mst_dev_config.queue_size     = SPI4_QUEUE_SIZE;
        break;
#endif
    default:
        return NULL;
    }
    p_spi_device->device_id  = spi_device_id;
    p_spi_device->p_spi_bus  = spi_bus;
    p_spi_device->p_callback = p_callback;
    // p_spi_device->p_cs_gpio = p_cs_gpio;
    /*
        spi_device_interface_config_t devcfg = {
    #ifdef CONFIG_LCD_OVERCLOCK
            .clock_speed_hz = 26 * 1000 * 1000, // Clock out at 26 MHz
    #else
            .clock_speed_hz = 10 * 1000 * 1000, // Clock out at 10 MHz
    #endif
            .mode         = 0,      // SPI mode 0
            .spics_io_num = cs_pin, // CS pin
            .queue_size   = 7,      // We want to be able to queue 7 transactions at a time
            .pre_cb       = NULL,   // lcd_spi_pre_transfer_callback, // Specify pre-transfer callback
                                    //  to handle D/C line
        };
    */
    p_spi_device->is_initialized = false;
    // ret                          = spi_bus_add_device(SPI2_HOST, &devcfg, &spi_device_handle);
    //  ret = spi_bus_add_device(SPI2_HOST, &i2c_mst_dev_config, &spi_device_handle);

    // p_spi_device->p_device_handle = &spi_device_handle;

    // ret = spi_bus_add_device(spi_device_id, &i2c_mst_dev_config, &spi_device_handles[spi_device_id]);
    ret = spi_bus_add_device(SPI_HOST, &i2c_mst_dev_config, &spi_device_handles[spi_device_id]);

    p_spi_device->p_device_handle = &spi_device_handles[spi_device_id];

    ESP_LOGI("SPI", "%s bus_add Device %d\n", __func__, ret);
    if (ret != ESP_OK)
    {
        return NULL;
    }
    p_spi_device->is_initialized = true;
    return p_spi_device;
}

/********************************************************************************
 * @brief          Write function for the SPI driver
 * @param[in]      p_spi_device - pointer to the SPI device
 * @param[in]      data -  pointer to data to send (may be words depending on MCU)
 * @param[in]      data_len - Length of data to write in bytes
 * @retval         Number of bytes written
 ********************************************************************************/

void spi_hal_write(const void *p_spi_device, const uint8_t *const data, const uint16_t data_len)
{
    spi_hal_device_t   *p_device          = (spi_hal_device_t *)p_spi_device;
    spi_device_handle_t spi_device_handle = *(spi_device_handle_t *)p_device->p_device_handle;
    esp_err_t           ret;
    spi_transaction_t   t;
    // printf_hal("%s\n", __func__);
    // spi_device_acquire_bus(spi_device_handle, portMAX_DELAY);
    if (data_len == 0)
    {
        return; // nothing to do
    }
    memset(&t, 0, sizeof(t));

    t.length    = data_len * 8; //
    t.tx_buffer = data;         //
    t.user      = (void *)0u;
    t.flags     = 0; // D/C needs to be set to 1
    // ret                    = spi_device_transmit(spi_device_handle, &t); //
    ret = spi_device_polling_transmit(spi_device_handle, &t); //
    // spi_device_release_bus(spi_device_handle);
    assert(ret == ESP_OK);
}

/********************************************************************************
 * @brief          Read function for the SPI driver
 * @param[in]      spi_device_id - SPI device ID
 * @param[in]      data -  pointer to data (in bytes!) to read
 * @param[in]      data_len - Length of data to read in bytes
 * @retval         Number of bytes read
 ********************************************************************************/
uint16_t spi_hal_read(const void *p_spi_device, uint8_t *data, const uint16_t data_len)
{
    spi_hal_device_t   *p_device          = (spi_hal_device_t *)p_spi_device;
    spi_device_handle_t spi_device_handle = *(spi_device_handle_t *)p_device->p_device_handle;
    spi_transaction_t   t;
    // printf_hal("%s\n", __func__);
    spi_device_acquire_bus(spi_device_handle, portMAX_DELAY);
    memset(&t, 0, sizeof(t));
    t.length = data_len * 8;
    t.flags  = SPI_TRANS_USE_RXDATA;
    t.user   = (void *)1u;
    // esp_err_t ret = spi_device_transmit(p_device->p_device_handle, &t);
    esp_err_t ret = spi_device_polling_transmit(spi_device_handle, &t);
    memcpy(data, t.rx_data, data_len);
    spi_device_release_bus(spi_device_handle);
    if (ret != ESP_OK)
    {
        return 0;
    }
    else
    {
        return data_len;
    }
}

/********************************************************************************
 * @brief  Enable or disable SPI
 * @param[in]      turn_on - enable or disable the SPI devices
 * @retval         None
 ********************************************************************************/
void spi_hal_enable(const void *p_spi_device, const bool turn_on)
{
}

void spi_hal_set_baud(const void *p_spi_device, const uint32_t baud_rate)
{
    // spi_bus_remove_device(p_spi_device->spi_handle);

    // spi_hal_add_device(p_spi_device, baud_rate);
}
#if USE_SPI_INTERRUPTS == 1
/**
 * @brief  Handle an I2C interrupt
 * @retval None
 */
void spi_hal_irqhandler(void);
#endif // USE_SPI_INTERRUPTS
#endif // HW_CONFIG_SPI
