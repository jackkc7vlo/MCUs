/**
 * @file rtc.h
 * @brief defines registers used for Real-Time Clock (RTC) on Arm devices
 * @author Jack Wilson
 * @par
 * Real-Time Clock (RTC)
 * This describes the RTC module, which is identical on all members of the
 * NHS31xx family.  
 * The RTC is configured as follows:
 * • Clock:
 *    The RTC block is clocked from the 32.768 kHz RTC oscillator.  
 * • Features:
 *  - Seconds, Minutes, Hours, Day of the Month, Month, Year, Day of the Week, and
 *      Day of the Year registers
 *  - Alarm registers for Seconds, Minutes, Hours, Day of the Month, Month, and Year
 *  - Automatic leap year correction
 *  - Century counter
 *  - Interrupt generation for alarm and 1-second ticks
 * 
 */
#include <stdint.h>

#ifndef RTC_H
#define RTC_H   
//lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
//lint -restore
//lint -esym(751,rtc_*) There are definitions they may not be used
//lint -esym(754,rtc_*) There are definitions they may not be used
// RTC Base Address
#define REG_RTC_BASE (0x40054000U)   //!< RTC Base Address

/**
 * @brief RTC registers structure definition
 */
typedef struct
{
    volatile uint32_t cr;       //!<  RTC control register                          offset 0x00
    volatile uint32_t sr;       //!<  RTC status register                           offset 0x04
    volatile uint32_t cal;      //!< RTC calibration value                          offset 0x08
    volatile uint32_t sleept;   //!<  RTC Sleep time                                offset 0x0C
    volatile uint32_t val;      //!< RTC value register                             offset 0x10
    volatile uint32_t imis;     //!< RTC interrupt mask set register                offset 0x14
    volatile uint32_t ris;      //!< RTC raw interrupt status register              offset 0x18
    volatile uint32_t mis;      //!< RTC masked interrupt status register           offset 0x1C
    volatile uint32_t icr;      //!< RTC interrupt clear register                   offset 0x20
    volatile uint32_t stat;     //!< RTC status register                            offset 0x24
    volatile uint32_t unknown;  //!< Reserved                                       offset 0x28
    volatile uint32_t time;     //!< RTC time register                              offset 0x30
} reg_rtc_t;


static reg_rtc_t * p_rtc = ((reg_rtc_t * const) REG_RTC_BASE); //!< The RTC registers

/**
 * @brief RTC control register (CR)
 * 
 * @details The CR register controls the operation of the RTC Timer unit. If the unit is disabled,
 * nothing happens.
 */
#define RTC_CR_EN_BIT           (1U)        //!< Bit 0 EN: RTC Enable 1: Enable the RTC Timer, 0: disable
#define RTC_CR_AUTOSTART_BIT    (1U<<1U)    //!< Bit 1 AUTOSTART: RTC Auto Start 1: RTC starts on power-up, 0: RTC requires software start
#define RTC_CR_START_BIT        (1U<<2U)    //!< Bit 2 START: RTC Start 1: Start the RTC Timer, 0: Clear

/**
 * @brief RTC status register (SR)
 * 
 * @details  The SR register shows the status of the RTC Timer unit. SR[1] is identical to
 * PMU.TMRCLKCTRL[0].
 * For the RTC to count down time (RUN=1) the following conditions must be fulfilled:
 * 1. it must be powered.
 * 2. it must be receiving 32 kHz pulses.
 * 3. it must be started
 *
 */
#define RTC_SR_EN_BIT       (1U)        //!< Bit 0 EN: 1: RTC timer enabled, 0: disabled
#define RTC_SR_FREEZE_BIT   (1U<<2U)    //!< Bit 2 FREEZE: asserted near to the edges of the internal tick clock. Values written
                                        //!< to the SLEEPT register during FREEZE=1 are delayed until after this condition clears
#define RTC_SR_RUN_BIT      (1U<<3U)    //!< Bit 3 RUN: 1: RTC timer is running, 0: RTC timer is not running

/**
 * @brief RTC calibration register (CAL)
 * 
 * @details  The CAL register contains the number of TFRO clock pulses in one tick. When the CALIB
 value is reached, one tick is issued. Setting 1 tick equal to 1 s is recommended. The
 default is 32,768 clock pulses in one tick.
 */
#define RTC_CAL_MASK       (0x0000FFFFU)  //!< Mask for the CAL value (bits 0-15)

/**
 * @brief RTC sleep time register (SLEEPT)
 * 
 * @details   The SLEEPT register is set to the time in ticks the timer has to sleep. The timer unit
 * decrements this value each tick and the RTC interrupt is generated whenever the value
 * reaches zero. Writing this register triggers a start unless the AUTOSTART bit is cleared.
 * Manual start or restart is possible by writing logic 1 to CR[2].
 */

#define RTC_SLEEPT_MASK       (0x000FFFFFU)  //!< Mask for the SLEEPT value (bits 0-23)

/**
 * @brief RTC value register (VAL)
 * 
 * @details   The RTCVAL register contains the current remaining value of the timer in ticks.
 */
#define RTC_VAL_MASK       (0x000FFFFFU)  //!< Mask for the VAL value (bits 0-23)

/**
 * @brief RTC timer interrupt mask set/clear register (IE)
 * 
 * @details    This register controls whether the interrupt condition is enabled.
 * Note:  ARM uses the word ‘masked’ in the opposite sense from classic computer
 * terminology, in which 'masked' meant ‘disabled’. ARM uses the word 'masked' to mean
 * ‘enabled’. To avoid confusion, we do not use the word ‘masked’.
 */
 #define RTC_IE_BIT      (1U)        //!< Bit 0 IE: Set this bit to enable interrupt when the countdown is finished

/**
 * @brief RTC timer raw interrupt status register (RIS)
 * 
 * @details    This read-only register contains a logic 1 for each interrupt condition that is asserted,
 * regardless of whether the interrupt is enabled in the IMSC register.
 */

#define RTC_RIS_BIT      (1U)        //!< Bit 0 RIS: This bit is logic 1 when the countdown has finished

/**
 * @brief RTC timer masked interrupt status register (MIS)
 * 
 * @details     This read-only register contains a logic 1 for each interrupt condition that is asserted
 * and enabled in the IMSC registers. When an RTC interrupt occurs, the interrupt service
 * routine should read this register to determine the cause or causes of the interrupt.
 */

#define RTC_MIS_BIT      (1U)        //!< Bit 0 MIS: This bit is logic 1 when the countdown has finished and IE is set
/**
 * @brief RTC timer interrupt clear register (ICR)
 * 
 * @details     Writing a logic 1 to a bit in this register clears the corresponding interrupt.
 * Writing a logic 0 has no effect.
 */
#define RTC_ICR_BIT      (1U)        //!< Bit 0 ICR: Writing 1 to this bit clears the countdown finished interrupt
/**
 * @brief RTC timer status register (STAT)
 * 
 * @details     This read-only register contains status information for the RTC timer.
 */
#define RTC_STAT_BUSY_BIT      (1U)        //!< Bit 0 BUSY: 1: RTC timer is busy updating SLEEPT or VAL registers, 0: RTC timer is not busy
/**
 * @brief RTC timer time register (TIME)
 * 
 * @details     This read-only register contains the current time in seconds since the RTC was started.
 */
#define RTC_TIME_MASK       (0xFFFFFFFFU)  //!< Mask for the TIME value (bits 0-31)

#endif // RTC_H