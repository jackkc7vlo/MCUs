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
 * Multiple include protection
 ********************************************************************************/
#ifndef spi_hal_h_
#define spi_hal_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdint.h>
#include <hw_config.h>
    /********************************************************************************
     * Defines
     ********************************************************************************/
#define SPI_HAL_MAX_DEVICES 4u
#define SPI_DEVICE_1 1u
#define SPI_DEVICE_2 2u
#define SPI_DEVICE_3 3u
#define SPI_DEVICE_4 4u

#define SPI_HAL_BAUD_1MHz 1000000u
#define SPI_HAL_BAUD_2MHz 2000000u
#define SPI_HAL_BAUD_30MHz 30000000u

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    typedef struct spi_hal_def_s *p_spi_hal_def_t;
    typedef void (*p_spi_callback_t)(void);

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    /********************************************************************************
     * @brief          Initialization function for the SPI driver
     * @retval         None
     ********************************************************************************/
    void spi_hal_initialize(const p_spi_hal_def_t p_spi_device);

    /********************************************************************************
     * @brief          Check if the SPI driver is initialized
     * @retval         true if initialized, false otherwise
     ********************************************************************************/
    bool spi_hal_is_initialized(const p_spi_hal_def_t p_spi_device);

    /********************************************************************************
     * @brief          Create an SPI device
     * @param[in]      spi_device_id - Which SPI device to use
     * @param[in]      p_callback - Pointer to the callback function
     * @retval         Pointer to the I2C device
     ********************************************************************************/
    p_spi_hal_def_t spi_hal_create_device(const uint32_t spi_device_id,
                                          const p_spi_callback_t p_callback);

    /********************************************************************************
     * @brief          Free an SPI device
     * @param[in]      p_spi_device - pointer to the i2c device
     * @retval         None
     * @note           This function will free the memory allocated for the device
     *                and remove it from the list of devices
     ********************************************************************************/
    void spi_hal_free(const p_spi_hal_def_t p_spi_device);

    /********************************************************************************
     * @brief          Write function for the SPI driver
     * @param[in]      p_spi_device - pointer to the SPI device
     * @param[in]      data -  pointer to data to send (may be words depending on MCU)
     * @param[in]      data_len - Length of data to write in bytes
     * @retval         Number of bytes written
     ********************************************************************************/
    uint_fast16_t spi_hal_write(const p_spi_hal_def_t p_spi_device,
                                const uint8_t *const data, const uint_fast16_t data_len);

    /********************************************************************************
     * @brief          Read function for the SPI driver
     * @param[in]      spi_device_id - SPI device ID
     * @param[in]      data -  pointer to data (in bytes!) to read
     * @param[in]      data_len - Length of data to read in bytes
     * @retval         Number of bytes read
     ********************************************************************************/
    uint_fast16_t spi_hal_read(const p_spi_hal_def_t p_spi_device,
                               uint8_t *data, const uint_fast16_t data_len);

    /********************************************************************************
     * @brief  Enable or disable SPI
     * @param[in]      turn_on - enable or disable the SPI devices
     * @retval         None
     ********************************************************************************/
    void spi_hal_enable(const p_spi_hal_def_t p_spi_device, const bool turn_on);

    /********************************************************************************
     * @brief  Set or clear the SPI chip select
     * @param[in]      turn_on - enable or disable the SPI devices
     * @retval         None
     ********************************************************************************/
    void spi_hal_chip_select(const p_spi_hal_def_t p_spi_device, const bool turn_on);

    /********************************************************************************
     * @brief  Set or clear the SPI baud rate
     * @param[in]      baud_rate - baud rate to set
     * @retval         None
     ********************************************************************************/
    void spi_hal_set_baud(const p_spi_hal_def_t p_spi_device, const uint32_t baud_rate);

#if USE_SPI_INTERRUPTS == 1
    /**
     * @brief  Handle an I2C interrupt
     * @retval None
     */
    void spi_hal_irqhandler(void);
#endif

#if UNIT_TESTS == 1u
    void spi_hal_unit_test(void);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* spi_hal.h_ */
