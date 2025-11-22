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
 * @file      device_ct16b.h
 * @defgroup  device_ct16b 16-bit Timer/Counter (CT16B)
 * @author    Jack Wilson
 * @brief     Defines and Macros for the 16-bit Timer/Counter (CT16B)
 *
 * @details   This describes the CT16B 16-bit timer, which is identical on all members of the
 * NHS31xx family.
 *
 * The CT16B is configured as follows:
 * • Pins:
 *    The match outputs of the timer can be routed to I/O pins by setting the I/O pin function
 *    in the IOCON register (see Table 254).
 * • Clock:
 *    The PCLK clock clocks the CT16B block. This clock is derived from the system clock
 *    (see Figure 7). The clock can be enabled by setting bit 8 in the SYSAHBCLKCTRL
 *    register (see Table 17).
 * • Features:
 *    One 16-bit timer with a programmable 16-bit prescaler.
 * • Timer operation
 *    Four 16-bit match registers that allow:– Continuous operation with optional interrupt 
 *    generation on match– Stop timer on match with optional interrupt generation– 
 *    Reset timer on match with optional interrupt generation
 *    Up to two CT16B external outputs corresponding to the match registers with the
 *    following capabilities:– Set LOW on match– Set HIGH on match– Toggle on match– Do nothing on match
 *    Up to two match registers can be configured as PWM allowing the use of up to two
 *    match outputs as single edge controlled PWM outputs.
 * 
 * @mod
 * @moditem{V1.0.x, Nov-12-2025, Jack Wilson, Initial Release, jackkc7vlo@gmail.com }
 * @modend
 *
*/

#include <stdint.h>

#ifndef DEVICE_CT16B_H
#define DEVICE_CT16B_H
//lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
//lint -restore
//lint -esym(751,ct16b_*) There are definitions they may not be used
//lint -esym(754,ct16b_*) There are definitions they may not be used

// 16-bit Timer/Counter 0 Base Address
#define REG_CT16B0_BASE (0x4000C000U)   //!< 16-bit Timer/Counter 0 Base Address

/**
 * @brief 16-bit Timer/Counter registers structure definition
 */
typedef struct
{
    volatile uint32_t ir;        //!< Interrupt Register (IR)                          offset 0x00
    volatile uint32_t tcr;       //!< Timer Control Register (TCR)                     offset 0x04
    volatile uint32_t tc;        //!< Timer Counter (TC)                               offset 0x08
    volatile uint32_t pr;        //!< Prescale Register (PR)                           offset 0x0C
    volatile uint32_t pc;        //!< Prescale Counter (PC)                            offset 0x10
    volatile uint32_t mcr;       //!< Match Control Register (MCR)                     offset 0x14
    volatile uint32_t mrs[4];    //!< Match Register (MR0..3)                          offset 0x18 - 0x24
    volatile uint32_t unused[5]; //!< Not used in the NHS3100                          offset 0x28
    volatile uint32_t emr;       //!< External Match Register (EMR)                    offset 0x3C
    volatile uint32_t unused1[13];//!< Not used in the NHS3100                         offset 0x40
    volatile uint32_t pwmc;      //!< PWM Control Register (PWMC)                      offset 0x70
} reg_ct16b_t;

static reg_ct16b_t * p_ct16b0 = ((reg_ct16b_t * const)REG_CT16B0_BASE); //!< The 16-bit Timer/Counter 0 registers


/**
 * @brief Interrupt register (IR)
 *
 * @details
 * The Interrupt register (IR) consists of 4 bits for the match interrupts and 1 bit for the
 * capture interrupt. If an interrupt is generated, the corresponding bit in the IR is HIGH.
 * Otherwise, the bit is LOW. Writing a logic 1 to the corresponding IR bit resets the
 * interrupt. Writing a logic 0 has no effect
*/
#define CT16B_IR_MR0INT_BIT      (1U)        //!< Bit 0 MR0INT: Interrupt flag for MR0
#define CT16B_IR_MR1INT_BIT      (1U<<1U)    //!< Bit 1 MR1INT: Interrupt flag for MR1
#define CT16B_IR_MR2INT_BIT      (1U<<2U)    //!< Bit 2 MR2INT: Interrupt flag for MR2
#define CT16B_IR_MR3INT_BIT      (1U<<3U)    //!< Bit 3 MR3INT: Interrupt flag for MR3

/**
 * @brief Timer control register (TCR)
 *
 * @details
 * The timer control register (TCR) is used to control the operation of the timer.
 */
#define CT16B_TCR_ENABLE_BIT      (1U)        //!< Bit 0 ENABLE: Counter Enable
#define CT16B_TCR_RESET_BIT       (1U<<1U)    //!< Bit 1 RESET: Counter Reset

/**
 * @brief Timer counter register (TC)
 *
 * @details
 * When the prescale counter reaches its terminal count, the 16-bit timer counter is
 incremented. Unless reset before reaching its upper limit, the TC counts up to 0x0000
 FFFF and then wraps back to the value 0x0000 0000. This event does not cause an
 interrupt, but a match register can be used to detect possible overflow, if necessary.
 */
