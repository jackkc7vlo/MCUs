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
 * @file      device_pmu.h
 * @defgroup  device_pmu Power Management Unit (PMU)
 * @author    Jack Wilson
 * @brief     Defines and Macros for the Power Management Unit (PMU)
 *
 * @details   The NHS31xx supports various power control features. In Active mode, when the
 * chip is running, power and clocks to selected peripherals can be optimized for power
 * consumption. In addition, there are three special modes of processor power reduction:
 * Sleep mode, Deep-sleep mode, and Deep power-down mode.
 * Note: Basic RFID/NFC tag functionality is maintained in all sleep and power-down
 * modes, as the RFID/NFC communication block is self-powered. Communication with the
 * Arm Cortex-M0+ core is however only possible in Active mode.
 * Note: The Debug mode is not supported in Deep power-down mode.
 *
 *
 * @mod
 * @moditem{V1.0.x, Nov-18-2025, Jack Wilson, Initial Release, jackkc7vlo@gmail.com }
 * @modend
 *
*/


#include <stdint.h>

#ifndef DEVICE_PMU_H
#define DEVICE_PMU_H
//lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
//lint -restore
//lint -esym(751,pmu_*) There are definitions they may not be used
//lint -esym(754,pmu_*) There are definitions they may not be used
// Power Management Unit Base Address
#define REG_PMU_BASE (0x40038000U)   //!< Power Management Unit Base Address
/**
 * @brief Power Management Unit registers structure definition
 */
typedef struct
{
    volatile uint32_t pcon;      //!< Power control register (PCON)                    offset 0x00
    volatile uint32_t gpreg[5];  //!< General purpose registers (GPREG0..4)            offset 0x04 - 0x14
    volatile uint32_t pstat;     //!< Power status register (PSTAT)                    offset 0x18
    volatile uint32_t reserved;  //!< Reserved                                         offset 0x1C
    volatile uint32_t accstat;   //!< Access status register (ACCSTAT)                 offset 0x20
    volatile uint32_t ldo1v6;    //!< LDO1 1.6V control register (LDO1V6)              offset 0x24
    volatile uint32_t reserved2; //!< Reserved                                         offset 0x28
    volatile uint32_t tmrclkctrl;//!< Timer clock control register (TMRCLKCTRL)        offset 0x2C
    volatile uint32_t imsc;      //!< Interrupt mask set/clear register (IMSC)         offset 0x30
    volatile uint32_t ris;       //!< Raw interrupt status register (RIS)              offset 0x34
    volatile uint32_t mis;       //!< Masked interrupt status register (MIS)           offset 0x38
    volatile uint32_t icr;       //!< Interrupt clear register (ICR)                   offset 0x3C
} reg_pmu_t;


static reg_pmu_t * p_pmu = ((reg_pmu_t * const) REG_PMU_BASE); //!< The Power Management Unit registers

/**
 * @brief Power control register (PCON)
 * 
 * @details The power control register selects whether one of the ARM Cortex-M0+ controlled
 * Power-down modes (Sleep mode or Deep-sleep mode) or the Deep power-down mode is
 * entered. The register also provides the flags for Sleep or Deep-sleep modes and Deep
 * power-down modes respectively. It also contains the overrides for the power source
 * selection.
 */
#define PMU_PCON_DPEN_BIT        (1U<<1U)        //!< Bit 1 DPEN: Deep power-down enable bit
#define PMU_PCON_SLEEPFLAG_BIT   (1U<<8U)        //!< Bit 8 SLEEPFLAG: Sleep mode flag bit
#define PMU_PCON_DPDFLAG_BIT     (1U<<11U)       //!< Bit 11 DPDFLAG: Deep power-down mode flag bit
#define PMU_PCON_PMULPM_BIT      (1U<<13U)       //!< Bit 13 PMULPM: PMU Ultra low power mode bit
#define PMU_PCON_VBAT_BIT        (1U<<14U)       //!< Bit 14 VBAT: VBAT power source override bit
#define PMU_PCON_BODEN_BIT       (1U<<15U)       //!< Bit 15 BODEN: Brown-out detector enable bit
#define PMU_PCON_FORCE_BITS      (3U<<17U)       //!< Bits 17..18 Force the power source selection
#define PMU_PCON_WAKEUP_BIT      (1U<<19U)       //!< Bit 19 WAKEUP: Enable the WAKEUP function on pin PIO0_0

