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
 * @file device_irq.h
 * @brief  Defines registers used for Interrupts and NVIC on Arm devices
 * @details Outlines the portable device register abstraction used to configure and interact with hardware peripherals
 *          on Arm devices
 *
 *
 * @author  Jack Wilson
 * @date    January 23, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

#ifndef DEVICE_IRQ_H
#define DEVICE_IRQ_H

#include "device_regs.h"
#include <stdint.h>

// lint -esym(528,*) There are definitions they may not be used
// lint -esym(750,*) There are definitions they may not be used
// lint -esym(751,*) There are definitions they may not be used
// lint -esym(754,*) There are definitions they may not be used
// lint -esym(749,*) There are definitions they may not be used

// NOTE:  This does not define every possible register or bit
// More can be added from the datasheet

// Reset Clock Control
#define REG_EXTI_BASE (REG_APB2PERIPH_BASE + 0x3C00U)
/**
 * @brief  Sysconfig memory definition
 */
typedef struct
{
    volatile uint32_t imr;   /*!< EXTI interrupt mask register,               Address offset: 0x00 */
    volatile uint32_t emr;   /*!< EXTI event mask register,                   Address offset: 0x04 */
    volatile uint32_t rtsr;  /*!< EXTI rising trigger selection register,     Address offset: 0x08 */
    volatile uint32_t ftsr;  /*!< EXTI falling trigger selection register,    Address offset: 0x0C */
    volatile uint32_t swier; /*!< EXTI software interrupt event register,     Address offset: 0x10 */
    volatile uint32_t pr;    /*!< EXTI pending register,                      Address offset: 0x14 */

} reg_exti_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_exti_t *p_device_exti = (reg_exti_t *const)(REG_EXTI_BASE);
#pragma GCC diagnostic pop

