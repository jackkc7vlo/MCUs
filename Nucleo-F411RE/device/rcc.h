/** @file rcc.h
 * 
 * @brief defines registers used for Reset Clock Control on Arm devices
 *
 *
 * @par
 * COPYWRITE NOTICE: (c) 2022 Jack Wilson.  All rights reserved
 *
 */

#ifndef RCC_H
#define RCC_H

#include <stdint.h>
#include "regs.h"

//lint -esym(528,*) There are definitions they may not be used
//lint -esym(750,*) There are definitions they may not be used
//lint -esym(751,*) There are definitions they may not be used
//lint -esym(754,*) There are definitions they may not be used 
//lint -esym(749,*) There are definitions they may not be used 

// NOTE:  This does not define every possible register or bit 
// More can be added from the datasheet


/**
  * @brief  RCC PLL configuration structure definition
  */
typedef struct
{
    volatile uint32_t pll_state;   /*!< The new state of the PLL.
                            This parameter can be a value of @ref RCC_PLL_Config                      */

    volatile uint32_t pll_source;  /*!< RCC_PLLSource: PLL entry clock source.
                            This parameter must be a value of @ref RCC_PLL_Clock_Source               */

    volatile uint32_t pllm;       /*!< PLLM: Division factor for PLL VCO input clock.
                            This parameter must be a number between Min_Data = 0 and Max_Data = 63    */

    volatile uint32_t plln;       /*!< PLLN: Multiplication factor for PLL VCO output clock.
                            This parameter must be a number between Min_Data = 50 and Max_Data = 432 
                            except for STM32F411xE devices where the Min_Data = 192 */

    volatile uint32_t pllp;       /*!< PLLP: Division factor for main system clock (SYSCLK).
                            This parameter must be a value of @ref RCC_PLLP_Clock_Divider             */

    volatile uint32_t pllq;       /*!< PLLQ: Division factor for OTG FS, SDIO and RNG clocks.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 15    */
#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
    defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
    defined(STM32F413xx) || defined(STM32F423xx)
    volatile uint32_t pllr;       /*!< PLLR: PLL division factor for I2S, SAI, SYSTEM, SPDIFRX clocks.
                            This parameter is only available in STM32F410xx/STM32F446xx/STM32F469xx/STM32F479xx
                            and STM32F412Zx/STM32F412Vx/STM32F412Rx/STM32F412Cx/STM32F413xx/STM32F423xx devices. 
                            This parameter must be a number between Min_Data = 2 and Max_Data = 7     */
#endif /* STM32F410xx || STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx || STM32F413xx || STM32F423xx */ 
} rcc_pll_init_t;


/**
  * @brief  RCC Internal/External Oscillator (HSE, HSI, LSE and LSI) configuration structure definition
  */ 
typedef struct
{
    volatile uint32_t oscillator_type;       /*!< The oscillators to be configured.
                                      This parameter can be a value of @ref RCC_Oscillator_Type                   */

    volatile uint32_t hse_state;             /*!< The new state of the HSE.
                                      This parameter can be a value of @ref RCC_HSE_Config                        */

    volatile uint32_t lse_state;             /*!< The new state of the LSE.
                                      This parameter can be a value of @ref RCC_LSE_Config                        */

    volatile uint32_t hsi_state;             /*!< The new state of the HSI.
                                      This parameter can be a value of @ref RCC_HSI_Config                        */

    volatile uint32_t hsi_calibration_value;  /*!< The HSI calibration trimming value (default is RCC_HSICALIBRATION_DEFAULT).
                                       This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x1F */

    volatile uint32_t lsi_state;             /*!< The new state of the LSI.
                                      This parameter can be a value of @ref RCC_LSI_Config                        */

    volatile rcc_pll_init_t pll;        /*!< PLL structure parameters                                                    */
} rcc_osc_init_t;



// Reset Clock Control
#define REG_RCC_BASE (REG_AHB1PERIPH_BASE + 0x3800U)

