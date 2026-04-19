
/*****************************(C) COPYRIGHT 2024 Old Man Software
 * *****************
 * @file       spi_hal.h
 * @author     Jack Wilson
 * @brief      hal layer for using SPI
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Jan-19-2024     Jack Wilson jackkc7vlo@gmail.com
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
 * ****************************(C) COPYRIGHT 2024 Old Man Software
 * *****************/

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef SPI_HAL_H
#define SPI_HAL_H

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <hw_config.h>
#include <stdbool.h> /*lint -e129*/
#include <stdint.h>

#if HW_CONFIG_SPI == 1u
/********************************************************************************
 * Defines
 ********************************************************************************/
#define SPI_HAL_MAX_BUS 4u

#define SPI_BUS_1 1u
#define SPI_BUS_2 2u
#define SPI_BUS_3 3u
#define SPI_BUS_4 4u

#define SPI_HAL_BAUD_1MHz 1000000u
#define SPI_HAL_BAUD_2MHz 2000000u
#define SPI_HAL_BAUD_30MHz 30000000u

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    // static uint8_t dummy;
    typedef void (*p_spi_callback_t)(void);

    /* Opaque spi_hal type: definition is private and resides in the .c
     * implementation file. Consumers should only use the pointer type. */
    typedef struct spi_hal  spi_hal_t;
    typedef struct spi_hal *p_spi_hal_t;

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    /********************************************************************************
     * @brief          Initialization function for the SPI driver
     * @retval         None
     ********************************************************************************/
    void spi_hal_initialize(void);

    /********************************************************************************
     * @brief          Check if the SPI driver is initialized
     * @retval         true if initialized, false otherwise
     ********************************************************************************/
    bool spi_hal_is_bus_initialized(void);

    /********************************************************************************
     * @brief          Create an SPI device
     * @param[in]      spi_device_id - Which SPI device to use
     * @param[in]      p_callback - Pointer to the callback function
     * @retval         Pointer to the SPI device
     ********************************************************************************/
    p_spi_hal_t spi_hal_create_device(const uint32_t spi_device_id, p_spi_callback_t p_callback);

    /********************************************************************************
     * @brief          Remove an SPI device
     * @param[in]      p_handle - pointer to the SPI device
     * @retval         None
     ********************************************************************************/
    void spi_hal_remove_device(p_spi_hal_t p_handle);

    /********************************************************************************
     * @brief          Write function for the SPI driver
     * @param[in]      p_spi_device - pointer to the SPI device
     * @param[in]      data -  pointer to data to send (may be words depending on
     *MCU)
     * @param[in]      data_len - Length of data to write in bytes
     * @retval         Number of bytes written
     ********************************************************************************/
    void spi_hal_write(const p_spi_hal_t p_spi_device, const uint8_t *const data, const uint16_t data_len);

    /********************************************************************************
     * @brief          Read function for the SPI driver
     * @param[in]      spi_device_id - SPI device ID
     * @param[in]      data -  pointer to data (in bytes!) to read
     * @param[in]      data_len - Length of data to read in bytes
     * @retval         Number of bytes read
     ********************************************************************************/
    uint16_t spi_hal_read(const p_spi_hal_t p_spi_device, uint8_t *data, const uint16_t data_len);

    /********************************************************************************
     * @brief  Enable or disable SPI
     * @param[in]      turn_on - enable or disable the SPI devices
     * @retval         None
     ********************************************************************************/
    void spi_hal_enable(const p_spi_hal_t p_spi_device, const bool turn_on);

    /********************************************************************************
     * @brief  Set or clear the SPI baud rate
     * @param[in]      baud_rate - baud rate to set
     * @retval         None
     ********************************************************************************/
    void spi_hal_set_baud(const p_spi_hal_t p_spi_device, const uint32_t baud_rate);

#if USE_SPI_INTERRUPTS == 1
    /**
     * @brief  Handle an I2C interrupt
     * @retval None
     */
    void spi_hal_irqhandler(void);
#endif

#endif // HW_CONFIG_SPI
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* SPI_HAL_H */