typedef enum
{
    /******  Cortex-M4 Processor Exceptions Numbers
     ****************************************************************/
    NONMASKABLEINT_IRQN   = -14,  /*!< 2 Non Maskable Interrupt                                          */
    MEMORYMANAGEMENT_IRQN = -12,  /*!< 4 Cortex-M4 Memory Management Interrupt                           */
    BUSFAULT_IRQN         = -11,  /*!< 5 Cortex-M4 Bus Fault Interrupt                                   */
    USAGEFAULT_IRQN       = -10,  /*!< 6 Cortex-M4 Usage Fault Interrupt                                 */
    SVCALL_IRQN           = -5,   /*!< 11 Cortex-M4 SV Call Interrupt                                    */
    DEBUGMONITOR_IRQN     = -4,   /*!< 12 Cortex-M4 Debug Monitor Interrupt                              */
    PENDSV_IRQN           = -2,   /*!< 14 Cortex-M4 Pend SV Interrupt                                    */
    SYSTICK_IRQN          = -1,   /*!< 15 Cortex-M4 System Tick Interrupt                                */
                                  /******  STM32 specific Interrupt Numbers
                                   **********************************************************************/
    WWDG_IRQN               = 0,  /*!< Window WatchDog Interrupt                                         */
    PVD_IRQN                = 1,  /*!< PVD through EXTI Line detection Interrupt                         */
    TAMP_STAMP_IRQN         = 2,  /*!< Tamper and TimeStamp interrupts through the EXTI line             */
    RTC_WKUP_IRQN           = 3,  /*!< RTC Wakeup interrupt through the EXTI line                        */
    FLASH_IRQN              = 4,  /*!< FLASH global Interrupt                                            */
    RCC_IRQN                = 5,  /*!< RCC global Interrupt                                              */
    EXTI0_IRQN              = 6,  /*!< EXTI Line0 Interrupt                                              */
    EXTI1_IRQN              = 7,  /*!< EXTI Line1 Interrupt                                              */
    EXTI2_IRQN              = 8,  /*!< EXTI Line2 Interrupt                                              */
    EXTI3_IRQN              = 9,  /*!< EXTI Line3 Interrupt                                              */
    EXTI4_IRQN              = 10, /*!< EXTI Line4 Interrupt                                              */
    DMA1_Stream0_IRQN       = 11, /*!< DMA1 Stream 0 global Interrupt                                    */
    DMA1_Stream1_IRQN       = 12, /*!< DMA1 Stream 1 global Interrupt                                    */
    DMA1_Stream2_IRQN       = 13, /*!< DMA1 Stream 2 global Interrupt                                    */
    DMA1_Stream3_IRQN       = 14, /*!< DMA1 Stream 3 global Interrupt                                    */
    DMA1_Stream4_IRQN       = 15, /*!< DMA1 Stream 4 global Interrupt                                    */
    DMA1_Stream5_IRQN       = 16, /*!< DMA1 Stream 5 global Interrupt                                    */
    DMA1_Stream6_IRQN       = 17, /*!< DMA1 Stream 6 global Interrupt                                    */
    ADC_IRQN                = 18, /*!< ADC1, ADC2 and ADC3 global Interrupts                             */
    EXTI9_5_IRQN            = 23, /*!< External Line[9:5] Interrupts                                     */
    TIM1_BRK_TIM9_IRQN      = 24, /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
    TIM1_UP_TIM10_IRQN      = 25, /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
    TIM1_TRG_COM_TIM11_IRQN = 26, /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
    TIM1_CC_IRQN            = 27, /*!< TIM1 Capture Compare Interrupt                                    */
    TIM2_IRQN               = 28, /*!< TIM2 global Interrupt                                             */
    TIM3_IRQN               = 29, /*!< TIM3 global Interrupt                                             */
    TIM4_IRQN               = 30, /*!< TIM4 global Interrupt                                             */
    I2C1_EV_IRQN            = 31, /*!< I2C1 Event Interrupt                                              */
    I2C1_ER_IRQN            = 32, /*!< I2C1 Error Interrupt                                              */
    I2C2_EV_IRQN            = 33, /*!< I2C2 Event Interrupt                                              */
    I2C2_ER_IRQN            = 34, /*!< I2C2 Error Interrupt                                              */
    SPI1_IRQN               = 35, /*!< SPI1 global Interrupt                                             */
    SPI2_IRQN               = 36, /*!< SPI2 global Interrupt                                             */
    USART1_IRQN             = 37, /*!< USART1 global Interrupt                                           */
    USART2_IRQN             = 38, /*!< USART2 global Interrupt                                           */
    EXTI15_10_IRQN          = 40, /*!< External Line[15:10] Interrupts                                   */
    RTC_Alarm_IRQN          = 41, /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
    OTG_FS_WKUP_IRQN        = 42, /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
    DMA1_Stream7_IRQN       = 47, /*!< DMA1 Stream7 Interrupt                                            */
    SDIO_IRQN               = 49, /*!< SDIO global Interrupt                                             */
    TIM5_IRQN               = 50, /*!< TIM5 global Interrupt                                             */
    SPI3_IRQN               = 51, /*!< SPI3 global Interrupt                                             */
    DMA2_Stream0_IRQN       = 56, /*!< DMA2 Stream 0 global Interrupt                                    */
    DMA2_Stream1_IRQN       = 57, /*!< DMA2 Stream 1 global Interrupt                                    */
    DMA2_Stream2_IRQN       = 58, /*!< DMA2 Stream 2 global Interrupt                                    */
    DMA2_Stream3_IRQN       = 59, /*!< DMA2 Stream 3 global Interrupt                                    */
    DMA2_Stream4_IRQN       = 60, /*!< DMA2 Stream 4 global Interrupt                                    */
    OTG_FS_IRQN             = 67, /*!< USB OTG FS global Interrupt                                       */
    DMA2_Stream5_IRQN       = 68, /*!< DMA2 Stream 5 global interrupt                                    */
    DMA2_Stream6_IRQN       = 69, /*!< DMA2 Stream 6 global interrupt                                    */
    DMA2_Stream7_IRQN       = 70, /*!< DMA2 Stream 7 global interrupt                                    */
    USART6_IRQN             = 71, /*!< USART6 global interrupt                                           */
    I2C3_EV_IRQN            = 72, /*!< I2C3 event interrupt                                              */
    I2C3_ER_IRQN            = 73, /*!< I2C3 error interrupt                                              */
    FPU_IRQN                = 81, /*!< FPU global interrupt                                              */
    SPI4_IRQN               = 84, /*!< SPI4 global Interrupt                                             */
    SPI5_IRQN               = 85  /*!< SPI5 global Interrupt                                              */
} interrupt_id_t;

