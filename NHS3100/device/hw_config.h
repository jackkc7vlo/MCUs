/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       hw_config.h
 * @author     Jack Wilson
 * @brief      hardware config for the EFM32PG22 MCU
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-28-2023     Jack Wilson                     jackkc7vlo@gmail.com
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
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************/

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef hw_config_h_
#define hw_config_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdint.h>
#include <reg_hal.h>
#include <iocon_nss.h>

/*********************************************************************************/
/* Memory Definitions                                                            */
/*********************************************************************************/
#define GENERAL_MEM_SIZE 0x400u // GPIO, SPI, I2C, UART, etc (Generally static)

    typedef enum memory_owner
    {
        SYSTEM_MEMORY, ///< System memory
        GPIO,
        BIZ_MEMORY,               ///< Business logic memory
        SENSOR_DATA_MEMORY,       ///< Sensor data memory
        TEMPS_LOG_MEMORY,         ///< Temperature log memory
        POINTS_LOG_MEMORY,        ///< Points log memory
        RETAIN_DATA_MEMORY,       ///< Retain data memory
        SENSOR_EXTRA_DATA_MEMORY, ///< Sensor Extra data memory
        INIT_DATA_MEMORY          ///< Init data memory
    } memory_owner_t;

#define BUTTON_PORT 0u
#define BUTTON_PIN 1u
#define WAIT_FOR_BUTTON 1u

#define LED_PORT 0u
#define LED_PIN (0x020u >> 2u) /*!< Represents the digital pin 8 */

#define I2C_ENABLE_PORT 0u
#define I2C_ENABLE_PIN 6u

#define USE_SPI 0u
#define USE_I2C 0u
#define USE_DEBUG_MENU 0u
#define USE_UART 0u
#define USE_EM4325 0u
#define USE_LM75 0u
#define USE_RV3028 0u
#define USE_BUTTON 0u
#define USE_RED_LED 1u
#define USE_GREEN_LED 1u
#define USE_MONZA 0u
#define USE_FLASH 0u
#define USE_EEPROM 0u
#define USE_PRINTF 0u
#define USE_SEGGER 0u
#define USE_RFID 0u
#define USE_NFC 0u
#define USE_BLE 0u
#define USE_TIMERS 0u
#define UNIT_TESTS 0u // needs print
#define USE_HW_CRC 0u
// printf option
#define PRINT_EM4325 0u
// dangerous to use this, it might cause a hard fault
#define PRINT_IRQ 0u
#define PRINT_PACKET 0u
#define PRINT_ALLOC 0u
#define PRINT_COMMAND 0u

#define MAX_GPIO 16u
#define MAX_SPI 2u
#define TIMER_HAL_MAX_TIMERS 1u /*!< The maximum number of timers available */

#define TEST_HW_TIMER 0u
#if USE_I2C == 1u

#define USE_I2C_INTERRUPTS 1u
#define I2C_SCL_PORT 0u
#define I2C_SCL_PIN 8u
#endif
    /***********************/
    /* Drivers Supported   */
    /***********************/
#define HAS_LED 1u
#define HAS_BUTTON 1u

#if USE_I2C == 1u

#define HAS_SI7021 0u
#define HAS_LM75 1u
#define LM75_I2C_ADDRESS 0x48u

#define HAS_ADXL345 0u
#define ADXL345_I2C_ADDRESS (0x53u)

#define HAS_TSL2561 1u
#define TSL2561_I2C_ADDRESS (0x29)

#define HAS_PCF8574A 1u
#define PCF8574A_I2C_ADDRESS (0x38)

#endif // USE_I2C == 1u

#if USE_SPI == 1u
#define HAS_ST7735 0u

#if HAS_ST7735 == 1u
#define ST7735_SPI_CS_PORT 0u
#define ST7735_SPI_CS_PIN 15u
#define ST7735_SPI_AO_PORT 0u
#define ST7735_SPI_AO_PIN 2u
#define ST7735_SPI_RESET_PORT 0
#define ST7735_SPI_RESET_PIN 0xffu
#endif // HAS_ST7735

#define HAS_ILI9341 0u
#if HAS_ILI9341 == 1u
#define ILI9341_SPI_CS_PORT 0u
#define ILI9341_SPI_CS_PIN 15u
#define ILI9341_SPI_DC_PORT 0u
#define ILI9341_SPI_DC_PIN 2u
#define ILI9341_SPI_RESET_PORT 0u
#define ILI9341_SPI_RESET_PIN 16u
#define ILI9341_SPI_BCKL_PORT 0u
#define ILI9341_SPI_BCKL_PIN 0xffu
#endif // HAS_ILI9341

#define HAS_ADS7843 0u
#if HAS_ADS7843 == 1u
#define ADS7843_SPI_CS_PORT 0u
#define ADS7843_SPI_CS_PIN 1u
#endif // HAS_ADS7843

#define HAS_EM4325 1u
#if HAS_EM4325 == 1u
#define EM4325_SPI_CS_PORT 0u
#define EM4325_SPI_CS_PIN IOCON_PIO0_2
#define EM4325_SPI_AUX_PORT 0u
#define EM4325_SPI_AUX_PIN IOCON_PIO0_0
#define EM4325_SPI_POWER_PORT 0u
#define EM4325_SPI_POWER_PIN IOCON_PIO0_7
#endif // HAS_EM4325

#endif // USE_SPI == 1u

#define USE_SPI_INTERRUPTS 0u

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* hw_config_h_ */