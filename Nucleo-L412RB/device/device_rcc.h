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
 * @file device_rcc.h
 * @brief  Defines registers used for Reset Clock Control on Arm devices
 * @details Outlines the portable device register abstraction used to configure and interact with hardware peripherals
 * on any supported platform.
 *
 * @author  Jack Wilson
 * @date    September 26, 2025
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Jack Wilson
 * @license MIT License
 */

#ifndef DEVICE_RCC_H
#define DEVICE_RCC_H

#include "device_regs.h"
#include <stdint.h>

// lint -esym(528,*) There are definitions they may not be used
// lint -esym(750,*) There are definitions they may not be used
// lint -esym(751,*) There are definitions they may not be used
// lint -esym(754,*) There are definitions they may not be used
// lint -esym(749,*) There are definitions they may not be used

// NOTE:  This does not define every possible register or bit
// More can be added from the datasheet

/**
 * @brief  RCC PLL configuration structure definition
 */
typedef struct
{
    volatile uint32_t pll_state; /*!< The new state of the PLL.
                          This parameter can be a value of @ref RCC_PLL_Config                      */

    volatile uint32_t pll_source; /*!< RCC_PLLSource: PLL entry clock source.
                           This parameter must be a value of @ref RCC_PLL_Clock_Source               */

    volatile uint32_t pllm; /*!< PLLM: Division factor for PLL VCO input clock.
                      This parameter must be a number between Min_Data = 0 and Max_Data = 63    */

    volatile uint32_t plln; /*!< PLLN: Multiplication factor for PLL VCO output clock.
                      This parameter must be a number between Min_Data = 50 and Max_Data = 432
                      except for STM32F411xE devices where the Min_Data = 192 */

    volatile uint32_t pllp; /*!< PLLP: Division factor for main system clock (SYSCLK).
                      This parameter must be a value of @ref RCC_PLLP_Clock_Divider             */

    volatile uint32_t pllq; /*!< PLLQ: Division factor for OTG FS, SDIO and RNG clocks.
                      This parameter must be a number between Min_Data = 2 and Max_Data = 15    */
#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) ||                    \
    defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) ||                    \
    defined(STM32F412Rx) || defined(STM32F412Cx) || defined(STM32F413xx) || defined(STM32F423xx)
    volatile uint32_t pllr; /*!< PLLR: PLL division factor for I2S, SAI, SYSTEM, SPDIFRX clocks.
                      This parameter is only available in STM32F410xx/STM32F446xx/STM32F469xx/STM32F479xx
                      and STM32F412Zx/STM32F412Vx/STM32F412Rx/STM32F412Cx/STM32F413xx/STM32F423xx devices.
                      This parameter must be a number between Min_Data = 2 and Max_Data = 7     */
#endif /* STM32F410xx || STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Vx || STM32F412Rx ||     \
          STM32F412Cx || STM32F413xx || STM32F423xx */
} rcc_pll_init_t;

/**
 * @brief  RCC Internal/External Oscillator (HSE, HSI, LSE and LSI) configuration structure definition
 */
typedef struct
{
    volatile uint32_t oscillator_type; /*!< The oscillators to be configured.
                                This parameter can be a value of @ref RCC_Oscillator_Type                   */

    volatile uint32_t hse_state; /*!< The new state of the HSE.
                          This parameter can be a value of @ref RCC_HSE_Config                        */

    volatile uint32_t lse_state; /*!< The new state of the LSE.
                          This parameter can be a value of @ref RCC_LSE_Config                        */

    volatile uint32_t hsi_state; /*!< The new state of the HSI.
                          This parameter can be a value of @ref RCC_HSI_Config                        */

    volatile uint32_t
        hsi_calibration_value; /*!< The HSI calibration trimming value (default is RCC_HSICALIBRATION_DEFAULT).
                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x1F */

    volatile uint32_t lsi_state; /*!< The new state of the LSI.
                          This parameter can be a value of @ref RCC_LSI_Config                        */

    volatile rcc_pll_init_t pll; /*!< PLL structure parameters                                                    */
} rcc_osc_init_t;

// Reset Clock Control
#define REG_RCC_BASE (REG_AHB1PERIPH_BASE + 0x1000U)