// Interrupt mask register(EXTI_IMR)
// Bits 31 : 23 Reserved, must be kept at reset value.
//  Bits 22 : 0 MRx : Interrupt mask on line x
//   0 : Interrupt request from line x is masked
//   1 : Interrupt request from line x is not masked

#define EXTI_IMR_MR0 (1U << 0U)   /*!< Interrupt Mask on line 0 */
#define EXTI_IMR_MR1 (1U << 1U)   /*!< Interrupt Mask on line 1 */
#define EXTI_IMR_MR2 (1U << 2U)   /*!< Interrupt Mask on line 2 */
#define EXTI_IMR_MR3 (1U << 3U)   /*!< Interrupt Mask on line 3 */
#define EXTI_IMR_MR4 (1U << 4U)   /*!< Interrupt Mask on line 4 */
#define EXTI_IMR_MR5 (1U << 5U)   /*!< Interrupt Mask on line 5 */
#define EXTI_IMR_MR6 (1U << 6U)   /*!< Interrupt Mask on line 6 */
#define EXTI_IMR_MR7 (1U << 7U)   /*!< Interrupt Mask on line 7 */
#define EXTI_IMR_MR8 (1U << 8U)   /*!< Interrupt Mask on line 8 */
#define EXTI_IMR_MR9 (1U << 9U)   /*!< Interrupt Mask on line 9 */
#define EXTI_IMR_MR10 (1U << 10U) /*!< Interrupt Mask on line 10 */
#define EXTI_IMR_MR11 (1U << 11U) /*!< Interrupt Mask on line 11 */
#define EXTI_IMR_MR12 (1U << 12U) /*!< Interrupt Mask on line 12 */
#define EXTI_IMR_MR13 (1U << 13U) /*!< Interrupt Mask on line 13 */
#define EXTI_IMR_MR14 (1U << 14U) /*!< Interrupt Mask on line 14 */
#define EXTI_IMR_MR15 (1U << 15U) /*!< Interrupt Mask on line 15 */
#define EXTI_IMR_MR16 (1U << 16U) /*!< Interrupt Mask on line 16 */
#define EXTI_IMR_MR17 (1U << 17U) /*!< Interrupt Mask on line 17 */
#define EXTI_IMR_MR18 (1U << 18U) /*!< Interrupt Mask on line 18 */
// #define EXTI_IMR_MR19 (1U << 19U) /*!< Interrupt Mask on line 19 */
// #define EXTI_IMR_MR20 (1U << 20U) /*!< Interrupt Mask on line 20 */
#define EXTI_IMR_MR21 (1U << 21U) /*!< Interrupt Mask on line 21 */
#define EXTI_IMR_MR22 (1U << 22U) /*!< Interrupt Mask on line 22 */

// Event mask register (EXTI_EMR)
// Address offset : 0x04
// Reset value : 0x0000 0000

