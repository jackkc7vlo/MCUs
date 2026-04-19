/******************************************************************************
 * Copyright (C) 2026 by Jack Wilson
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
 ************************(C) COPYRIGHT 2026 Jack Wilson **********************/
/**
 * @file device_spi.h
 * @brief  Defines registers used for SPI peripherals on STM32F401xE.
 *
 * @author  Jack Wilson
 * @date    February 26, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

#ifndef DEVICE_SPI_H
#define DEVICE_SPI_H

#include "device_regs.h"
#include <stdint.h>

// lint -esym(528,*) There are definitions they may not be used
// lint -esym(750,*) There are definitions they may not be used
// lint -esym(751,*) There are definitions they may not be used
// lint -esym(754,*) There are definitions they may not be used
// lint -esym(749,*) There are definitions they may not be used

// NOTE:  This does not define every possible register or bit
// More can be added from the datasheet

/********************************************************************************
 * SPI peripheral base addresses (STM32F401xE)
 ********************************************************************************/
#define REG_SPI1_BASE (REG_APB2PERIPH_BASE + 0x3000U)
#define REG_SPI2_BASE (REG_APB1PERIPH_BASE + 0x3800U)
#define REG_SPI3_BASE (REG_APB1PERIPH_BASE + 0x3C00U)
// #define REG_SPI4_BASE (REG_APB2PERIPH_BASE + 0x3400U)

/********************************************************************************
 * SPI register map (RM0368 §18.6)
 ********************************************************************************/
/**
 * @brief Serial Peripheral Interface
 */

typedef struct
{
    volatile uint32_t cr1;    /*!< SPI control register 1 (not used in I2S mode),      Address offset: 0x00 */
    volatile uint32_t cr2;    /*!< SPI control register 2,                             Address offset: 0x04 */
    volatile uint32_t sr;     /*!< SPI status register,                                Address offset: 0x08 */
    volatile uint32_t dr;     /*!< SPI data register,                                  Address offset: 0x0C */
    volatile uint32_t crcpr;  /*!< SPI CRC polynomial register (not used in I2S mode), Address offset: 0x10 */
    volatile uint32_t rxcrcr; /*!< SPI RX CRC register (not used in I2S mode),         Address offset: 0x14 */
    volatile uint32_t txcrcr; /*!< SPI TX CRC register (not used in I2S mode),         Address offset: 0x18 */
} reg_spi_t;

/********************************************************************************
 * Peripheral pointer instances
 ********************************************************************************/
// lint -e844
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_spi_t *p_device_spi1 = ((reg_spi_t *const)REG_SPI1_BASE);
static reg_spi_t *p_device_spi2 = ((reg_spi_t *const)REG_SPI2_BASE);
static reg_spi_t *p_device_spi3 = ((reg_spi_t *const)REG_SPI3_BASE);
// static reg_spi_t *p_device_spi4 = ((reg_spi_t *const)REG_SPI4_BASE);
#pragma GCC diagnostic pop

