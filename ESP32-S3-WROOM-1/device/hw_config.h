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

// begin GPIO configuration
#define HW_CONFIG_GPIO 1
// number of GPIO interrupts supported
#define GPIOS_INTERRUPTS 1
#define NUMBER_GPIOS_PORTS 1

#define USE_BUTTON_GPIO 1
#define NUMBER_OF_BUTTONS 1U
#define BUTTON_ACTIVE_HIGH 0u
#define BUTTON_PIN 5U
#define BUTTON_PORT 0
#define BUTTON_MODE PULLDOWN

#define LED_PIN 18U
#define LED_PORT 0
// end GPIO configuration

#define HW_CONFIG_M4_FPU 0
#define HW_CONFIG_DEBUG_UART 0
#define HW_CONFIG_TIMEBASE 0
#define HW_CONFIG_BSP 0
#define HW_CONFIG_ADC 0

// begin SPI configuration
#define HW_CONFIG_SPI 0
#define AUTO_CS 1
#define HW_CONFIG_SPI_NUM_BUS 1
#define HW_CONFIG_SPI1_DEVICE_ID 1u
#define HW_CONFIG_SPI1_MOSI_PORT 17
#define HW_CONFIG_SPI1_MISO_PORT -1
#define HW_CONFIG_SPI1_SCLK_PORT 16
#define HW_CONFIG_SPI1_CS_PORT 15
#define SPI_USE_BUS_1 1u
#define SPI_USE_BUS_2 0u
#define SPI_USE_BUS_3 0u
#define SPI_USE_BUS_4 0u
#if SPI_USE_BUS_1 + SPI_USE_BUS_2 + SPI_USE_BUS_3 + SPI_USE_BUS_4 > HW_CONFIG_SPI_NUM_BUS
#pragma message("Too Many SPI Buses Defined!")
#endif
#define HW_CONFIG_SPI_NUM_DEVICES 1u

#define SPI_USE_DEVICE_1 1u
#define SPI1_BUS_ID 1U
#define SPI1_BAUD_RATE 10000000u
#define SPI1_QUEUE_SIZE 7u
#define SPI1_MODE 0

#define SPI_USE_DEVICE_2 0u
#define SPI_USE_DEVICE_3 0u
#define SPI_USE_DEVICE_4 0u
#if SPI_USE_DEVICE_1 + SPI_USE_DEVICE_2 + SPI_USE_DEVICE_3 + SPI_USE_DEVICE_4 > HW_CONFIG_SPI_NUM_DEVICES
#pragma message("Too Many SPI Devices Defined!")
#endif
#define HAS_ILI9341 0U
#define ILI9341_SPI_CS_PORT 0u
#define ILI9341_SPI_CS_PIN 15u
#define ILI9341_SPI_DC_PORT 0u
#define ILI9341_SPI_DC_PIN 2u
#define ILI9341_SPI_RESET_PORT 0u
#define ILI9341_SPI_RESET_PIN 16u
#define ILI9341_SPI_BCKL_PORT 0u
#define ILI9341_SPI_BCKL_PIN 0x5u

#define SPI_HOST SPI3_HOST
#define SPI_DEVICE_1 1u
#define SPI_DEVICE_2 2u
#define SPI_DEVICE_3 3u
#define SPI_DEVICE_4 4u

#define HAS_ST7735 0U
#define IS_AIPI 0U
#define ST7735_SPI_CS_PORT 0u
#define ST7735_SPI_CS_PIN 15u
#define ST7735_SPI_DC_PORT 0u
#define ST7735_SPI_DC_PIN 7u
#define ST7735_SPI_RESET_PORT 0u
#define ST7735_SPI_RESET_PIN 18u
#define ST7735_SPI_BCKL_PORT 0u
#define ST7735_SPI_BCKL_PIN 3u
#if IS_AIPI == 1u
#define ST7735_WIDTH 128
#define ST7735_HEIGHT 128
#define HAS_LED_STRIP 1u
#define STRIP_GPIO_NUM 46
#else
#define HAS_LED_STRIP 0u
#define STRIP_GPIO_NUM 48
#define ST7735_WIDTH 160
#define ST7735_HEIGHT 128
#endif
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION ((1 << 5) | (1 << 6) | (1 << 3))
#define ST7735_COLOR_MODE 0x55 //  (normally 0x05)16-bit color

// end SPI configuration

// begin I2C configuration
#define HW_CONFIG_I2C 0U
#define I2C_BUS I2C_NUM_0
#define HW_CONFIG_I2C_SDA_PORT 5
#define HW_CONFIG_I2C_SCL_PORT 4

#define HW_CONFIG_I2C_NUM_DEVICES 2
#define HW_CONFIG_I2C1_ADDRESS 0x18 // es8311 address

#define HW_CONFIG_ADDRESS_LENGTH_1 7

#define HW_CONFIG_I2C2_ADDRESS 0x29
#define HW_CONFIG_ADDRESS_LENGTH_2 7

// If you need more I2C devices, add them here and in i2c_hal.c
// end I2C configuration

// i2c devices
#define HAS_PCF8574A 0u

#define HAS_ES8311 0u
#define ES8311_I2C_ADDRESS 0x18
#define ES8311_PA_ENABLE_GPIO 9

// i2s devices
#define HAS_ES8311_I2S 0u

#endif /* HW_CONFIG_H_ */