#define EXTI_EMR_MR0 (1U << 0U)   /*!< Event Mask on line 0 */
#define EXTI_EMR_MR1 (1U << 1U)   /*!< Event Mask on line 1 */
#define EXTI_EMR_MR2 (1U << 2U)   /*!< Event Mask on line 2 */
#define EXTI_EMR_MR3 (1U << 3U)   /*!< Event Mask on line 3 */
#define EXTI_EMR_MR4 (1U << 4U)   /*!< Event Mask on line 4 */
#define EXTI_EMR_MR5 (1U << 5U)   /*!< Event Mask on line 5 */
#define EXTI_EMR_MR6 (1U << 6U)   /*!< Event Mask on line 6 */
#define EXTI_EMR_MR7 (1U << 7U)   /*!< Event Mask on line 7 */
#define EXTI_EMR_MR8 (1U << 8U)   /*!< Event Mask on line 8 */
#define EXTI_EMR_MR9 (1U << 9U)   /*!< Event Mask on line 9 */
#define EXTI_EMR_MR10 (1U << 10U) /*!< Event Mask on line 10 */
#define EXTI_EMR_MR11 (1U << 11U) /*!< Event Mask on line 11 */
#define EXTI_EMR_MR12 (1U << 12U) /*!< Event Mask on line 12 */
#define EXTI_EMR_MR13 (1U << 13U) /*!< Event Mask on line 13 */
#define EXTI_EMR_MR14 (1U << 14U) /*!< Event Mask on line 14 */
#define EXTI_EMR_MR15 (1U << 15U) /*!< Event Mask on line 15 */
#define EXTI_EMR_MR16 (1U << 16U) /*!< Event Mask on line 16 */
#define EXTI_EMR_MR17 (1U << 17U) /*!< Event Mask on line 17 */
#define EXTI_EMR_MR18 (1U << 18U) /*!< Event Mask on line 18 */
// #define EXTI_EMR_MR19 (1U << 19U) /*!< Event Mask on line 19 */
// #define EXTI_EMR_MR20 (1U << 20U) /*!< Event Mask on line 20 */
#define EXTI_EMR_MR21 (1U << 21U) /*!< Event Mask on line 21 */
#define EXTI_EMR_MR22 (1U << 22U) /*!< Event Mask on line 22 */

// Rising trigger selection register (EXTI_RTSR)
// Address offset : 0x08
// Reset value : 0x0000 0000
// Bits 31:23 Reserved, must be kept at reset value.
// Bits 22:0 TRx: Rising trigger event configuration bit of line x
//   0: Rising trigger disabled (for Event and Interrupt) for input line
// 1: Rising trigger enabled (for Event and Interrupt) for input line
// Note:
// The external wakeup lines are edge triggered, no glitch must be generated on these lines.
// If a rising edge occurs on the external interrupt line while writing to the
// EXTI_RTSR register,the pending bit is be set.
// Rising and falling edge triggers can be set for the same interrupt line. In this configuration,
// both generate a trigger condition.

#define EXTI_RTSR_TR0 (1U << 0U)   /*!< Rising trigger event configuration bit of line 0 */
#define EXTI_RTSR_TR1 (1U << 1U)   /*!< Rising trigger event configuration bit of line 1 */
#define EXTI_RTSR_TR2 (1U << 2U)   /*!< Rising trigger event configuration bit of line 2 */
#define EXTI_RTSR_TR3 (1U << 3U)   /*!< Rising trigger event configuration bit of line 3 */
#define EXTI_RTSR_TR4 (1U << 4U)   /*!< Rising trigger event configuration bit of line 4 */
#define EXTI_RTSR_TR5 (1U << 5U)   /*!< Rising trigger event configuration bit of line 5 */
#define EXTI_RTSR_TR6 (1U << 6U)   /*!< Rising trigger event configuration bit of line 6 */
#define EXTI_RTSR_TR7 (1U << 7U)   /*!< Rising trigger event configuration bit of line 7 */
#define EXTI_RTSR_TR8 (1U << 8U)   /*!< Rising trigger event configuration bit of line 8 */
#define EXTI_RTSR_TR9 (1U << 9U)   /*!< Rising trigger event configuration bit of line 9 */
#define EXTI_RTSR_TR10 (1U << 10U) /*!< Rising trigger event configuration bit of line 10 */
#define EXTI_RTSR_TR11 (1U << 11U) /*!< Rising trigger event configuration bit of line 11 */
#define EXTI_RTSR_TR12 (1U << 12U) /*!< Rising trigger event configuration bit of line 12 */
#define EXTI_RTSR_TR13 (1U << 13U) /*!< Rising trigger event configuration bit of line 13 */
#define EXTI_RTSR_TR14 (1U << 14U) /*!< Rising trigger event configuration bit of line 14 */
#define EXTI_RTSR_TR15 (1U << 15U) /*!< Rising trigger event configuration bit of line 15 */
#define EXTI_RTSR_TR16 (1U << 16U) /*!< Rising trigger event configuration bit of line 16 */
#define EXTI_RTSR_TR17 (1U << 17U) /*!< Rising trigger event configuration bit of line 17 */
#define EXTI_RTSR_TR18 (1U << 18U) /*!< Rising trigger event configuration bit of line 18 */
// #define EXTI_RTSR_TR19 (1U << 19U) /*!< Rising trigger event configuration bit of line 19 */
// #define EXTI_RTSR_TR20 (1U << 20U) /*!< Rising trigger event configuration bit of line 20 */
#define EXTI_RTSR_TR21 (1U << 21U) /*!< Rising trigger event configuration bit of line 21 */
#define EXTI_RTSR_TR22 (1U << 22U) /*!< Rising trigger event configuration bit of line 22 */