/******************  Bits definition for GPIO_MODER register  *****************/
#define GPIO_MODER_MODER0_Pos (0U)
#define GPIO_MODER_MODER0_Msk (0x3UL << GPIO_MODER_MODER0_Pos) /*!< 0x00000003 */
#define GPIO_MODER_MODER0 GPIO_MODER_MODER0_Msk
#define GPIO_MODER_MODER0_0 (0x1UL << GPIO_MODER_MODER0_Pos) /*!< 0x00000001 */
#define GPIO_MODER_MODER0_1 (0x2UL << GPIO_MODER_MODER0_Pos) /*!< 0x00000002 */
#define GPIO_MODER_MODER1_Pos (2U)
#define GPIO_MODER_MODER1_Msk (0x3UL << GPIO_MODER_MODER1_Pos) /*!< 0x0000000C */
#define GPIO_MODER_MODER1 GPIO_MODER_MODER1_Msk
#define GPIO_MODER_MODER1_0 (0x1UL << GPIO_MODER_MODER1_Pos) /*!< 0x00000004 */
#define GPIO_MODER_MODER1_1 (0x2UL << GPIO_MODER_MODER1_Pos) /*!< 0x00000008 */
#define GPIO_MODER_MODER2_Pos (4U)
#define GPIO_MODER_MODER2_Msk (0x3UL << GPIO_MODER_MODER2_Pos) /*!< 0x00000030 */
#define GPIO_MODER_MODER2 GPIO_MODER_MODER2_Msk
#define GPIO_MODER_MODER2_0 (0x1UL << GPIO_MODER_MODER2_Pos) /*!< 0x00000010 */
#define GPIO_MODER_MODER2_1 (0x2UL << GPIO_MODER_MODER2_Pos) /*!< 0x00000020 */
#define GPIO_MODER_MODER3_Pos (6U)
#define GPIO_MODER_MODER3_Msk (0x3UL << GPIO_MODER_MODER3_Pos) /*!< 0x000000C0 */
#define GPIO_MODER_MODER3 GPIO_MODER_MODER3_Msk
#define GPIO_MODER_MODER3_0 (0x1UL << GPIO_MODER_MODER3_Pos) /*!< 0x00000040 */
#define GPIO_MODER_MODER3_1 (0x2UL << GPIO_MODER_MODER3_Pos) /*!< 0x00000080 */
#define GPIO_MODER_MODER4_Pos (8U)
#define GPIO_MODER_MODER4_Msk (0x3UL << GPIO_MODER_MODER4_Pos) /*!< 0x00000300 */
#define GPIO_MODER_MODER4 GPIO_MODER_MODER4_Msk
#define GPIO_MODER_MODER4_0 (0x1UL << GPIO_MODER_MODER4_Pos) /*!< 0x00000100 */
#define GPIO_MODER_MODER4_1 (0x2UL << GPIO_MODER_MODER4_Pos) /*!< 0x00000200 */
#define GPIO_MODER_MODER5_Pos (10U)
#define GPIO_MODER_MODER5_Msk (0x3UL << GPIO_MODER_MODER5_Pos) /*!< 0x00000C00 */
#define GPIO_MODER_MODER5 GPIO_MODER_MODER5_Msk
#define GPIO_MODER_MODER5_0 (0x1UL << GPIO_MODER_MODER5_Pos) /*!< 0x00000400 */
#define GPIO_MODER_MODER5_1 (0x2UL << GPIO_MODER_MODER5_Pos) /*!< 0x00000800 */
#define GPIO_MODER_MODER6_Pos (12U)
#define GPIO_MODER_MODER6_Msk (0x3UL << GPIO_MODER_MODER6_Pos) /*!< 0x00003000 */
#define GPIO_MODER_MODER6 GPIO_MODER_MODER6_Msk
#define GPIO_MODER_MODER6_0 (0x1UL << GPIO_MODER_MODER6_Pos) /*!< 0x00001000 */
#define GPIO_MODER_MODER6_1 (0x2UL << GPIO_MODER_MODER6_Pos) /*!< 0x00002000 */
#define GPIO_MODER_MODER7_Pos (14U)
#define GPIO_MODER_MODER7_Msk (0x3UL << GPIO_MODER_MODER7_Pos) /*!< 0x0000C000 */
#define GPIO_MODER_MODER7 GPIO_MODER_MODER7_Msk
#define GPIO_MODER_MODER7_0 (0x1UL << GPIO_MODER_MODER7_Pos) /*!< 0x00004000 */
#define GPIO_MODER_MODER7_1 (0x2UL << GPIO_MODER_MODER7_Pos) /*!< 0x00008000 */
#define GPIO_MODER_MODER8_Pos (16U)
#define GPIO_MODER_MODER8_Msk (0x3UL << GPIO_MODER_MODER8_Pos) /*!< 0x00030000 */
#define GPIO_MODER_MODER8 GPIO_MODER_MODER8_Msk
#define GPIO_MODER_MODER8_0 (0x1UL << GPIO_MODER_MODER8_Pos) /*!< 0x00010000 */
#define GPIO_MODER_MODER8_1 (0x2UL << GPIO_MODER_MODER8_Pos) /*!< 0x00020000 */
#define GPIO_MODER_MODER9_Pos (18U)
#define GPIO_MODER_MODER9_Msk (0x3UL << GPIO_MODER_MODER9_Pos) /*!< 0x000C0000 */
#define GPIO_MODER_MODER9 GPIO_MODER_MODER9_Msk
#define GPIO_MODER_MODER9_0 (0x1UL << GPIO_MODER_MODER9_Pos) /*!< 0x00040000 */
#define GPIO_MODER_MODER9_1 (0x2UL << GPIO_MODER_MODER9_Pos) /*!< 0x00080000 */
#define GPIO_MODER_MODER10_Pos (20U)
#define GPIO_MODER_MODER10_Msk (0x3UL << GPIO_MODER_MODER10_Pos) /*!< 0x00300000 */
#define GPIO_MODER_MODER10 GPIO_MODER_MODER10_Msk
#define GPIO_MODER_MODER10_0 (0x1UL << GPIO_MODER_MODER10_Pos) /*!< 0x00100000 */
#define GPIO_MODER_MODER10_1 (0x2UL << GPIO_MODER_MODER10_Pos) /*!< 0x00200000 */
#define GPIO_MODER_MODER11_Pos (22U)
#define GPIO_MODER_MODER11_Msk (0x3UL << GPIO_MODER_MODER11_Pos) /*!< 0x00C00000 */
#define GPIO_MODER_MODER11 GPIO_MODER_MODER11_Msk
#define GPIO_MODER_MODER11_0 (0x1UL << GPIO_MODER_MODER11_Pos) /*!< 0x00400000 */
#define GPIO_MODER_MODER11_1 (0x2UL << GPIO_MODER_MODER11_Pos) /*!< 0x00800000 */
#define GPIO_MODER_MODER12_Pos (24U)
#define GPIO_MODER_MODER12_Msk (0x3UL << GPIO_MODER_MODER12_Pos) /*!< 0x03000000 */
#define GPIO_MODER_MODER12 GPIO_MODER_MODER12_Msk
#define GPIO_MODER_MODER12_0 (0x1UL << GPIO_MODER_MODER12_Pos) /*!< 0x01000000 */
#define GPIO_MODER_MODER12_1 (0x2UL << GPIO_MODER_MODER12_Pos) /*!< 0x02000000 */
#define GPIO_MODER_MODER13_Pos (26U)
#define GPIO_MODER_MODER13_Msk (0x3UL << GPIO_MODER_MODER13_Pos) /*!< 0x0C000000 */
#define GPIO_MODER_MODER13 GPIO_MODER_MODER13_Msk
#define GPIO_MODER_MODER13_0 (0x1UL << GPIO_MODER_MODER13_Pos) /*!< 0x04000000 */
#define GPIO_MODER_MODER13_1 (0x2UL << GPIO_MODER_MODER13_Pos) /*!< 0x08000000 */
#define GPIO_MODER_MODER14_Pos (28U)
#define GPIO_MODER_MODER14_Msk (0x3UL << GPIO_MODER_MODER14_Pos) /*!< 0x30000000 */
#define GPIO_MODER_MODER14 GPIO_MODER_MODER14_Msk
#define GPIO_MODER_MODER14_0 (0x1UL << GPIO_MODER_MODER14_Pos) /*!< 0x10000000 */
#define GPIO_MODER_MODER14_1 (0x2UL << GPIO_MODER_MODER14_Pos) /*!< 0x20000000 */
#define GPIO_MODER_MODER15_Pos (30U)
#define GPIO_MODER_MODER15_Msk (0x3UL << GPIO_MODER_MODER15_Pos) /*!< 0xC0000000 */
#define GPIO_MODER_MODER15 GPIO_MODER_MODER15_Msk
#define GPIO_MODER_MODER15_0 (0x1UL << GPIO_MODER_MODER15_Pos) /*!< 0x40000000 */
#define GPIO_MODER_MODER15_1 (0x2UL << GPIO_MODER_MODER15_Pos) /*!< 0x80000000 */