typedef struct
{
	volatile uint32_t cr;          // offset 0x00 
  volatile uint32_t pllcfgr;     // offset 0x04
  volatile uint32_t cfgr;        // offset 0x08
  volatile uint32_t cir;         // offset 0x0c
  volatile uint32_t ahb1rstr;    // offset 0x10
  volatile uint32_t ahb2rstr;    // offset 0x14
  volatile uint32_t ahb3rstr;    // offset 0x18
  uint32_t      reserved0;       // offset 0x1c
  volatile uint32_t apb1rstr;    // offset 0x20
  volatile uint32_t apb2rstr;    // offset 0x24
  uint32_t      reserved1[2];    // offset 0x28-0x2c
  volatile uint32_t ahb1enr;     // offset 0x30
  volatile uint32_t ahb2enr;     // offset 0x34
  volatile uint32_t ahb3enr;     // offset 0x38
  uint32_t      reserved2;       // offset 0x3c
  volatile uint32_t apb1enr;     // offset 0x40
  volatile uint32_t apb2enr;     // offset 0x44
  uint32_t      reserved3[2];    // offset 0x48-0x4c
  volatile uint32_t ahb1lpenr;   // offset 0x50
  volatile uint32_t ahb2lpenr;   // offset 0x54
  volatile uint32_t ahb3lpenr;   // offset 0x58
  uint32_t      reserved4;       // offset 0x5c
  volatile uint32_t apb1lpenr;   // offset 0x60
  volatile uint32_t apb2lpenr;   // offset 0x64
  uint32_t      reserved5[2];    // offset 0x68-0x6c
  volatile uint32_t bdcr;        // offset 0x70
  volatile uint32_t csr;         // offset 0x74
  uint32_t      reserved6[2];    // offset 0x78-0x7c
  volatile uint32_t sscgr;       // offset 0x80
  volatile uint32_t plli2scfgr;  // offset 0x84
  uint32_t     reserved7;        // offset 0x88
  volatile uint32_t dckcfgr;     // offset 0x8c
} reg_rcc_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_rcc_t * p_rcc = (reg_rcc_t * const) (REG_RCC_BASE);
// create RCC_AHB1ENR register
//#define REG_RCC_AHB1ENR (*(volatile unsigned int *)(REG_RCC_BASE + 0x30U))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

static uint32_t * p_rcc_ahb1enr = (uint32_t * const) (REG_RCC_BASE + 0x30U);	
#define RCC_AHB1ENR_GPIOAEN_BIT		(0U)
#define RCC_AHB1ENR_GPIOBEN_BIT		(1U<<1U)
#define RCC_AHB1ENR_GPIOCEN_BIT		(1U<<2U)
#define RCC_AHB1ENR_GPIODEN_BIT		(1U<<3U)
#define RCC_AHB1ENR_GPIOEEN_BIT		(1U<<4U)
//#define RCC_AHB1ENR_GPIOFEN_BIT		(1U<<5U)
//#define RCC_AHB1ENR_GPIOGEN_BIT		(1U<<6U)
#define RCC_AHB1ENR_GPIOHEN_BIT		(1U<<7U)
//#define RCC_AHB1ENR_GPIOIEN_BIT		(1U<<8U)
#define RCC_AHB1ENR_CRCEN_BIT		(1U<<12U)
//#define RCC_AHB1ENR_BKPSRAMEN_BIT	(1U<<18U)
#define RCC_AHB1ENR_DMA1EN_BIT		(1U<<21U)
#define RCC_AHB1ENR_DMA2EN_BIT		(1U<<22U)


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static uint32_t * p_rcc_apb1enr = (uint32_t * const) (REG_RCC_BASE + 0x40U);	
#pragma GCC diagnostic pop

#define RCC_APB1ENR_TIM2EN_BIT		(1U)
#define RCC_APB1ENR_TIM3EN_BIT		(1U<<1U)
#define RCC_APB1ENR_TIM4EN_BIT		(1U<<2U)
#define RCC_APB1ENR_TIM5EN_BIT		(1U<<3U)
#define RCC_APB1ENR_TIM6EN_BIT		(1U<<4U)
#define RCC_APB1ENR_TIM7EN_BIT		(1U<<5U)
#define RCC_APB1ENR_TIM12EN_BIT		(1U<<6U)
#define RCC_APB1ENR_TIM13EN_BIT	 	(1U<<7U)
#define RCC_APB1ENR_TIM14EN_BIT		(1U<<8U)
#define RCC_APB1ENR_WWDGEN_BIT		(1U<<11U)
#define RCC_APB1ENR_SPI2EN_BIT		(1U<<14U)
#define RCC_APB1ENR_SPI3EN_BIT		(1U<<15U)
#define RCC_APB1ENR_USART2EN_BIT	(1U<<17U)
#define RCC_APB1ENR_USART3EN_BIT	(1U<<18U)
#define RCC_APB1ENR_UART4EN_BIT		(1U<<19U)
#define RCC_APB1ENR_UART5EN_BIT		(1U<<20U)
#define RCC_APB1ENR_I2C1EN_BIT		(1U<<21U)
#define RCC_APB1ENR_I2C2EN_BIT		(1U<<22U)
#define RCC_APB1ENR_I2C3EN_BIT		(1U<<23U)
#define RCC_APB1ENR_CAN1EN_BIT		(1U<<25U)
#define RCC_APB1ENR_CAN2EN_BIT		(1U<<26U)
#define RCC_APB1ENR_PWREN_BIT			(1U<<28U)
#define RCC_APB1ENR_DACEN_BIT			(1U<<29U)



#endif /* GPIO_H */