// Falling trigger selection register (EXTI_FTSR)
// Address offset : 0x0C
// Reset value : 0x0000 0000
// Note:
// The external wakeup lines are edge triggered, no glitch must be generated on these lines.
// If a falling edge occurs on the external interrupt line while writing to the EXTI_FTSR register,
// the pending bit is not set.
// Rising and falling edge triggers can be set for the same interrupt line. In this configuration,
// both generate a trigger condition.

#define EXTI_FTSR_TR0 (1U << 0U)   /*!< Falling trigger event configuration bit of line 0 */
#define EXTI_FTSR_TR1 (1U << 1U)   /*!< Falling trigger event configuration bit of line 1 */
#define EXTI_FTSR_TR2 (1U << 2U)   /*!< Falling trigger event configuration bit of line 2 */
#define EXTI_FTSR_TR3 (1U << 3U)   /*!< Falling trigger event configuration bit of line 3 */
#define EXTI_FTSR_TR4 (1U << 4U)   /*!< Falling trigger event configuration bit of line 4 */
#define EXTI_FTSR_TR5 (1U << 5U)   /*!< Falling trigger event configuration bit of line 5 */
#define EXTI_FTSR_TR6 (1U << 6U)   /*!< Falling trigger event configuration bit of line 6 */
#define EXTI_FTSR_TR7 (1U << 7U)   /*!< Falling trigger event configuration bit of line 7 */
#define EXTI_FTSR_TR8 (1U << 8U)   /*!< Falling trigger event configuration bit of line 8 */
#define EXTI_FTSR_TR9 (1U << 9U)   /*!< Falling trigger event configuration bit of line 9 */
#define EXTI_FTSR_TR10 (1U << 10U) /*!< Falling trigger event configuration bit of line 10 */
#define EXTI_FTSR_TR11 (1U << 11U) /*!< Falling trigger event configuration bit of line 11 */
#define EXTI_FTSR_TR12 (1U << 12U) /*!< Falling trigger event configuration bit of line 12 */
#define EXTI_FTSR_TR13 (1U << 13U) /*!< Falling trigger event configuration bit of line 13 */
#define EXTI_FTSR_TR14 (1U << 14U) /*!< Falling trigger event configuration bit of line 14 */
#define EXTI_FTSR_TR15 (1U << 15U) /*!< Falling trigger event configuration bit of line 15 */
#define EXTI_FTSR_TR16 (1U << 16U) /*!< Falling trigger event configuration bit of line 16 */
#define EXTI_FTSR_TR17 (1U << 17U) /*!< Falling trigger event configuration bit of line 17 */
#define EXTI_FTSR_TR18 (1U << 18U) /*!< Falling trigger event configuration bit of line 18 */
// #define EXTI_FTSR_TR19 (1U << 19U) /*!< Falling trigger event configuration bit of line 19 */
// #define EXTI_FTSR_TR20 (1U << 20U) /*!< Falling trigger event configuration bit of line 20 */
#define EXTI_FTSR_TR21 (1U << 21U) /*!< Falling trigger event configuration bit of line 21 */
#define EXTI_FTSR_TR22 (1U << 22U) /*!< Falling trigger event configuration bit of line 22 */

// Software interrupt event register (EXTI_SWIER)
// Address offset : 0x10
// Reset value : 0x0000 0000
// Bits 31 : 23 Reserved, must be kept at reset value.
// Bits 22 : 0 SWIERx: Software Interrupt on line x
//   If interrupt are enabled on line x in the EXTI_IMR register,
//   writing '1' to SWIERx bit when it is set at '0' sets the corresponding pending bit in
//   the EXTI_PR register, thus resulting in an interrupt request generation.
//   This bit is cleared by clearing the corresponding bit in EXTI_PR(by writing a 1 to the bit)

