/**
 * @file systick.h
 * @brief defines registers used for SysTick on Arm devices
 * @author Jack Wilson
 * @par
 * 
 *
 */
#include <stdint.h>

#ifndef SYSTICK_H
#define SYSTICK_H
//lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
//lint -restore
//lint -esym(751,systick_*) There are definitions they may not be used
//lint -esym(754,systick_*) There are definitions they may not be used


/*
The SYSTICK timer is a 24-bit timer that counts down to zero and generates an interrupt. 
The intent is to provide a fixed 10 millisecond time interval between interrupts. The
SYSTICK timer is clocked from the CPU system clock (see Figure 22) or from the
reference clock. The reference clock is fixed to half the frequency of the CPU clock. To
generate recurring interrupts at a specific interval, the RVR register must be initialized
with the correct value for the desired interval. A default value is provided in the CALIB
register which is software can change.
If the CPU clock is set to 2 MHz, this frequency gives a 10 millisecond interrupt rate
*/

/*
Example timer calculation
 To use the system tick timer, do the following:
 1. Program the RVR register with the reload value RELOAD to obtain the desired time
 interval.
 2. Clear the CVR register by writing to it. This action ensures that the timer counts from
 the RVR value rather than an arbitrary value when the timer is enabled.
 3. Program the SCR register with the value 0x7 which enables the SYSTICK timer and
 the SYSTICK timer interrupt.
 The following example illustrates the selection of the timer reload value to obtain a 10 ms
 time interval with the system clock set to 20 MHz.
 The system tick clock = system clock = 8 MHz. Bit CLKSOURCE in the CSR register set
 to 1 (system clock).
 RELOAD = (system tick clock
 frequency × 10 ms) − 1 = (8 MHz × 10 ms) − 1 = 80000 − 1 = 79999 = 0x0001 387F
*/

// Systick Base Address
#define REG_SYSTICK_BASE (0xE000E000)   //!< Systick Base Address

typedef struct
{
    volatile unused_t reserved0[4]; //!< Reserved                                            offset 0x00 - 0x0C    
    volatile uint32_t csr;          //!< System timer control and status register (CSR)      offset 0x10
    volatile uint32_t rvr;          //!< System timer reload value register (RVR)            offset 0x14
    volatile uint32_t cvr;          //!< System timer current value register (CVR)           offset 0x18
    volatile uint32_t calib;        //!< System timer calibration value register (CALIB)     offset 0x1C

} reg_systick_t;

static reg_systick_t * p_systick  = (reg_systick_t * const) REG_SYSTICK_BASE;

/**
 * @brief System timer control and status register (CSR)
 * 
 * The CSR register contains control information for the SYSTICK timer and provides a
 * status flag. This register is part of the Arm Cortex-M0+ core system timer register block.
 * This register determines the clock source for the system tick timer
*/

#define SYST_CSR_ENABLE_BIT        (1U)        //!< Bit 0 ENABLE: Enables the counter
#define SYST_CSR_TICKINT_BIT      (1U<<1U)   //!< Bit 1 TICKINT: Enables the SysTick exception request
#define SYST_CSR_CLKSOURCE_BIT    (1U<<2U)   //!< Bit 2 CLKSOURCE: Indicates the clock source
#define SYST_CSR_COUNTFLAG_BIT    (1U<<16U)  //!< Bit 16 COUNTFLAG: Returns 1 if timer counted to 0 since last time this was read

/** 
 * @brief   System timer reload value register (RVR)
 *
 * The RVR register is set to the value that is loaded into the SYSTICK timer whenever it
 * counts down to zero. Timer initialization loads this register via software. If the CPU is
 * running at the frequency intended for use with the CALIB value, the CALIB register may
 * be read and used as the value for RVR register.
*/


#define SYST_RVR_RELOAD_MASK    (0x00FFFFFFU)  //!< Mask for the RELOAD value (bits 0-23)

/** 
 * @brief   System timer current value register (CVR)
 *
 * The CVR register is the current value of the SYSTICK timer. Writing any value to this
 * register clears it to 0 and also clears the COUNTFLAG in the CSR register. The timer
 * then begins counting down from the value in the RVR register.
*/

#define SYST_CVR_CURRENT_MASK    (0x00FFFFFFU)  //!< Mask for the CURRENT value (bits 0-23)


/** 
 * @brief   System timer calibration value register (CALIB)
 *
 * The CALIB register provides a factory-calibrated value to load the RVR register to
 * create a 10 ms time interval, assuming that the system clock is running at the frequency
 * intended during the factory calibration. The TENMS value is in ticks of the clock source
 * selected for the SYSTICK timer.
*/

#define SYST_CALIB_TENMS_MASK    (0x00FFFFFFU)  //!< Mask for the TENMS value (bits 0-23)
#define SYST_CALIB_SKEW_BIT      (1U<<30U)     //!< Bit 30 SKEW: Indicates whether the TENMS value is exact
#define SYST_CALIB_NOREF_BIT     (1U<<31U)     //!< Bit 31 NOREF: Indicates whether the reference clock is provided

#endif // SYSTICK_H