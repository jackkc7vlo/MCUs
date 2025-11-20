/**
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************
 * @file       spi_hal.h
 * @author     Jack Wilson
 * @brief      hal layer for using SPI
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Jan-19-2024     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 *
 * ==============================================================================
 * @endverbatim
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************/

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdint.h>
#include <stdlib.h> /*lint -e129*/
#include <hw_config.h>
#include <stdbool.h>

#if USE_SPI == 1u
#include <spi_hal.h>
#include <assert.h>
#include <clock_hal.h>
#include <gpio_hal.h>
#include <string.h>
#include <stdio.h>
#include "printf_hal.h"
#include <ssp_nss.h>
#if UNIT_TESTS == 1u
#include <unittest.h>

#endif
    /********************************************************************************
     * Defines
     ********************************************************************************/
#define SPI_DEVICE_1 1u
#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK 12
#define DMA_CHAN 1
#define PIN_NUM_CS 15

// To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
// but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
#define PARALLEL_LINES 16

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    typedef struct spi_hal_def_s
    {
        uint32_t device_id;          /*!< bus id */
        p_spi_callback_t p_callback; /*!< I2C IRQ callback */
        void *p_spi;                 /*!< Pointer to the SPI peripheral */
        bool spi_initialized;        /*!< true if initialized, false otherwise */

    } spi_hal_def_t;

    static spi_hal_def_t *p_spis[MAX_SPI] = {NULL}; /*lint !e956*/
    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    static uint32_t spi_hal_get_free_id(void)
    {
        for (uint32_t i = 0u; i < MAX_SPI; i++)
        {
            if (p_spis[i] == NULL)
            {
                return i;
            }
        }
        return MAX_SPI;
    }
    /********************************************************************************
     * @brief          Initialization function for the SPI driver
     * @retval         None
     ********************************************************************************/
    void spi_hal_add_device(const p_spi_hal_def_t p_spi_device, const uint32_t baud_rate)
    {
    }

    void spi_hal_initialize(const p_spi_hal_def_t p_spi_device)
    {

        assert(p_spi_device != NULL);
        // assert(p_spi_device->p_spi != NULL);
        // assert(p_spi_device->p_cs_gpio != NULL);
        //        assert(p_spi_device->p_callback != NULL);

        if (p_spi_device->spi_initialized == true)
        {
            return; // already done
        }
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_2, IOCON_FUNC_0 | IOCON_RMODE_INACT); // CS
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_6, IOCON_FUNC_1);                     // SCLK
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_8, IOCON_FUNC_1);                     // MISO
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_9, IOCON_FUNC_1);                     // MOSI

        // Chip_GPIO_SetPinDIRInput(NSS_GPIO, Pinport, MISO);
        // Chip_GPIO_SetPinDIROutput(NSS_GPIO, Pinport, MOSI);
        Chip_SSP_Init(NSS_SSP0);
        /* Configure transfer parameters */
        Chip_SSP_SetMaster(NSS_SSP0, 1);
        Chip_SSP_SetFormat(NSS_SSP0, (int32_t)SSP_BITS_8, SSP_FRAME_FORMAT_SPI, SSP_CLOCK_MODE0);
        Chip_SSP_SetBitRate(NSS_SSP0, 10000u);
        Chip_SSP_Enable(NSS_SSP0);
        // spi_hal_add_device(p_spi_device, 80000000u);

        p_spi_device->spi_initialized = true;
    }

    spi_hal_def_t *spi_hal_get_by_device_id(uint32_t device_id)
    {
        (void)printf_hal("%s SPI ID %d\n", __func__, device_id);
        if (device_id >= MAX_SPI)
        {
            (void)printf_hal("%s SPI ID too large. ID %d\n", __func__, device_id);
            return NULL;
        }

        for (uint32_t i = 0; i < MAX_SPI; i++)
        {

            if (p_spis[i] != NULL && device_id == p_spis[i]->device_id)
            {
                return p_spis[i];
            }
        }
        return NULL;
    }

    /********************************************************************************
     * @brief          Check if the SPI driver is initialized
     * @retval         true if initialized, false otherwise
     ********************************************************************************/
    bool spi_hal_is_initialized(const p_spi_hal_def_t p_spi_device)
    {
        return p_spi_device->spi_initialized;
    }

    /********************************************************************************
     * @brief          Create an SPI device
     * @param[in]      spi_device_id - Which SPI device to use
     * @param[in]      p_callback - Pointer to the callback function
     * @retval         Pointer to the I2C device
     ********************************************************************************/
    p_spi_hal_def_t spi_hal_create_device(const uint32_t spi_device_id,
                                          const p_spi_callback_t p_callback)
    {

        (void)printf_hal("%s\n", __func__);
        p_spi_hal_def_t p_spi_device = spi_hal_get_by_device_id(spi_device_id);
        if (spi_hal_get_by_device_id(spi_device_id) != NULL)
        {
            (void)printf_hal("%s SPI ID already in use. ID %d\n", __func__, spi_device_id);
            return p_spi_device;
        }
        p_spi_device = (p_spi_hal_def_t)malloc(sizeof(spi_hal_def_t));
        assert(p_spi_device != NULL);
        p_spi_device->device_id = spi_device_id;
        p_spi_device->p_callback = p_callback;
        // p_spi_device->p_cs_gpio = p_cs_gpio;
        p_spi_device->spi_initialized = false;
        if (spi_device_id < MAX_SPI)
        {
            p_spis[spi_device_id] = p_spi_device;
        }

        return p_spi_device;
    }

    /********************************************************************************
     * @brief          Free an SPI device
     * @param[in]      p_spi_device - pointer to the i2c device
     * @retval         None
     * @note           This function will free the memory allocated for the device
     *                and remove it from the list of devices
     ********************************************************************************/
    void spi_hal_free(const p_spi_hal_def_t p_spi_device)
    {
        (void)printf_hal("%s\n", __func__);
        assert(p_spi_device != NULL);
        free((void *)p_spi_device);
    }

    /********************************************************************************
     * @brief          Write byte function for the SPI driver
     ********************************************************************************/

    /********************************************************************************
     * @brief          Write function for the SPI driver
     * @param[in]      p_spi_device - pointer to the SPI device
     * @param[in]      data -  pointer to data to send (may be words depending on MCU)
     * @param[in]      data_len - Length of data to write in bytes
     * @retval         Number of bytes written
     ********************************************************************************/
    uint_fast16_t spi_hal_write(const p_spi_hal_def_t p_spi_device,
                                const uint8_t *const data, const uint_fast16_t data_len)
    {
        if (data_len == 0u)
        {
            return 0u; // nothing to do
        }
        /* Initialise rx/tx setup data structure */

        if (Chip_SSP_WriteFrames_Blocking(NSS_SSP0, (uint8_t *)data, data_len) == data_len)
        {
            /* Transfer successful */
            return data_len;
        }
        else
        {
            /* Something went wrong */
            return 0u;
        }
    }

    /********************************************************************************
     * @brief          Read function for the SPI driver
     * @param[in]      spi_device_id - SPI device ID
     * @param[in]      data -  pointer to data (in bytes!) to read
     * @param[in]      data_len - Length of data to read in bytes
     * @retval         Number of bytes read
     ********************************************************************************/
    uint_fast16_t spi_hal_read(const p_spi_hal_def_t p_spi_device,
                               uint8_t *data, const uint_fast16_t data_len)
    {
        if (Chip_SSP_ReadFrames_Blocking(NSS_SSP0, data, data_len) == data_len)
        {
            /* Transfer successful */
            return data_len;
        }
        else
        {
            /* Something went wrong */
            return 0u;
        }
    }

    /********************************************************************************
     * @brief  Enable or disable SPI
     * @param[in]      turn_on - enable or disable the SPI devices
     * @retval         None
     ********************************************************************************/
    void spi_hal_enable(const p_spi_hal_def_t p_spi_device, const bool turn_on)
    {
        (void)turn_on;
        (void)p_spi_device;
    }

    void spi_hal_set_baud(const p_spi_hal_def_t p_spi_device, const uint32_t baud_rate)
    {
        (void)baud_rate;
        (void)p_spi_device;
    }
#if USE_SPI_INTERRUPTS == 1u
    /**
     * @brief  Handle an I2C interrupt
     * @retval None
     */
    void spi_hal_irqhandler(void);
#endif

#if UNIT_TESTS == 1u
    void spi_hal_unit_test(void)
    {
    }
#endif
#endif /* USE_SPI */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
