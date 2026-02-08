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
 * @file      device_systick.h
 * @defgroup  device_systick SysTick Timer (SYSTICK)
 * @author    Jack Wilson
 * @brief     Defines and Macros for the SysTick Timer (SYSTICK)
 * @details   Outlines the portable device register abstraction used to configure and interact with the SysTick Timer
 *            peripheral on Arm Cortex-M based devices
 */

#ifndef DEVICE_SYSTICK_H
#define DEVICE_SYSTICK_H

#include "device_regs.h"
#include <stdint.h>
#define SysTick_BASE (SCS_BASE + 0x0010UL) /*!< SysTick Base Address */

/**
  \brief  Structure type to access the System Timer (SysTick).
 */
typedef struct
{
    volatile uint32_t       CTRL;  /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
    volatile uint32_t       LOAD;  /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
    volatile uint32_t       VAL;   /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
    volatile const uint32_t CALIB; /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;

/* SysTick Control / Status Register Definitions */
#define SysTick_CTRL_COUNTFLAG_Pos 16U                                 /*!< SysTick CTRL: COUNTFLAG Position */
#define SysTick_CTRL_COUNTFLAG_Msk (1UL << SysTick_CTRL_COUNTFLAG_Pos) /*!< SysTick CTRL: COUNTFLAG Mask */

#define SysTick_CTRL_CLKSOURCE_Pos 2U                                  /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk (1UL << SysTick_CTRL_CLKSOURCE_Pos) /*!< SysTick CTRL: CLKSOURCE Mask */

#define SysTick_CTRL_TICKINT_Pos 1U                                /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk (1UL << SysTick_CTRL_TICKINT_Pos) /*!< SysTick CTRL: TICKINT Mask */

#define SysTick_CTRL_ENABLE_Pos 0U                                   /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk (1UL /*<< SysTick_CTRL_ENABLE_Pos*/) /*!< SysTick CTRL: ENABLE Mask */

/* SysTick Reload Register Definitions */
#define SysTick_LOAD_RELOAD_Pos 0U                                          /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk (0xFFFFFFUL /*<< SysTick_LOAD_RELOAD_Pos*/) /*!< SysTick LOAD: RELOAD Mask */

/* SysTick Current Register Definitions */
#define SysTick_VAL_CURRENT_Pos 0U                                          /*!< SysTick VAL: CURRENT Position */
#define SysTick_VAL_CURRENT_Msk (0xFFFFFFUL /*<< SysTick_VAL_CURRENT_Pos*/) /*!< SysTick VAL: CURRENT Mask */

/* SysTick Calibration Register Definitions */
#define SysTick_CALIB_NOREF_Pos 31U                              /*!< SysTick CALIB: NOREF Position */
#define SysTick_CALIB_NOREF_Msk (1UL << SysTick_CALIB_NOREF_Pos) /*!< SysTick CALIB: NOREF Mask */

#define SysTick_CALIB_SKEW_Pos 30U                             /*!< SysTick CALIB: SKEW Position */
#define SysTick_CALIB_SKEW_Msk (1UL << SysTick_CALIB_SKEW_Pos) /*!< SysTick CALIB: SKEW Mask */

#define SysTick_CALIB_TENMS_Pos 0U                                          /*!< SysTick CALIB: TENMS Position */
#define SysTick_CALIB_TENMS_Msk (0xFFFFFFUL /*<< SysTick_CALIB_TENMS_Pos*/) /*!< SysTick CALIB: TENMS Mask */

/*@} end of group CMSIS_SysTick */
#define SysTick ((SysTick_Type *)SysTick_BASE) /*!< SysTick configuration struct */
#endif                                         /* DEVICE_SYSTICK_H */