/******************  Bits definition for GPIO_OTYPER register  ****************/
#define GPIO_OTYPER_OT0_Pos (0U)
#define GPIO_OTYPER_OT0_Msk (0x1UL << GPIO_OTYPER_OT0_Pos) /*!< 0x00000001 */
#define GPIO_OTYPER_OT0 GPIO_OTYPER_OT0_Msk
#define GPIO_OTYPER_OT1_Pos (1U)
#define GPIO_OTYPER_OT1_Msk (0x1UL << GPIO_OTYPER_OT1_Pos) /*!< 0x00000002 */
#define GPIO_OTYPER_OT1 GPIO_OTYPER_OT1_Msk
#define GPIO_OTYPER_OT2_Pos (2U)
#define GPIO_OTYPER_OT2_Msk (0x1UL << GPIO_OTYPER_OT2_Pos) /*!< 0x00000004 */
#define GPIO_OTYPER_OT2 GPIO_OTYPER_OT2_Msk
#define GPIO_OTYPER_OT3_Pos (3U)
#define GPIO_OTYPER_OT3_Msk (0x1UL << GPIO_OTYPER_OT3_Pos) /*!< 0x00000008 */
#define GPIO_OTYPER_OT3 GPIO_OTYPER_OT3_Msk
#define GPIO_OTYPER_OT4_Pos (4U)
#define GPIO_OTYPER_OT4_Msk (0x1UL << GPIO_OTYPER_OT4_Pos) /*!< 0x00000010 */
#define GPIO_OTYPER_OT4 GPIO_OTYPER_OT4_Msk
#define GPIO_OTYPER_OT5_Pos (5U)
#define GPIO_OTYPER_OT5_Msk (0x1UL << GPIO_OTYPER_OT5_Pos) /*!< 0x00000020 */
#define GPIO_OTYPER_OT5 GPIO_OTYPER_OT5_Msk
#define GPIO_OTYPER_OT6_Pos (6U)
#define GPIO_OTYPER_OT6_Msk (0x1UL << GPIO_OTYPER_OT6_Pos) /*!< 0x00000040 */
#define GPIO_OTYPER_OT6 GPIO_OTYPER_OT6_Msk
#define GPIO_OTYPER_OT7_Pos (7U)
#define GPIO_OTYPER_OT7_Msk (0x1UL << GPIO_OTYPER_OT7_Pos) /*!< 0x00000080 */
#define GPIO_OTYPER_OT7 GPIO_OTYPER_OT7_Msk
#define GPIO_OTYPER_OT8_Pos (8U)
#define GPIO_OTYPER_OT8_Msk (0x1UL << GPIO_OTYPER_OT8_Pos) /*!< 0x00000100 */
#define GPIO_OTYPER_OT8 GPIO_OTYPER_OT8_Msk
#define GPIO_OTYPER_OT9_Pos (9U)
#define GPIO_OTYPER_OT9_Msk (0x1UL << GPIO_OTYPER_OT9_Pos) /*!< 0x00000200 */
#define GPIO_OTYPER_OT9 GPIO_OTYPER_OT9_Msk
#define GPIO_OTYPER_OT10_Pos (10U)
#define GPIO_OTYPER_OT10_Msk (0x1UL << GPIO_OTYPER_OT10_Pos) /*!< 0x00000400 */
#define GPIO_OTYPER_OT10 GPIO_OTYPER_OT10_Msk
#define GPIO_OTYPER_OT11_Pos (11U)
#define GPIO_OTYPER_OT11_Msk (0x1UL << GPIO_OTYPER_OT11_Pos) /*!< 0x00000800 */
#define GPIO_OTYPER_OT11 GPIO_OTYPER_OT11_Msk
#define GPIO_OTYPER_OT12_Pos (12U)
#define GPIO_OTYPER_OT12_Msk (0x1UL << GPIO_OTYPER_OT12_Pos) /*!< 0x00001000 */
#define GPIO_OTYPER_OT12 GPIO_OTYPER_OT12_Msk
#define GPIO_OTYPER_OT13_Pos (13U)
#define GPIO_OTYPER_OT13_Msk (0x1UL << GPIO_OTYPER_OT13_Pos) /*!< 0x00002000 */
#define GPIO_OTYPER_OT13 GPIO_OTYPER_OT13_Msk
#define GPIO_OTYPER_OT14_Pos (14U)
#define GPIO_OTYPER_OT14_Msk (0x1UL << GPIO_OTYPER_OT14_Pos) /*!< 0x00004000 */
#define GPIO_OTYPER_OT14 GPIO_OTYPER_OT14_Msk
#define GPIO_OTYPER_OT15_Pos (15U)
#define GPIO_OTYPER_OT15_Msk (0x1UL << GPIO_OTYPER_OT15_Pos) /*!< 0x00008000 */
#define GPIO_OTYPER_OT15 GPIO_OTYPER_OT15_Msk