typedef struct
{
    volatile uint32_t cr;          // offset 0x00
    volatile uint32_t icscr;       // offset 0x04
    volatile uint32_t cfgr;        // offset 0x08
    volatile uint32_t pllcfgr;     // offset 0x0c
    volatile uint32_t pllsai1cfgr; // offset 0x10
    uint32_t          reserved0;   // offset 0x14
    volatile uint32_t cier;        // offset 0x18
    volatile uint32_t cifr;        // offset 0x1c
    volatile uint32_t cicr;        // offset 0x20
    uint32_t          reserved1;   // offset 0x24
    volatile uint32_t ahb1rstr;    // offset 0x28
    volatile uint32_t ahb2rstr;    // offset 0x2c
    volatile uint32_t ahb3rstr;    // offset 0x30
    uint32_t          reserved2;   // offset 0x34
    volatile uint32_t apb1rstr1;   // offset 0x38
    volatile uint32_t apb1rstr2;   // offset 0x3c
    volatile uint32_t apb2rstr;    // offset 0x40
    uint32_t          reserved3;   // offset 0x44
    volatile uint32_t ahb1enr;     // offset 0x48
    volatile uint32_t ahb2enr;     // offset 0x4c
    volatile uint32_t ahb3enr;     // offset 0x50
    uint32_t          reserved4;   // offset 0x54
    volatile uint32_t apb1enr1;    // offset 0x58
    volatile uint32_t apb1enr2;    // offset 0x5c
    volatile uint32_t apb2enr;     // offset 0x60
    uint32_t          reserved5;   // offset 0x64
    volatile uint32_t ahb1smenr;   // offset 0x68
    volatile uint32_t ahb2smenr;   // offset 0x6c
    volatile uint32_t ahb3smenr;   // offset 0x70
    uint32_t          reserved6;   // offset 0x74
    volatile uint32_t apb1smenr1;  // offset 0x78
    volatile uint32_t apb1smenr2;  // offset 0x7c
    volatile uint32_t apb2smenr;   // offset 0x80
    uint32_t          reserved7;   // offset 0x84
    volatile uint32_t ccipr;       // offset 0x88
    uint32_t          reserved8;   // offset 0x8c
    volatile uint32_t bdcr;        // offset 0x90
    volatile uint32_t csr;         // offset 0x94
    volatile uint32_t crrcr;       // offset 0x98
    volatile uint32_t ccipr2;      // offset 0x9c

} reg_rcc_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_rcc_t *p_device_rcc = (reg_rcc_t *const)(REG_RCC_BASE);
/********************  Bit definition for RCC_CR register  ********************/
#define RCC_CR_MSION ((uint32_t)0x00000001)    /*!< Internal Multi Speed oscillator (MSI) clock enable */
#define RCC_CR_MSIRDY ((uint32_t)0x00000002)   /*!< Internal Multi Speed oscillator (MSI) clock ready flag */
#define RCC_CR_MSIPLLEN ((uint32_t)0x00000004) /*!< Internal Multi Speed oscillator (MSI) PLL enable */
#define RCC_CR_MSIRGSEL ((uint32_t)0x00000008) /*!< Internal Multi Speed oscillator (MSI) range selection */

/*!< MSIRANGE configuration : 12 frequency ranges available */
#define RCC_CR_MSIRANGE ((uint32_t)0x000000F0)    /*!< Internal Multi Speed oscillator (MSI) clock Range */
#define RCC_CR_MSIRANGE_0 ((uint32_t)0x00000000)  /*!< Internal Multi Speed oscillator (MSI) clock Range 100 KHz */
#define RCC_CR_MSIRANGE_1 ((uint32_t)0x00000010)  /*!< Internal Multi Speed oscillator (MSI) clock Range 200 KHz */
#define RCC_CR_MSIRANGE_2 ((uint32_t)0x00000020)  /*!< Internal Multi Speed oscillator (MSI) clock Range 400 KHz */
#define RCC_CR_MSIRANGE_3 ((uint32_t)0x00000030)  /*!< Internal Multi Speed oscillator (MSI) clock Range 800 KHz */
#define RCC_CR_MSIRANGE_4 ((uint32_t)0x00000040)  /*!< Internal Multi Speed oscillator (MSI) clock Range 1 MHz */
#define RCC_CR_MSIRANGE_5 ((uint32_t)0x00000050)  /*!< Internal Multi Speed oscillator (MSI) clock Range 2 MHz */
#define RCC_CR_MSIRANGE_6 ((uint32_t)0x00000060)  /*!< Internal Multi Speed oscillator (MSI) clock Range 4  MHz */
#define RCC_CR_MSIRANGE_7 ((uint32_t)0x00000070)  /*!< Internal Multi Speed oscillator (MSI) clock Range 8 KHz */
#define RCC_CR_MSIRANGE_8 ((uint32_t)0x00000080)  /*!< Internal Multi Speed oscillator (MSI) clock Range 16 MHz */
#define RCC_CR_MSIRANGE_9 ((uint32_t)0x00000090)  /*!< Internal Multi Speed oscillator (MSI) clock Range 24 MHz */
#define RCC_CR_MSIRANGE_10 ((uint32_t)0x000000A0) /*!< Internal Multi Speed oscillator (MSI) clock Range 32 MHz */
#define RCC_CR_MSIRANGE_11 ((uint32_t)0x000000B0) /*!< Internal Multi Speed oscillator (MSI) clock Range 48  MHz */