#define CT16B_TC_MASK             (0x0000FFFFU)  //!< Mask for the TC value (bits 0-15)

/** 
 * @brief register (PR)
 *
 * @details
 * The 16-bit prescale register specifies the maximum value for the prescale counter.
 */
#define CT16B_PR_MASK             (0x0000FFFFU)  //!< Mask for the PR value (bits 0-15)

/** 
 * @brief Prescale counter register (PC)
 *
 * @details
 * The 16-bit prescale counter controls division of PCLK by some constant value before it is
 * applied to the timer counter. This division allows control of the relationship between the
 * resolution of the timer and the maximum time before the timer overflows. The prescale
 * counter is incremented on every PCLK. When it reaches the value stored in the prescale
 * register, the timer counter is incremented and the prescale counter is reset on the next
 * PCLK. This reset causes the TC to increment on every PCLK when PR = 0, every 2
 * PCLKs when PR = 1, and so on
 */

#define CT16B_PC_MASK             (0x0000FFFFU)  //!< Mask for the PC value (bits 0-15)

/** 
 * @brief Match control register (MCR)
 *
 * @details
 * The match control register (MCR) is used to generate an interrupt, reset the timer
 * counter, and stop the timer counter when a match occurs between the timer counter
 * (TC) and one of the four match registers (MR0 to MR3).
 */

#define CT16B_MCR_MR0I_BIT        (1U)           //!< Bit 0 MR0I: Interrupt on MR0
#define CT16B_MCR_MR0R_BIT        (1U<<1U)       //!< Bit 1 MR0R: Reset on MR0
#define CT16B_MCR_MR0S_BIT        (1U<<2U)       //!< Bit 2 MR0S: Stop on MR0
#define CT16B_MCR_MR1I_BIT        (1U<<3U)       //!< Bit 3 MR1I: Interrupt on MR1
#define CT16B_MCR_MR1R_BIT        (1U<<4U)       //!< Bit 4 MR1R: Reset on MR1
#define CT16B_MCR_MR1S_BIT        (1U<<5U)       //!< Bit 5 MR1S: Stop on MR1
#define CT16B_MCR_MR2I_BIT        (1U<<6U)       //!< Bit 6 MR2I: Interrupt on MR2
#define CT16B_MCR_MR2R_BIT        (1U<<7U)       //!< Bit 7 MR2R: Reset on MR2
#define CT16B_MCR_MR2S_BIT        (1U<<8U)       //!< Bit 8 MR2S: Stop on MR2
#define CT16B_MCR_MR3I_BIT        (1U<<9U)       //!< Bit 9 MR3I: Interrupt on MR3
#define CT16B_MCR_MR3R_BIT        (1U<<10U)      //!< Bit 10 MR3R: Reset on MR3
#define CT16B_MCR_MR3S_BIT        (1U<<11U)      //!< Bit 11 MR3S: Stop on MR3

/** 
 * @brief Match register (MR0/1/2/3)
 *
 * @details
 * The match register values are continuously compared to the counter/timer value. When
 * the two values are equal, actions can be triggered automatically. The action possibilities
 * are to generate an interrupt, reset the counter/timer, or stop the timer. The settings in the
 * MCR register control the actions
 */

#define CT16B_MR_MASK             (0x0000FFFFU)  //!< Mask for the MR value (bits 0-15)

/** 
 * @brief External match register (EMR)
 * 
 * @details
 * The external match register provides both control and status of the external match
 * channels and external match pins CT16B_MAT[1:0].
 * If the match outputs are configured as PWM outputs in the PWMCON registers.
*/

#define CT16B_EMR_EM0_BIT         (1U)        //!< Bit 0 EM0: External Match 0
#define CT16B_EMR_EM1_BIT         (1U<<1U)    //!< Bit 1 EM1: External Match 1
#define CT16B_EMR_EMC0_BITS       (3U<<4U)    //!< Bits 4..5 EMC0: External Match Control 0
#define CT16B_EMR_EMC1_BITS       (3U<<6U)    //!< Bits 6..7 EMC1: External Match Control 1

/** 
 * @brief PWM control register (PWMC)
 *
 * @details
 * The PWM control register is used to configure the match outputs as PWM outputs. Each
 * match output can be independently set as either a PWM output, or match output. The
 * external match register (EMR) controls the function of the match output.
 * Three single-edge controlled PWM outputs can be selected on the CT16B_M[1:0]
 * outputs. One additional match register determines the PWM cycle length. When a match
 * occurs in any of the other match registers, the PWM output is set to HIGH. The match
 * register that is configured to set the PWM cycle length, resets the timer. When the
 * timer is reset to zero, all currently HIGH match outputs configured as PWM outputs are
 * cleared.
*/
#define CT16B_PWMCR_PWMEN0_BIT    (1U)        //!< Bit 0 PWMEN0: PWM mode enable for MR0
#define CT16B_PWMCR_PWMEN1_BIT    (1U<<1U)    //!< Bit 1 PWMEN1: PWM mode enable for MR1

#endif // DEVICE_CT16B_H