/**
 * @brief Power status register (PSTAT)
 *
 * @details The power status register indicates the power source and configuration of LDOs and
 * power switches. Reading the PSTAT register clears the BOD and RFPOW interrupts.
 * 
 */
#define PMU_PSTAT_PSWNFC        (1U)            //!< Bit 0 POR: Status of NFC power selection switch
#define PMU_PSTAT_PSWBAT        (1U<<1U)        //!< Bit 1 POR: Status of VBAT power selection switch
#define PMU_PSTAT_DPDEXIT       (3U<<3U)        //!< Bits 3..4 DPD exit status
#define PMU_PSTAT_BOD1V8        (1U<<5U)        //!< Bit 5 BOD1V8: Status of brown-out detector for 1.8V domain
#define PMU_PSTAT_RFPOW         (1U<<7U)        //!< Bit 7 RFPOW: Status of RF power switch

/**
 * @brief Access status register (ACCSTAT)
 * 
 * @details The access status register indicates the status of accesses to the PMU registers.
 */

#define PMU_ACCSTAT_PMU_READY_BIT   (1U)        //!< Bit 0 PMU_READY:logic 0: PMU access not possible, logic 1: PMU access possible

/**
 * @brief Timer clock control register (TMRCLKCTRL)
 *
 * @details The timer clock control register controls the 32 kHz oscillator and RTC clock source.
 * Note: this register resides in the always-on power domain, hence its location in the PMU.
 * The 32 kHz TFRO is always running when VDD_ALON is available.
 *
 */
#define PMU_TMRCLKCTRL_RTCSEL_BITS   (1U)        //!< Bits 0 RTC clock source select 0- no clock, 1- 32kHz FRO
#define PMU_TMRCLKCTRL_CLKTRIM_BITS (3U<<16U)    //!< Bits 16..17 32kHz FRO trim bits
#define PMU_TMRCLKCTRL_CURRLVL_BITS (3U<<23U)    //!< Bits 23..24 Current level of 32kHz FRO

/**
 * @brief  PMU Interrupt mask set and clear register (IMSC)
 *
 * @details This register controls whether each of the two possible interrupt conditions in the PMU are
 * enabled.
 */
#define PMU_IMSC_BODI_BIT       (1U)       //!< Bit 0 BOD interrupt mask bit
#define PMU_IMSC_RFPOWRI_BIT    (1U<<1U)   //!< Bit 1 RFPOWRI interrupt mask bit

/**
 * @brief  PMU Raw interrupt status register (RIS)
 *
 * @details This read-only register contains a logic 1 for each interrupt condition that is asserted,
 * regardless of whether the interrupt is enabled in the IMSC register.
 */
#define PMU_RIS_BODRIS_BIT      (1U)       //!< Bit 0 BOD raw interrupt status bit
#define PMU_RIS_RFPOWRIS_BIT    (1U<<1U)   //!< Bit 1 RFPOWRIS raw interrupt status bit

/**
 * @brief  PMU Masked interrupt status register (MIS)
 *
 * @details This read-only register contains the masked interrupt status, which is the logical AND
 * of the raw interrupt status (in the RIS register) and the interrupt mask bits (in the IMSC
 * register).
 */
#define PMU_MIS_BODMIS_BIT      (1U)       //!< Bit 0 BOD masked interrupt status bit
#define PMU_MIS_RFPOWMIS_BIT    (1U<<1U)   //!< Bit 1 RFPOWMIS masked interrupt status bit
/**
 * @brief  PMU Interrupt clear register (ICR)
 *
 * @details Writing a logic 1 to any bit in this register clears the corresponding interrupt
 * in the RIS register. Writing a logic 0 has no effect.
 */
#define PMU_ICR_BODIC_BIT       (1U)       //!< Bit 0 BOD interrupt clear bit
#define PMU_ICR_RFPOWRIC_BIT    (1U<<1U)   //!< Bit 1 RFPOWRIC interrupt clear bit  



#endif // DEVICE_PMU_H