/******************  Bits definition for GPIO_OSPEEDR register  ***************/
#define GPIO_OSPEEDR_OSPEED0_Pos (0U)
#define GPIO_OSPEEDR_OSPEED0_Msk (0x3UL << GPIO_OSPEEDR_OSPEED0_Pos) /*!< 0x00000003 */
#define GPIO_OSPEEDR_OSPEED0 GPIO_OSPEEDR_OSPEED0_Msk
#define GPIO_OSPEEDR_OSPEED0_0 (0x1UL << GPIO_OSPEEDR_OSPEED0_Pos) /*!< 0x00000001 */
#define GPIO_OSPEEDR_OSPEED0_1 (0x2UL << GPIO_OSPEEDR_OSPEED0_Pos) /*!< 0x00000002 */
#define GPIO_OSPEEDR_OSPEED1_Pos (2U)
#define GPIO_OSPEEDR_OSPEED1_Msk (0x3UL << GPIO_OSPEEDR_OSPEED1_Pos) /*!< 0x0000000C */
#define GPIO_OSPEEDR_OSPEED1 GPIO_OSPEEDR_OSPEED1_Msk
#define GPIO_OSPEEDR_OSPEED1_0 (0x1UL << GPIO_OSPEEDR_OSPEED1_Pos) /*!< 0x00000004 */
#define GPIO_OSPEEDR_OSPEED1_1 (0x2UL << GPIO_OSPEEDR_OSPEED1_Pos) /*!< 0x00000008 */
#define GPIO_OSPEEDR_OSPEED2_Pos (4U)
#define GPIO_OSPEEDR_OSPEED2_Msk (0x3UL << GPIO_OSPEEDR_OSPEED2_Pos) /*!< 0x00000030 */
#define GPIO_OSPEEDR_OSPEED2 GPIO_OSPEEDR_OSPEED2_Msk
#define GPIO_OSPEEDR_OSPEED2_0 (0x1UL << GPIO_OSPEEDR_OSPEED2_Pos) /*!< 0x00000010 */
#define GPIO_OSPEEDR_OSPEED2_1 (0x2UL << GPIO_OSPEEDR_OSPEED2_Pos) /*!< 0x00000020 */
#define GPIO_OSPEEDR_OSPEED3_Pos (6U)
#define GPIO_OSPEEDR_OSPEED3_Msk (0x3UL << GPIO_OSPEEDR_OSPEED3_Pos) /*!< 0x000000C0 */
#define GPIO_OSPEEDR_OSPEED3 GPIO_OSPEEDR_OSPEED3_Msk
#define GPIO_OSPEEDR_OSPEED3_0 (0x1UL << GPIO_OSPEEDR_OSPEED3_Pos) /*!< 0x00000040 */
#define GPIO_OSPEEDR_OSPEED3_1 (0x2UL << GPIO_OSPEEDR_OSPEED3_Pos) /*!< 0x00000080 */
#define GPIO_OSPEEDR_OSPEED4_Pos (8U)
#define GPIO_OSPEEDR_OSPEED4_Msk (0x3UL << GPIO_OSPEEDR_OSPEED4_Pos) /*!< 0x00000300 */
#define GPIO_OSPEEDR_OSPEED4 GPIO_OSPEEDR_OSPEED4_Msk
#define GPIO_OSPEEDR_OSPEED4_0 (0x1UL << GPIO_OSPEEDR_OSPEED4_Pos) /*!< 0x00000100 */
#define GPIO_OSPEEDR_OSPEED4_1 (0x2UL << GPIO_OSPEEDR_OSPEED4_Pos) /*!< 0x00000200 */
#define GPIO_OSPEEDR_OSPEED5_Pos (10U)
#define GPIO_OSPEEDR_OSPEED5_Msk (0x3UL << GPIO_OSPEEDR_OSPEED5_Pos) /*!< 0x00000C00 */
#define GPIO_OSPEEDR_OSPEED5 GPIO_OSPEEDR_OSPEED5_Msk
#define GPIO_OSPEEDR_OSPEED5_0 (0x1UL << GPIO_OSPEEDR_OSPEED5_Pos) /*!< 0x00000400 */
#define GPIO_OSPEEDR_OSPEED5_1 (0x2UL << GPIO_OSPEEDR_OSPEED5_Pos) /*!< 0x00000800 */
#define GPIO_OSPEEDR_OSPEED6_Pos (12U)
#define GPIO_OSPEEDR_OSPEED6_Msk (0x3UL << GPIO_OSPEEDR_OSPEED6_Pos) /*!< 0x00003000 */
#define GPIO_OSPEEDR_OSPEED6 GPIO_OSPEEDR_OSPEED6_Msk
#define GPIO_OSPEEDR_OSPEED6_0 (0x1UL << GPIO_OSPEEDR_OSPEED6_Pos) /*!< 0x00001000 */
#define GPIO_OSPEEDR_OSPEED6_1 (0x2UL << GPIO_OSPEEDR_OSPEED6_Pos) /*!< 0x00002000 */
#define GPIO_OSPEEDR_OSPEED7_Pos (14U)
#define GPIO_OSPEEDR_OSPEED7_Msk (0x3UL << GPIO_OSPEEDR_OSPEED7_Pos) /*!< 0x0000C000 */
#define GPIO_OSPEEDR_OSPEED7 GPIO_OSPEEDR_OSPEED7_Msk
#define GPIO_OSPEEDR_OSPEED7_0 (0x1UL << GPIO_OSPEEDR_OSPEED7_Pos) /*!< 0x00004000 */
#define GPIO_OSPEEDR_OSPEED7_1 (0x2UL << GPIO_OSPEEDR_OSPEED7_Pos) /*!< 0x00008000 */
#define GPIO_OSPEEDR_OSPEED8_Pos (16U)
#define GPIO_OSPEEDR_OSPEED8_Msk (0x3UL << GPIO_OSPEEDR_OSPEED8_Pos) /*!< 0x00030000 */
#define GPIO_OSPEEDR_OSPEED8 GPIO_OSPEEDR_OSPEED8_Msk
#define GPIO_OSPEEDR_OSPEED8_0 (0x1UL << GPIO_OSPEEDR_OSPEED8_Pos) /*!< 0x00010000 */
#define GPIO_OSPEEDR_OSPEED8_1 (0x2UL << GPIO_OSPEEDR_OSPEED8_Pos) /*!< 0x00020000 */
#define GPIO_OSPEEDR_OSPEED9_Pos (18U)
#define GPIO_OSPEEDR_OSPEED9_Msk (0x3UL << GPIO_OSPEEDR_OSPEED9_Pos) /*!< 0x000C0000 */
#define GPIO_OSPEEDR_OSPEED9 GPIO_OSPEEDR_OSPEED9_Msk
#define GPIO_OSPEEDR_OSPEED9_0 (0x1UL << GPIO_OSPEEDR_OSPEED9_Pos) /*!< 0x00040000 */
#define GPIO_OSPEEDR_OSPEED9_1 (0x2UL << GPIO_OSPEEDR_OSPEED9_Pos) /*!< 0x00080000 */
#define GPIO_OSPEEDR_OSPEED10_Pos (20U)
#define GPIO_OSPEEDR_OSPEED10_Msk (0x3UL << GPIO_OSPEEDR_OSPEED10_Pos) /*!< 0x00300000 */
#define GPIO_OSPEEDR_OSPEED10 GPIO_OSPEEDR_OSPEED10_Msk
#define GPIO_OSPEEDR_OSPEED10_0 (0x1UL << GPIO_OSPEEDR_OSPEED10_Pos) /*!< 0x00100000 */
#define GPIO_OSPEEDR_OSPEED10_1 (0x2UL << GPIO_OSPEEDR_OSPEED10_Pos) /*!< 0x00200000 */
#define GPIO_OSPEEDR_OSPEED11_Pos (22U)
#define GPIO_OSPEEDR_OSPEED11_Msk (0x3UL << GPIO_OSPEEDR_OSPEED11_Pos) /*!< 0x00C00000 */
#define GPIO_OSPEEDR_OSPEED11 GPIO_OSPEEDR_OSPEED11_Msk
#define GPIO_OSPEEDR_OSPEED11_0 (0x1UL << GPIO_OSPEEDR_OSPEED11_Pos) /*!< 0x00400000 */
#define GPIO_OSPEEDR_OSPEED11_1 (0x2UL << GPIO_OSPEEDR_OSPEED11_Pos) /*!< 0x00800000 */
#define GPIO_OSPEEDR_OSPEED12_Pos (24U)
#define GPIO_OSPEEDR_OSPEED12_Msk (0x3UL << GPIO_OSPEEDR_OSPEED12_Pos) /*!< 0x03000000 */
#define GPIO_OSPEEDR_OSPEED12 GPIO_OSPEEDR_OSPEED12_Msk
#define GPIO_OSPEEDR_OSPEED12_0 (0x1UL << GPIO_OSPEEDR_OSPEED12_Pos) /*!< 0x01000000 */
#define GPIO_OSPEEDR_OSPEED12_1 (0x2UL << GPIO_OSPEEDR_OSPEED12_Pos) /*!< 0x02000000 */
#define GPIO_OSPEEDR_OSPEED13_Pos (26U)
#define GPIO_OSPEEDR_OSPEED13_Msk (0x3UL << GPIO_OSPEEDR_OSPEED13_Pos) /*!< 0x0C000000 */
#define GPIO_OSPEEDR_OSPEED13 GPIO_OSPEEDR_OSPEED13_Msk
#define GPIO_OSPEEDR_OSPEED13_0 (0x1UL << GPIO_OSPEEDR_OSPEED13_Pos) /*!< 0x04000000 */
#define GPIO_OSPEEDR_OSPEED13_1 (0x2UL << GPIO_OSPEEDR_OSPEED13_Pos) /*!< 0x08000000 */
#define GPIO_OSPEEDR_OSPEED14_Pos (28U)
#define GPIO_OSPEEDR_OSPEED14_Msk (0x3UL << GPIO_OSPEEDR_OSPEED14_Pos) /*!< 0x30000000 */
#define GPIO_OSPEEDR_OSPEED14 GPIO_OSPEEDR_OSPEED14_Msk
#define GPIO_OSPEEDR_OSPEED14_0 (0x1UL << GPIO_OSPEEDR_OSPEED14_Pos) /*!< 0x10000000 */
#define GPIO_OSPEEDR_OSPEED14_1 (0x2UL << GPIO_OSPEEDR_OSPEED14_Pos) /*!< 0x20000000 */
#define GPIO_OSPEEDR_OSPEED15_Pos (30U)
#define GPIO_OSPEEDR_OSPEED15_Msk (0x3UL << GPIO_OSPEEDR_OSPEED15_Pos) /*!< 0xC0000000 */
#define GPIO_OSPEEDR_OSPEED15 GPIO_OSPEEDR_OSPEED15_Msk
#define GPIO_OSPEEDR_OSPEED15_0 (0x1UL << GPIO_OSPEEDR_OSPEED15_Pos) /*!< 0x40000000 */
#define GPIO_OSPEEDR_OSPEED15_1 (0x2UL << GPIO_OSPEEDR_OSPEED15_Pos) /*!< 0x80000000 */

