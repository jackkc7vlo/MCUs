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
 * @file      device_gpio.h
 * @defgroup  device_gpio General Purpose Input/Output (GPIO)
 * @author    Jack Wilson
 * @brief     Defines and Macros for the General Purpose Input/Output (GPIO)
 *
 * @details   The NHS31xx supports various GPIO features. Each GPIO pin can be configured
 * as an input or output, and can generate interrupts on specific edge conditions.
 *
 * @mod
 * @moditem{V1.0.x, Nov-12-2025, Jack Wilson, Initial Release, jackkc7vlo@gmail.com }
 * @modend
 *
 */

#include <stdint.h>
#ifndef DEVICE_GPIO_H
#define DEVICE_GPIO_H
// lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
// lint -restore
// lint -esym(751,gpio_*) There are definitions they may not be used
// lint -esym(754,gpio_*) There are definitions they may not be used

#define SET_GPIO_BIT(variable, bit_position) ((variable) |= (1U << (bit_position)))
#define CLEAR_GPIO_BIT(variable, bit_position) ((variable) &= ~(1U << (bit_position)))
#define TOGGLE_GPIO_BIT(variable, bit_position) ((variable) ^= (1U << (bit_position)))
#define READ_GPIO_BIT(variable, bit_position) (((variable) >> (bit_position)) & 1U)

// GPIO Base Address
#define REG_GPIO_BASE (0x50000000U) //!< GPIO Base Address

/**
 * @brief Pin Bits
 *
 */
#define GPIO_PIN0_BIT (1U)         //!< Bit 0 PIO0_0 bit
#define GPIO_PIN1_BIT (1U << 1U)   //!< Bit 1 PIO0_1 bit
#define GPIO_PIN2_BIT (1U << 2U)   //!< Bit 2 PIO0_2 bit
#define GPIO_PIN3_BIT (1U << 3U)   //!< Bit 3 PIO0_3 bit
#define GPIO_PIN4_BIT (1U << 4U)   //!< Bit 4 PIO0_4 bit
#define GPIO_PIN5_BIT (1U << 5U)   //!< Bit 5 PIO0_5 bit
#define GPIO_PIN6_BIT (1U << 6U)   //!< Bit 6 PIO0_6 bit
#define GPIO_PIN7_BIT (1U << 7U)   //!< Bit 7 PIO0_7 bit
#define GPIO_PIN8_BIT (1U << 8U)   //!< Bit 8 PIO0_8 bit
#define GPIO_PIN9_BIT (1U << 9U)   //!< Bit 9 PIO0_9 bit
#define GPIO_PIN10_BIT (1U << 10U) //!< Bit 10 PIO0_10 bit
#define GPIO_PIN11_BIT (1U << 11U) //!< Bit 11 PIO0_11 bit

/**
 * @brief GPIO registers structure definition
 */
typedef struct
{
    volatile uint32_t
        data[4096]; //!< GPIO Data address masking register (R/W)       offset 0x0000 - 0x3FFC
    volatile uint32_t
        reserved0[4096];    //!< Reserved                                    offset 0x4000 - 0x7FFF
    volatile uint32_t dir;  //!< GPIO Direction Register (GPIODIR)              offset 0x8000
    volatile uint32_t is;   //!< GPIO Interrupt Sense Register (GPIOIS)         offset 0x8004
    volatile uint32_t ibe;  //!< GPIO Interrupt Both Edges Register(GPIOIBE)    offset 0x8008
    volatile uint32_t iev;  //!< GPIO Interrupt Event Register (GPIOIEV)        offset 0x800C
    volatile uint32_t imse; //!< GPIO Interrupt Mask Register (GPIOIM)          offset 0x8010
    volatile uint32_t ris;  //!< GPIO Raw Interrupt Status Register (GPIORIS)   offset 0x8014
    volatile uint32_t mis;  //!< GPIO Masked Interrupt Status Register (GPIOMIS)offset 0x8018
    volatile uint32_t ic;   //!< GPIO Interrupt Clear Register (GPIOIC)         offset 0x801C
} reg_gpio_t;
/** @brief GPIO registers
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_gpio_t *p_device_gpio_a = ((reg_gpio_t *const)REG_GPIO_BASE); //!< The GPIO registers
#pragma GCC diagnostic pop

/**
 * @brief GPIO data direction register (DIR)
 *
 */

#define GPIO_DIR_INPUT (0U)  //!< Configure pin as input
#define GPIO_DIR_OUTPUT (1U) //!< Configure pin as output

#endif // DEVICE_GPIO_H
