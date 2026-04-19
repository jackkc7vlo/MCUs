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
 * @file      hw_config.h
 * @defgroup  features Controls the features available for a project
 * @author    Jack Wilson
 * @brief     #defines to determine what features are included in a project
 *
 * @details   Depending on the project various feature may or may not be
 *            available. Since the code is purposefully generic and may be
 *            part of a library, we want to be able to easily configure what
 *            features are a part of it.  The will be done with defines in
 *            the form of #define HAS_FEATURE_xxxx such as:
 *            #define HAS_FEATURE_DEBUG_UART 1 and then code that uses the
 *            feature would need to wrap call and include files with
 *            #if HAS_FEATURE_DEBUG_UART==1
 *               #include "debug_uart.h"
 *            #endif
 *
 *
 * @mod
 * @moditem{V1.0.x, Mar-08-2025, Jack Wilson, Initial Release, jackkc7vlo@gmail.com }
 * @modend
 *
 */
#include <stdint.h>

#ifndef HW_CONFIG_H_
#define HW_CONFIG_H_

#define HW_CONFIG_GPIO 1
// number of GPIO interrupts supported
#define GPIOS_INTERRUPTS 16
#define NUMBER_GPIOS_PORTS 6

#define USE_PORTA_GPIO 0
#define USE_PORTB_GPIO 0
#define USE_PORTC_GPIO 0
#define USE_PORTD_GPIO 0
#define USE_PORTE_GPIO 0
#define USE_PORTF_GPIO 0
#define USE_PORTG_GPIO 0
#define USE_PORTH_GPIO 0

#define USE_LED_GPIO 1U
#define NUMBER_OF_LEDS 1U
#define LED_PIN 13U
#define LED_MODE FLOAT
#define LED_PORT GPIOB_PORT

#define USE_BUTTON_GPIO 1
#define NUMBER_OF_BUTTONS 1U
#define BUTTON_ACTIVE_HIGH 1
#define BUTTON_PIN 13U
#define BUTTON_PORT GPIOC_PORT
#define BUTTON_MODE PUSHPULL
#define BUTTON_EDGE IRQ_NEGATIVE
#define BUTTON_DEBOUNCE_MS 10U
#define BUTTON_IRQ_PRIORITY 3U

#define HW_CONFIG_M4_FPU 0
#define HW_CONFIG_DEBUG_UART 0
#define HW_CONFIG_TIMEBASE 0
#define HW_CONFIG_BSP 0
#define HW_CONFIG_ADC 0
#define HW_CONFIG_SPI 1
#define HW_CONFIG_I2C 1

#define NUM_I2C_DEVICES 3U        /*!< I2C1, I2C2, I2C3 on STM32L412xE */
#define USE_I2C_INTERRUPTS 0      /*!< 1 = use interrupt-driven I2C     */
#define I2C_DEFAULT_SPEED 100000U /*!< Default I2C clock speed (Hz)     */
#define I2C1_SDA_PORT GPIOB_PORT
#define I2C1_SDA_PIN 7U
#define I2C1_SCL_PORT GPIOB_PORT
#define I2C1_SCL_PIN 8U

#define HAS_ST7735 0u

#if HAS_ST7735 == 1u
#define ST7735_SPI_CS_PORT 1u
#define ST7735_SPI_CS_PIN 3u
#define ST7735_SPI_AO_PORT 1u
#define ST7735_SPI_AO_PIN 4u
#define ST7735_SPI_RESET_PORT 1u
#define ST7735_SPI_RESET_PIN 5u
#endif

#define HAS_ILI9341 1u
#if HAS_ILI9341 == 1u
// D3 PB3
#define ILI9341_SPI_CS_PORT GPIOB_PORT
#define ILI9341_SPI_CS_PIN 3u

// D5 PA15
#define ILI9341_SPI_DC_PORT GPIOA_PORT
#define ILI9341_SPI_DC_PIN 15u

// D4 PB5
#define ILI9341_SPI_RESET_PORT GPIOB_PORT
#define ILI9341_SPI_RESET_PIN 5u
// D6 PB10
#define ILI9341_SPI_BCKL_PORT GPIOB_PORT
#define ILI9341_SPI_BCKL_PIN 10u
#endif // HAS_ILI9341

#define NUM_TIMERS 2U

#endif /* HW_CONFIG_H_ */