/*******************  Bit definition for SPI_CR1 register  ********************/
#define SPI_CR1_CPHA_Pos (0U)
#define SPI_CR1_CPHA_Msk (0x1UL << SPI_CR1_CPHA_Pos) /*!< 0x00000001 */
#define SPI_CR1_CPHA SPI_CR1_CPHA_Msk                /*!<Clock Phase      */
#define SPI_CR1_CPOL_Pos (1U)
#define SPI_CR1_CPOL_Msk (0x1UL << SPI_CR1_CPOL_Pos) /*!< 0x00000002 */
#define SPI_CR1_CPOL SPI_CR1_CPOL_Msk                /*!<Clock Polarity   */
#define SPI_CR1_MSTR_Pos (2U)
#define SPI_CR1_MSTR_Msk (0x1UL << SPI_CR1_MSTR_Pos) /*!< 0x00000004 */
#define SPI_CR1_MSTR SPI_CR1_MSTR_Msk                /*!<Master Selection */

#define SPI_CR1_BR_Pos (3U)
#define SPI_CR1_BR_Msk (0x7UL << SPI_CR1_BR_Pos) /*!< 0x00000038 */
#define SPI_CR1_BR SPI_CR1_BR_Msk                /*!<BR[2:0] bits (Baud Rate Control) */
#define SPI_CR1_BR_0 (0x1UL << SPI_CR1_BR_Pos)   /*!< 0x00000008 */
#define SPI_CR1_BR_1 (0x2UL << SPI_CR1_BR_Pos)   /*!< 0x00000010 */
#define SPI_CR1_BR_2 (0x4UL << SPI_CR1_BR_Pos)   /*!< 0x00000020 */