#define EXTI_SWIER_SWIER0 (1U << 0U)   /*!< Software Interrupt on line 0 */
#define EXTI_SWIER_SWIER1 (1U << 1U)   /*!< Software Interrupt on line 1 */
#define EXTI_SWIER_SWIER2 (1U << 2U)   /*!< Software Interrupt on line 2 */
#define EXTI_SWIER_SWIER3 (1U << 3U)   /*!< Software Interrupt on line 3 */
#define EXTI_SWIER_SWIER4 (1U << 4U)   /*!< Software Interrupt on line 4 */
#define EXTI_SWIER_SWIER5 (1U << 5U)   /*!< Software Interrupt on line 5 */
#define EXTI_SWIER_SWIER6 (1U << 6U)   /*!< Software Interrupt on line 6 */
#define EXTI_SWIER_SWIER7 (1U << 7U)   /*!< Software Interrupt on line 7 */
#define EXTI_SWIER_SWIER8 (1U << 8U)   /*!< Software Interrupt on line 8 */
#define EXTI_SWIER_SWIER9 (1U << 9U)   /*!< Software Interrupt on line 9 */
#define EXTI_SWIER_SWIER10 (1U << 10U) /*!< Software Interrupt on line 10 */
#define EXTI_SWIER_SWIER11 (1U << 11U) /*!< Software Interrupt on line 11 */
#define EXTI_SWIER_SWIER12 (1U << 12U) /*!< Software Interrupt on line 12 */
#define EXTI_SWIER_SWIER13 (1U << 13U) /*!< Software Interrupt on line 13 */
#define EXTI_SWIER_SWIER14 (1U << 14U) /*!< Software Interrupt on line 14 */
#define EXTI_SWIER_SWIER15 (1U << 15U) /*!< Software Interrupt on line 15 */
#define EXTI_SWIER_SWIER16 (1U << 16U) /*!< Software Interrupt on line 16 */
#define EXTI_SWIER_SWIER17 (1U << 17U) /*!< Software Interrupt on line 17 */
#define EXTI_SWIER_SWIER18 (1U << 18U) /*!< Software Interrupt on line 18 */
// #define EXTI_SWIER_SWIER19 (1U << 19U) /*!< Software Interrupt on line 19 */
// #define EXTI_SWIER_SWIER20 (1U << 20U) /*!< Software Interrupt on line 20 */
#define EXTI_SWIER_SWIER21 (1U << 21U) /*!< Software Interrupt on line 21 */
#define EXTI_SWIER_SWIER22 (1U << 22U) /*!< Software Interrupt on line 22 */

// Pending register(EXTI_PR)
// Address offset : 0x14
// Reset value : undefined
// Bits 31 : 23 Reserved, must be kept at reset value.
// Bits 22 : 0 PRx : Pending bit
//   0 : No trigger request occurred
//   1 : selected trigger request occurred This bit is set when the selected edge
//       event arrives on the external interrupt line.
//   This bit is cleared by programming it to ‘1’.

