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
 * @details Outlines the portable device register abstraction used to configure and interact with
 * hardware peripherals on Arm devices
 *
 *
 * @author  Jack Wilson
 * @date    January 26, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

#ifndef DEVICE_IRQ_H
#define DEVICE_IRQ_H

#include <stdint.h>

// lint -esym(528,*) There are definitions they may not be used
// lint -esym(750,*) There are definitions they may not be used
// lint -esym(751,*) There are definitions they may not be used
// lint -esym(754,*) There are definitions they may not be used
// lint -esym(749,*) There are definitions they may not be used

// NOTE:  This does not define every possible register or bit
// More can be added from the datasheet

typedef enum
{

    /******  Cortex-M0 Processor Exceptions Numbers
     *************************************************/
    Reset_IRQn          = -15, /*!< 1 Reset Vector, invoked on Power up and warm reset */
    NonMaskableInt_IRQn = -14, /*!< 2 Non Maskable Interrupt */
    HardFault_IRQn      = -13, /*!< 3 Cortex-M0+ Hard Fault Interrupt */
    SVCall_IRQn         = -5,  /*!< 11 Cortex-M0+ SV Call Interrupt */
    PendSV_IRQn         = -2,  /*!< 14 Cortex-M0+ Pend SV Interrupt */
    SysTick_IRQn        = -1,  /*!< 15 Cortex-M0+ System Tick Interrupt */
    /******  Interrupt Numbers
     **********************************************************************/
    PIO0_0_IRQn    = 0,  /*!< PIO0_0 Start Logic Interrupt */
    PIO0_1_IRQn    = 1,  /*!< PIO0_1 Start Logic Interrupt */
    PIO0_2_IRQn    = 2,  /*!< PIO0_2 Start Logic Interrupt */
    PIO0_3_IRQn    = 3,  /*!< PIO0_3 Start Logic Interrupt */
    PIO0_4_IRQn    = 4,  /*!< PIO0_4 Start Logic Interrupt */
    PIO0_5_IRQn    = 5,  /*!< PIO0_5 Start Logic Interrupt */
    PIO0_6_IRQn    = 6,  /*!< PIO0_6 Start Logic Interrupt */
    PIO0_7_IRQn    = 7,  /*!< PIO0_7 Start Logic Interrupt */
    PIO0_8_IRQn    = 8,  /*!< PIO0_8 Start Logic Interrupt */
    PIO0_9_IRQn    = 9,  /*!< PIO0_9 Start Logic Interrupt */
    PIO0_10_IRQn   = 10, /*!< PIO0_10 Start Logic Interrupt */
    RFFIELD_IRQn   = 11, /*!< NFC Access Start Logic Interrupt */
    RTCPWREQ_IRQn  = 12, /*!< RTC Wakeup Request Start Logic Interrupt */
    NFC_IRQn       = 13, /*!< NFC Read/Write Interrupt */
    RTC_IRQn       = 14, /*!< RTC Wakeup Interrupt */
    I2C0_IRQn      = 15, /*!< I2C0 Interrupt */
    CT16B0_IRQn    = 16, /*!< 16-bit Timer 0 Interrupt */
    PMUFLD_IRQn    = 17, /*!< RF Power Detection Interrupt */
    CT32B0_IRQn    = 18, /*!< 32-bit Timer 0 Interrupt */
    PMUBOD_IRQn    = 19, /*!< Brown Out Detection Interrupt */
    SSP0_IRQn      = 20, /*!< SSP0 Interrupt */
    TSEN_IRQn      = 21, /*!< Temperature Sensor Interrupt */
    C2D_IRQn       = 22, /*!< Capacitance-to-Digital converter Interrupt */
    Reserved1_IRQn = 23, /*!< reserved */
    I2D_IRQn       = 24, /*!< Current-to-Digital converter Interrupt */
    ADCDAC_IRQn    = 25, /*!< Analog-to-Digital/Digital-to-Analog converter Interrupt */
    WDT_IRQn       = 26, /*!< Watchdog Timer Interrupt */
    FLASH_IRQn     = 27, /*!< FLASH memory Interrupt */
    EEPROM_IRQn    = 28, /*!< EEPROM memory Interrupt */
    Reserved2_IRQn = 29, /*!< reserved */
    Reserved3_IRQn = 30, /*!< reserved */
    PIO0_IRQn      = 31, /*!< GPIO Port 0 Interrupt */
} interrupt_id_t;

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
    volatile uint32_t ISER[1]; /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register           */
    uint32_t          RESERVED0[31];
    volatile uint32_t ICER[1]; /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register          */
    uint32_t          RSERVED1[31];
    volatile uint32_t ISPR[1]; /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register           */
    uint32_t          RESERVED2[31];
    volatile uint32_t ICPR[1]; /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register         */
    uint32_t          RESERVED3[31];
    uint32_t          RESERVED4[64];
    volatile uint32_t IP[8]; /*!< Offset: 0x300 (R/W)  Interrupt Priority Register              */
} NVIC_Type;

#define NVIC ((NVIC_Type *)NVIC_BASE) /*!< NVIC configuration struct */

static inline void NVIC_EnableIRQ(interrupt_id_t IRQn)
{
    NVIC->ISER[0] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

static inline void NVIC_ClearPendingIRQ(interrupt_id_t IRQn)
{
    NVIC->ICPR[0] = (1 << ((uint32_t)(IRQn) & 0x1F)); /* Clear pending interrupt */
}

static inline void NVIC_DisableIRQ(interrupt_id_t IRQn)
{
    NVIC->ICER[0] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

static inline void NVIC_SetPendingIRQ(interrupt_id_t IRQn)
{
    NVIC->ISPR[0] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

__attribute__((always_inline)) static inline void __enable_irq(void)
{
    asm volatile("cpsie i");
}
#endif /* DEVICE_IRQ_H */