#define RCC_CR_HSION ((uint32_t)0x00000100) /*!< Internal High Speed oscillator (HSI16) clock enable */
#define RCC_CR_HSIKERON                                                                                                \
    ((uint32_t)0x00000200) /*!< Internal High Speed oscillator (HSI16) clock enable for some IPs Kernel */
#define RCC_CR_HSIRDY ((uint32_t)0x00000400)  /*!< Internal High Speed oscillator (HSI16) clock ready flag */
#define RCC_CR_HSIASFS ((uint32_t)0x00000800) /*!< HSI16 Automatic Start from Stop */

#define RCC_CR_HSEON ((uint32_t)0x00010000)  /*!< External High Speed oscillator (HSE) clock enable */
#define RCC_CR_HSERDY ((uint32_t)0x00020000) /*!< External High Speed oscillator (HSE) clock ready */
#define RCC_CR_HSEBYP ((uint32_t)0x00040000) /*!< External High Speed oscillator (HSE) clock bypass */
#define RCC_CR_CSSON ((uint32_t)0x00080000)  /*!< HSE Clock Security System enable */

#define RCC_CR_PLLON ((uint32_t)0x01000000)      /*!< System PLL clock enable */
#define RCC_CR_PLLRDY ((uint32_t)0x02000000)     /*!< System PLL clock ready */
#define RCC_CR_PLLSAI1ON ((uint32_t)0x04000000)  /*!< SAI1 PLL enable */
#define RCC_CR_PLLSAI1RDY ((uint32_t)0x08000000) /*!< SAI1 PLL ready */
#define RCC_CR_PLLSAI2ON ((uint32_t)0x10000000)  /*!< SAI2 PLL enable */
#define RCC_CR_PLLSAI2RDY ((uint32_t)0x20000000) /*!< SAI2 PLL ready */

/*!< SW configuration */
#define RCC_CFGR_SW ((uint32_t)0x00000003)   /*!< SW[1:0] bits (System clock Switch) */
#define RCC_CFGR_SW_0 ((uint32_t)0x00000001) /*!<Bit 0 */
#define RCC_CFGR_SW_1 ((uint32_t)0x00000002) /*!<Bit 1 */

#define RCC_CFGR_SW_MSI ((uint32_t)0x00000000) /*!< MSI oscillator selection as system clock */
#define RCC_CFGR_SW_HSI ((uint32_t)0x00000001) /*!< HSI16 oscillator selection as system clock */
#define RCC_CFGR_SW_HSE ((uint32_t)0x00000002) /*!< HSE oscillator selection as system clock */
#define RCC_CFGR_SW_PLL ((uint32_t)0x00000003) /*!< PLL selection as system clock */

/*!< SWS configuration */
#define RCC_CFGR_SWS ((uint32_t)0x0000000C)   /*!< SWS[1:0] bits (System Clock Switch Status) */
#define RCC_CFGR_SWS_0 ((uint32_t)0x00000004) /*!<Bit 0 */
#define RCC_CFGR_SWS_1 ((uint32_t)0x00000008) /*!<Bit 1 */

#define RCC_CFGR_SWS_MSI ((uint32_t)0x00000000) /*!< MSI oscillator used as system clock */
#define RCC_CFGR_SWS_HSI ((uint32_t)0x00000004) /*!< HSI16 oscillator used as system clock */
#define RCC_CFGR_SWS_HSE ((uint32_t)0x00000008) /*!< HSE oscillator used as system clock */
#define RCC_CFGR_SWS_PLL ((uint32_t)0x0000000C) /*!< PLL used as system clock */
// create RCC_AHB1ENR register
// #define REG_RCC_AHB1ENR (*(volatile unsigned int *)(REG_RCC_BASE + 0x30U))