#define SPI_CR1_SPE_Pos (6U)
#define SPI_CR1_SPE_Msk (0x1UL << SPI_CR1_SPE_Pos) /*!< 0x00000040 */
#define SPI_CR1_SPE SPI_CR1_SPE_Msk                /*!<SPI Enable                          */
#define SPI_CR1_LSBFIRST_Pos (7U)
#define SPI_CR1_LSBFIRST_Msk (0x1UL << SPI_CR1_LSBFIRST_Pos) /*!< 0x00000080 */
#define SPI_CR1_LSBFIRST SPI_CR1_LSBFIRST_Msk                /*!<Frame Format                        */
#define SPI_CR1_SSI_Pos (8U)
#define SPI_CR1_SSI_Msk (0x1UL << SPI_CR1_SSI_Pos) /*!< 0x00000100 */
#define SPI_CR1_SSI SPI_CR1_SSI_Msk                /*!<Internal slave select               */
#define SPI_CR1_SSM_Pos (9U)
#define SPI_CR1_SSM_Msk (0x1UL << SPI_CR1_SSM_Pos) /*!< 0x00000200 */
#define SPI_CR1_SSM SPI_CR1_SSM_Msk                /*!<Software slave management           */
#define SPI_CR1_RXONLY_Pos (10U)
#define SPI_CR1_RXONLY_Msk (0x1UL << SPI_CR1_RXONLY_Pos) /*!< 0x00000400 */
#define SPI_CR1_RXONLY SPI_CR1_RXONLY_Msk                /*!<Receive only                        */
#define SPI_CR1_DFF_Pos (11U)
#define SPI_CR1_DFF_Msk (0x1UL << SPI_CR1_DFF_Pos) /*!< 0x00000800 */
#define SPI_CR1_DFF SPI_CR1_DFF_Msk                /*!<Data Frame Format                   */
#define SPI_CR1_CRCNEXT_Pos (12U)
#define SPI_CR1_CRCNEXT_Msk (0x1UL << SPI_CR1_CRCNEXT_Pos) /*!< 0x00001000 */
#define SPI_CR1_CRCNEXT SPI_CR1_CRCNEXT_Msk                /*!<Transmit CRC next                   */
#define SPI_CR1_CRCEN_Pos (13U)
#define SPI_CR1_CRCEN_Msk (0x1UL << SPI_CR1_CRCEN_Pos) /*!< 0x00002000 */
#define SPI_CR1_CRCEN SPI_CR1_CRCEN_Msk                /*!<Hardware CRC calculation enable     */
#define SPI_CR1_BIDIOE_Pos (14U)
#define SPI_CR1_BIDIOE_Msk (0x1UL << SPI_CR1_BIDIOE_Pos) /*!< 0x00004000 */
#define SPI_CR1_BIDIOE SPI_CR1_BIDIOE_Msk                /*!<Output enable in bidirectional mode */
#define SPI_CR1_BIDIMODE_Pos (15U)
#define SPI_CR1_BIDIMODE_Msk (0x1UL << SPI_CR1_BIDIMODE_Pos) /*!< 0x00008000 */
#define SPI_CR1_BIDIMODE SPI_CR1_BIDIMODE_Msk                /*!<Bidirectional data mode enable      */

