/** @file gpio.h
 * 
 * @brief defines registers used for GPIO on Arm devices
 *
 *
 * @par
 * COPYWRITE NOTICE: (c) 2022 Jack Wilson.  All rights reserved
 *
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "regs.h"

//lint -esym(528,*) There are definitions they may not be used
//lint -esym(750,*) There are definitions they may not be used
//lint -esym(751,*) There are definitions they may not be used
//lint -esym(754,*) There are definitions they may not be used 
//lint -esym(749,*) There are definitions they may not be used 

// NOTE:  This does not define every possible register or bit 
// More can be added from the datasheet

#define REG_GPIOA_EN (1U)	
#define REG_GPIOB_EN (1U<<1U)	
#define REG_GPIOC_EN (1U<<2U)	
#define REG_GPIOD_EN (1U<<3U)
#define REG_GPIOE_EN (1U<<4U)
#define REG_GPIOF_EN (1U<<5U)
#define REG_GPIOG_EN (1U<<6U)
#define REG_GPIOH_EN (1U<<7U)
#define REG_GPIOI_EN (1U<<8U)


// GPIO
#define REG_GPIOA_BASE (REG_AHB1PERIPH_BASE)
#define REG_GPIOB_BASE (REG_AHB1PERIPH_BASE + 0x0400U)
#define REG_GPIOC_BASE (REG_AHB1PERIPH_BASE + 0x0800U)
#define REG_GPIOD_BASE (REG_AHB1PERIPH_BASE + 0x0C00U)
#define REG_GPIOE_BASE (REG_AHB1PERIPH_BASE + 0x1000U)
//#define REG_GPIOF_BASE (REG_AHB1PERIPH_BASE + 0x1400U)
//#define REG_GPIOG_BASE (REG_AHB1PERIPH_BASE + 0x1800U)
#define REG_GPIOH_BASE (REG_AHB1PERIPH_BASE + 0x1C00U)
//#define REG_GPIOI_BASE (REG_AHB1PERIPH_BASE + 0x2000U)

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

//lint -e844
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_gpio_t * p_gpioa = ((reg_gpio_t * const) REG_GPIOA_BASE); //!< The GPIO registers
static reg_gpio_t * p_gpiob = ((reg_gpio_t * const) REG_GPIOB_BASE); //lint !e844
static reg_gpio_t * p_gpioc = ((reg_gpio_t * const) REG_GPIOC_BASE); //lint !e844
static reg_gpio_t * p_gpiod = ((reg_gpio_t * const) REG_GPIOD_BASE); //lint !e844
static reg_gpio_t * p_gpioe = ((reg_gpio_t * const) REG_GPIOE_BASE); //lint !e844
//static reg_gpio_t * p_gpiof = ((reg_gpio_t * const) REG_GPIOF_BASE); //lint !e844
//static reg_gpio_t * p_gpiog = ((reg_gpio_t * const) REG_GPIOG_BASE); //lint !e844
static reg_gpio_t * p_gpioh = ((reg_gpio_t * const) REG_GPIOH_BASE); //lint !e844
//static reg_gpio_t * p_gpioi = ((reg_gpio_t * const) REG_GPIOI_BASE); //lint !e844

#pragma GCC diagnostic pop

typedef enum {
	eGPIO_MODE_INPUT = 00U, 
	eGPIO_MODE_OUTPUT = 01U,
	eGPIO_MODE_ALTERNATE = 02U,
	eGPIO_MODE_ANALOG = 03U
} gpio_modes_t; 

typedef uint32_t pin_t; //lint -esym(751,pin_t) suppress unused  

typedef volatile struct 
{
	reg_gpio_t *port;
	pin_t		pin;
} gpio_address_t; 


#endif /* GPIO_H */