static uint32_t *p_device_rcc_ahb1enr = (uint32_t *const)(REG_RCC_BASE + 0x48U);
static uint32_t *p_device_rcc_ahb2enr = (uint32_t *const)(REG_RCC_BASE + 0x4CU);
#define RCC_AHB2ENR_GPIOAEN_BIT (1U << 0U)
#define RCC_AHB2ENR_GPIOBEN_BIT (1U << 1U)
#define RCC_AHB2ENR_GPIOCEN_BIT (1U << 2U)
#define RCC_AHB2ENR_GPIODEN_BIT (1U << 3U)
#define RCC_AHB2ENR_GPIOEEN_BIT (1U << 4U)
// #define RCC_AHB1ENR_GPIOFEN_BIT		(1U<<5U)
// #define RCC_AHB1ENR_GPIOGEN_BIT		(1U<<6U)
#define RCC_AHB2ENR_GPIOHEN_BIT (1U << 7U)
// #define RCC_AHB1ENR_GPIOIEN_BIT		(1U<<8U)
#define RCC_AHB1ENR_CRCEN_BIT (1U << 12U)
// #define RCC_AHB1ENR_BKPSRAMEN_BIT	(1U<<18U)
#define RCC_AHB1ENR_DMA1EN_BIT (1U)
#define RCC_AHB1ENR_DMA2EN_BIT (1U << 1U)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static uint32_t *p_device_rcc_apb1enr1 = (uint32_t *const)(REG_RCC_BASE + 0x58U);
#pragma GCC diagnostic pop

#define RCC_APB1ENR1_TIM2EN_BIT (0U)
#define RCC_APB1ENR1_TIM3EN_BIT (1U << 1U)
// #define RCC_APB1ENR_TIM4EN_BIT (1U << 2U)
// #define RCC_APB1ENR_TIM5EN_BIT (1U << 3U)
#define RCC_APB1ENR1_TIM6EN_BIT (1U << 4U)
#define RCC_APB1ENR1_TIM7EN_BIT (1U << 5U)
// #define RCC_APB1ENR_TIM12EN_BIT (1U << 6U)
// #define RCC_APB1ENR_TIM13EN_BIT (1U << 7U)
#define RCC_APB1ENR1_LCDEN_BIT (1U << 9U)
#define RCC_APB1ENR1_RTCAPEN_BIT (1U << 10U)
#define RCC_APB1ENR1_WWDGEN_BIT (1U << 11U)
#define RCC_APB1ENR1_SPI2EN_BIT (1U << 14U)
#define RCC_APB1ENR1_SPI3EN_BIT (1U << 15U)
#define RCC_APB1ENR1_USART2EN_BIT (1U << 17U)
#define RCC_APB1ENR1_USART3EN_BIT (1U << 18U)
#define RCC_APB1ENR1_UART4EN_BIT (1U << 19U)
#define RCC_APB1ENR1_UART5EN_BIT (1U << 20U)
#define RCC_APB1ENR1_I2C1EN_BIT (1U << 21U)
#define RCC_APB1ENR1_I2C2EN_BIT (1U << 22U)
#define RCC_APB1ENR1_I2C3EN_BIT (1U << 23U)
#define RCC_APB1ENR1_CRSEN_BIT (1U << 24U)
#define RCC_APB1ENR1_CAN1EN_BIT (1U << 25U)
#define RCC_APB1ENR1_USBFSEN_BIT (1U << 26U)
#define RCC_APB1ENR1_PWREN_BIT (1U << 28U)
#define RCC_APB1ENR1_DACEN_BIT (1U << 29U)
#define RCC_APB1ENR1_OPAMPEN_BIT (1U << 30U)
#define RCC_APB1ENR1_LPTIM1EN_BIT (1U << 31U)

static uint32_t *p_device_rcc_apb1enr2 = (uint32_t *const)(REG_RCC_BASE + 0x5CU);
#define RCC_APB1ENR2_LPUART1EN_BIT (0U)
#define RCC_APB1ENR2_I2C4EN_BIT (1U << 1U)
#define RCC_APB1ENR2_SWPMI1EN_BIT (1U << 2U)
#define RCC_APB1ENR2_LPTIM2EN_BIT (1U << 5U)

static uint32_t *p_device_rcc_apb2enr = (uint32_t *const)(REG_RCC_BASE + 0x60U);

#define RCC_APB2ENR_SYSCFGEN_BIT (1U << 0U)
#define RCC_APB2ENR_FWEN_BIT (1U << 7U)
#define RCC_APB2ENR_SDMMC1EN_BIT (1U << 10U)
#define RCC_APB2ENR_TIM1EN_BIT (1U << 11U)
#define RCC_APB2ENR_SPI1EN_BIT (1U << 12U)
#define RCC_APB2ENR_USART1EN_BIT (1U << 14U)
#define RCC_APB2ENR_TIM15EN_BIT (1U << 16U)
#define RCC_APB2ENR_TIM16EN_BIT (1U << 17U)
#define RCC_APB2ENR_SAI1EN_BIT (1U << 21U)
#define RCC_APB2ENR_DFSDM1EN_BIT (1U << 24U)

#endif /* DEVICE_RCC_H */