/*******************  Bit definition for SPI_CR2 register  ********************/
#define SPI_CR2_RXDMAEN_Pos (0U)
#define SPI_CR2_RXDMAEN_Msk (0x1UL << SPI_CR2_RXDMAEN_Pos) /*!< 0x00000001 */
#define SPI_CR2_RXDMAEN SPI_CR2_RXDMAEN_Msk                /*!<Rx Buffer DMA Enable                 */
#define SPI_CR2_TXDMAEN_Pos (1U)
#define SPI_CR2_TXDMAEN_Msk (0x1UL << SPI_CR2_TXDMAEN_Pos) /*!< 0x00000002 */
#define SPI_CR2_TXDMAEN SPI_CR2_TXDMAEN_Msk                /*!<Tx Buffer DMA Enable                 */
#define SPI_CR2_SSOE_Pos (2U)
#define SPI_CR2_SSOE_Msk (0x1UL << SPI_CR2_SSOE_Pos) /*!< 0x00000004 */
#define SPI_CR2_SSOE SPI_CR2_SSOE_Msk                /*!<SS Output Enable                     */
#define SPI_CR2_FRF_Pos (4U)
#define SPI_CR2_FRF_Msk (0x1UL << SPI_CR2_FRF_Pos) /*!< 0x00000010 */
#define SPI_CR2_FRF SPI_CR2_FRF_Msk                /*!<Frame Format                         */
#define SPI_CR2_ERRIE_Pos (5U)
#define SPI_CR2_ERRIE_Msk (0x1UL << SPI_CR2_ERRIE_Pos) /*!< 0x00000020 */
#define SPI_CR2_ERRIE SPI_CR2_ERRIE_Msk                /*!<Error Interrupt Enable               */
#define SPI_CR2_RXNEIE_Pos (6U)
#define SPI_CR2_RXNEIE_Msk (0x1UL << SPI_CR2_RXNEIE_Pos) /*!< 0x00000040 */
#define SPI_CR2_RXNEIE SPI_CR2_RXNEIE_Msk                /*!<RX buffer Not Empty Interrupt Enable */
#define SPI_CR2_TXEIE_Pos (7U)
#define SPI_CR2_TXEIE_Msk (0x1UL << SPI_CR2_TXEIE_Pos) /*!< 0x00000080 */
#define SPI_CR2_TXEIE SPI_CR2_TXEIE_Msk                /*!<Tx buffer Empty Interrupt Enable     */

