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
 * @file device_gpio.h
 * @brief Generic GPIO device definition.
 * @details Outlines the portable GPIO abstraction used to configure and interact with digital pins
 * on any supported platform.
 *
 * @author  Jack Wilson
 * @date    September 26, 2025
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Jack Wilson
 * @license MIT License
 */

#ifndef DEVICE_GPIO_H
#define DEVICE_GPIO_H

#include "device_regs.h"
#include <stdint.h>

// lint -esym(528,*) There are definitions they may not be used
// lint -esym(750,*) There are definitions they may not be used
// lint -esym(751,*) There are definitions they may not be used
// lint -esym(754,*) There are definitions they may not be used
// lint -esym(749,*) There are definitions they may not be used

// NOTE:  This does not define every possible register or bit
// More can be added from the datasheet

#define REG_GPIOA_EN (1U)
#define REG_GPIOB_EN (1U << 1U)
#define REG_GPIOC_EN (1U << 2U)
#define REG_GPIOD_EN (1U << 3U)
#define REG_GPIOE_EN (1U << 4U)
#define REG_GPIOF_EN (1U << 5U)
#define REG_GPIOG_EN (1U << 6U)
#define REG_GPIOH_EN (1U << 7U)
#define REG_GPIOI_EN (1U << 8U)

// GPIO
#define REG_GPIOA_BASE (REG_AHB2PERIPH_BASE)
#define REG_GPIOB_BASE (REG_AHB2PERIPH_BASE + 0x0400U)
#define REG_GPIOC_BASE (REG_AHB2PERIPH_BASE + 0x0800U)
#define REG_GPIOD_BASE (REG_AHB2PERIPH_BASE + 0x0C00U)
#define REG_GPIOE_BASE (REG_AHB2PERIPH_BASE + 0x1000U)
#define REG_GPIOF_BASE (REG_AHB2PERIPH_BASE + 0x1400U)
#define REG_GPIOG_BASE (REG_AHB2PERIPH_BASE + 0x1800U)
#define REG_GPIOH_BASE (REG_AHB2PERIPH_BASE + 0x1C00U)
#define REG_GPIOI_BASE (REG_AHB2PERIPH_BASE + 0x2000U)

typedef struct
{
    volatile uint32_t moder;   // offset 0x00
    volatile uint32_t otype;   // offset 0x04
    volatile uint32_t ospeedr; // offset 0x08
    volatile uint32_t pupdr;   // offset 0x0c
    volatile uint32_t idr;     // offset 0x10
    volatile uint32_t odr;     // offset 0x14
    volatile uint32_t bsrr;    // offset 0x18
    volatile uint32_t lckr;    // offset 0x1c
    volatile uint32_t afr[2];  // offset 0x20
} reg_gpio_t;

// lint -e844
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_gpio_t *p_device_gpio_a = ((reg_gpio_t *const)REG_GPIOA_BASE); //!< The GPIO registers
static reg_gpio_t *p_device_gpio_b = ((reg_gpio_t *const)REG_GPIOB_BASE); // lint !e844
static reg_gpio_t *p_device_gpio_c = ((reg_gpio_t *const)REG_GPIOC_BASE); // lint !e844
static reg_gpio_t *p_device_gpio_d = ((reg_gpio_t *const)REG_GPIOD_BASE); // lint !e844
static reg_gpio_t *p_device_gpio_e = ((reg_gpio_t *const)REG_GPIOE_BASE); // lint !e844
static reg_gpio_t *p_device_gpio_f = ((reg_gpio_t *const)REG_GPIOF_BASE); // lint !e844
static reg_gpio_t *p_device_gpio_g = ((reg_gpio_t *const)REG_GPIOG_BASE); // lint !e844
static reg_gpio_t *p_device_gpio_h = ((reg_gpio_t *const)REG_GPIOH_BASE); // lint !e844
static reg_gpio_t *p_device_gpio_i = ((reg_gpio_t *const)REG_GPIOI_BASE); // lint !e844

#pragma GCC diagnostic pop

typedef enum
{
    eGPIO_MODE_INPUT     = 00U,
    eGPIO_MODE_OUTPUT    = 01U,
    eGPIO_MODE_ALTERNATE = 02U,
    eGPIO_MODE_ANALOG    = 03U
} gpio_modes_t;

typedef uint32_t pin_t; // lint -esym(751,pin_t) suppress unused

typedef volatile struct
{
    reg_gpio_t *port;
    pin_t       pin;
} gpio_address_t;

#endif /* DEVICE_GPIO_H */