#define EXTI_PR_PR0 (1U << 0U)   /*!< Pending bit of line 0 */
#define EXTI_PR_PR1 (1U << 1U)   /*!< Pending bit of line 1 */
#define EXTI_PR_PR2 (1U << 2U)   /*!< Pending bit of line 2 */
#define EXTI_PR_PR3 (1U << 3U)   /*!< Pending bit of line 3 */
#define EXTI_PR_PR4 (1U << 4U)   /*!< Pending bit of line 4 */
#define EXTI_PR_PR5 (1U << 5U)   /*!< Pending bit of line 5 */
#define EXTI_PR_PR6 (1U << 6U)   /*!< Pending bit of line 6 */
#define EXTI_PR_PR7 (1U << 7U)   /*!< Pending bit of line 7 */
#define EXTI_PR_PR8 (1U << 8U)   /*!< Pending bit of line 8 */
#define EXTI_PR_PR9 (1U << 9U)   /*!< Pending bit of line 9 */
#define EXTI_PR_PR10 (1U << 10U) /*!< Pending bit of line 10 */
#define EXTI_PR_PR11 (1U << 11U) /*!< Pending bit of line 11 */
#define EXTI_PR_PR12 (1U << 12U) /*!< Pending bit of line 12 */
#define EXTI_PR_PR13 (1U << 13U) /*!< Pending bit of line 13 */
#define EXTI_PR_PR14 (1U << 14U) /*!< Pending bit of line 14 */
#define EXTI_PR_PR15 (1U << 15U) /*!< Pending bit of line 15 */
#define EXTI_PR_PR16 (1U << 16U) /*!< Pending bit of line 16 */
#define EXTI_PR_PR17 (1U << 17U) /*!< Pending bit of line 17 */
#define EXTI_PR_PR18 (1U << 18U) /*!< Pending bit of line 18 */
// #define EXTI_PR_PR19 (1U << 19U) /*!< Pending bit of line 19 */
// #define EXTI_PR_PR20 (1U << 20U) /*!< Pending bit of line 20 */
#define EXTI_PR_PR21 (1U << 21U) /*!< Pending bit of line 21 */
#define EXTI_PR_PR22 (1U << 22U) /*!< Pending bit of line 22 */

#define __COMPILER_BARRIER() asm volatile("" ::: "memory")

// from core_cm4.h
/* Memory mapping of Core Hardware */
#define SCS_BASE (0xE000E000UL)            /*!< System Control Space Base Address */
#define ITM_BASE (0xE0000000UL)            /*!< ITM Base Address */
#define DWT_BASE (0xE0001000UL)            /*!< DWT Base Address */
#define TPI_BASE (0xE0040000UL)            /*!< TPI Base Address */
#define CoreDebug_BASE (0xE000EDF0UL)      /*!< Core Debug Base Address */
#define SysTick_BASE (SCS_BASE + 0x0010UL) /*!< SysTick Base Address */
#define NVIC_BASE (SCS_BASE + 0x0100UL)    /*!< NVIC Base Address */
#define SCB_BASE (SCS_BASE + 0x0D00UL)     /*!< System Control Block Base Address */

#define SCnSCB ((SCnSCB_Type *)SCS_BASE)             /*!< System control Register not in SCB */
#define SCB ((SCB_Type *)SCB_BASE)                   /*!< SCB configuration struct */
#define SysTick ((SysTick_Type *)SysTick_BASE)       /*!< SysTick configuration struct */
#define NVIC ((NVIC_Type *)NVIC_BASE)                /*!< NVIC configuration struct */
#define ITM ((ITM_Type *)ITM_BASE)                   /*!< ITM configuration struct */
#define DWT ((DWT_Type *)DWT_BASE)                   /*!< DWT configuration struct */
#define TPI ((TPI_Type *)TPI_BASE)                   /*!< TPI configuration struct */
#define CoreDebug ((CoreDebug_Type *)CoreDebug_BASE) /*!< Core Debug configuration struct */

/**
  \brief Structure type to access the Nested Vectored Interrupt Controller(NVIC).
*/

typedef struct
{
    volatile uint32_t ISER[8U]; /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register */
    uint32_t          RESERVED0[24U];
    volatile uint32_t ICER[8U]; /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
    uint32_t          RSERVED1[24U];
    volatile uint32_t ISPR[8U]; /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register */
    uint32_t          RESERVED2[24U];
    volatile uint32_t ICPR[8U]; /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
    uint32_t          RESERVED3[24U];
    volatile uint32_t IABR[8U]; /*!< Offset: 0x200 (R/W)  Interrupt Active bit Register */
    uint32_t          RESERVED4[56U];
    volatile uint8_t  IP[240U]; /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
    uint32_t          RESERVED5[644U];
    volatile uint32_t STIR; /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register */
} NVIC_Type;
#define NVIC ((NVIC_Type *)NVIC_BASE) /*!< NVIC configuration struct */

static inline void NVIC_EnableIRQ(interrupt_id_t IRQn)
{

    if ((int32_t)(IRQn) >= 0)

    {
        __COMPILER_BARRIER();

        NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));

        __COMPILER_BARRIER();
    }
}

#endif /* DEVICE_IRQ_H */