/********************  Bit definition for SPI_SR register  ********************/
#define SPI_SR_RXNE_Pos (0U)
#define SPI_SR_RXNE_Msk (0x1UL << SPI_SR_RXNE_Pos) /*!< 0x00000001 */
#define SPI_SR_RXNE SPI_SR_RXNE_Msk                /*!<Receive buffer Not Empty */
#define SPI_SR_TXE_Pos (1U)
#define SPI_SR_TXE_Msk (0x1UL << SPI_SR_TXE_Pos) /*!< 0x00000002 */
#define SPI_SR_TXE SPI_SR_TXE_Msk                /*!<Transmit buffer Empty    */
#define SPI_SR_CHSIDE_Pos (2U)
#define SPI_SR_CHSIDE_Msk (0x1UL << SPI_SR_CHSIDE_Pos) /*!< 0x00000004 */
#define SPI_SR_CHSIDE SPI_SR_CHSIDE_Msk                /*!<Channel side             */
#define SPI_SR_UDR_Pos (3U)
#define SPI_SR_UDR_Msk (0x1UL << SPI_SR_UDR_Pos) /*!< 0x00000008 */
#define SPI_SR_UDR SPI_SR_UDR_Msk                /*!<Underrun flag            */
#define SPI_SR_CRCERR_Pos (4U)
#define SPI_SR_CRCERR_Msk (0x1UL << SPI_SR_CRCERR_Pos) /*!< 0x00000010 */
#define SPI_SR_CRCERR SPI_SR_CRCERR_Msk                /*!<CRC Error flag           */
#define SPI_SR_MODF_Pos (5U)
#define SPI_SR_MODF_Msk (0x1UL << SPI_SR_MODF_Pos) /*!< 0x00000020 */
#define SPI_SR_MODF SPI_SR_MODF_Msk                /*!<Mode fault               */
#define SPI_SR_OVR_Pos (6U)
#define SPI_SR_OVR_Msk (0x1UL << SPI_SR_OVR_Pos) /*!< 0x00000040 */
#define SPI_SR_OVR SPI_SR_OVR_Msk                /*!<Overrun flag             */
#define SPI_SR_BSY_Pos (7U)
#define SPI_SR_BSY_Msk (0x1UL << SPI_SR_BSY_Pos) /*!< 0x00000080 */
#define SPI_SR_BSY SPI_SR_BSY_Msk                /*!<Busy flag                */
#define SPI_SR_FRE_Pos (8U)
#define SPI_SR_FRE_Msk (0x1UL << SPI_SR_FRE_Pos) /*!< 0x00000100 */
#define SPI_SR_FRE SPI_SR_FRE_Msk                /*!<Frame format error flag  */

/********************  Bit definition for SPI_DR register  ********************/
#define SPI_DR_DR_Pos (0U)
#define SPI_DR_DR_Msk (0xFFFFUL << SPI_DR_DR_Pos) /*!< 0x0000FFFF */
#define SPI_DR_DR SPI_DR_DR_Msk                   /*!<Data Register           */

/*******************  Bit definition for SPI_CRCPR register  ******************/
#define SPI_CRCPR_CRCPOLY_Pos (0U)
#define SPI_CRCPR_CRCPOLY_Msk (0xFFFFUL << SPI_CRCPR_CRCPOLY_Pos) /*!< 0x0000FFFF */
#define SPI_CRCPR_CRCPOLY SPI_CRCPR_CRCPOLY_Msk                   /*!<CRC polynomial register */

/******************  Bit definition for SPI_RXCRCR register  ******************/
#define SPI_RXCRCR_RXCRC_Pos (0U)
#define SPI_RXCRCR_RXCRC_Msk (0xFFFFUL << SPI_RXCRCR_RXCRC_Pos) /*!< 0x0000FFFF */
#define SPI_RXCRCR_RXCRC SPI_RXCRCR_RXCRC_Msk                   /*!<Rx CRC Register         */

/******************  Bit definition for SPI_TXCRCR register  ******************/
#define SPI_TXCRCR_TXCRC_Pos (0U)
#define SPI_TXCRCR_TXCRC_Msk (0xFFFFUL << SPI_TXCRCR_TXCRC_Pos) /*!< 0x0000FFFF */
#define SPI_TXCRCR_TXCRC SPI_TXCRCR_TXCRC_Msk                   /*!<Tx CRC Register         */

#endif /* DEVICE_SPI_H */
