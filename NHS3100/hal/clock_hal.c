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
 * @file clock_hal.h
 * @brief Generic GPIO HAL interface definition.
 * @details Outlines the portable GPIO abstraction used to configure and
 * interact with digital pins on any supported platform.
 *
 * @author  Jack Wilson
 * @date    February 2, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/

/********************************************************************************
 * Includes
 ********************************************************************************/
/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SysTick     System Tick Timer (SysTick)
  \brief    Type definitions for the System Timer Registers.
  @{
 */

/**
  \brief  Structure type to access the System Timer (SysTick).
 */

#include <stddef.h>
#include <stdint.h>

#define SCS_BASE (0xE000E000UL) /*!< System Control Space Base Address */
#define SysTick_BASE (SCS_BASE + 0x0010UL)
#define SysTick ((SysTick_Type *)SysTick_BASE) /*!< SysTick configuration struct */

typedef struct
{
    volatile uint32_t CTRL;  /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
    volatile uint32_t LOAD;  /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
    volatile uint32_t VAL;   /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
    volatile uint32_t CALIB; /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;

/* SysTick Control / Status Register Definitions */
#define SysTick_CTRL_COUNTFLAG_Pos 16U /*!< SysTick CTRL: COUNTFLAG Position */
#define SysTick_CTRL_COUNTFLAG_Msk                                                                 \
    (1UL << SysTick_CTRL_COUNTFLAG_Pos) /*!< SysTick CTRL: COUNTFLAG Mask */

#define SysTick_CTRL_CLKSOURCE_Pos 2U /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk                                                                 \
    (1UL << SysTick_CTRL_CLKSOURCE_Pos) /*!< SysTick CTRL: CLKSOURCE Mask */

#define SysTick_CTRL_TICKINT_Pos 1U /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk                                                                   \
    (1UL << SysTick_CTRL_TICKINT_Pos) /*!< SysTick CTRL: TICKINT Mask */

#define SysTick_CTRL_ENABLE_Pos 0U /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk                                                                    \
    (1UL /*<< SysTick_CTRL_ENABLE_Pos*/) /*!< SysTick CTRL: ENABLE Mask */

/* SysTick Reload Register Definitions */
#define SysTick_LOAD_RELOAD_Pos 0U /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk                                                                    \
    (0xFFFFFFUL /*<< SysTick_LOAD_RELOAD_Pos*/) /*!< SysTick LOAD: RELOAD Mask */

/* SysTick Current Register Definitions */
#define SysTick_VAL_CURRENT_Pos 0U /*!< SysTick VAL: CURRENT Position */
#define SysTick_VAL_CURRENT_Msk                                                                    \
    (0xFFFFFFUL /*<< SysTick_VAL_CURRENT_Pos*/) /*!< SysTick VAL: CURRENT Mask */

/* SysTick Calibration Register Definitions */
#define SysTick_CALIB_NOREF_Pos 31U /*!< SysTick CALIB: NOREF Position */
#define SysTick_CALIB_NOREF_Msk (1UL << SysTick_CALIB_NOREF_Pos) /*!< SysTick CALIB: NOREF Mask */

#define SysTick_CALIB_SKEW_Pos 30U                             /*!< SysTick CALIB: SKEW Position */
#define SysTick_CALIB_SKEW_Msk (1UL << SysTick_CALIB_SKEW_Pos) /*!< SysTick CALIB: SKEW Mask */

#define SysTick_CALIB_TENMS_Pos 0U /*!< SysTick CALIB: TENMS Position */
#define SysTick_CALIB_TENMS_Msk (0xFFFFFFUL /*<< SysTick_CALIB_TENMS_Pos*/)

/*@} end of group CMSIS_SysTick */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <assert.h>
#include <clock_hal.h>

#include <device_syscon.h>
#include <device_systick.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/
#define NSS_SFRO_FREQUENCY 8000000u /*!< System Free-Running Oscillator (SFRO) */

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Functions
     ********************************************************************************/
    static volatile uint32_t ms_ticks            = 0;
    static volatile bool     tick_1ms_elapsed    = false;
    static volatile bool     tick_1000ms_elapsed = false;

    //! The interrupt handler for the SysTick module
    void SysTick_Handler(void)
    {
        ms_ticks++;

        tick_1ms_elapsed = true;

        if (ms_ticks % 1000 == 0)
        {
            tick_1000ms_elapsed = true;
        }
    }
    /**
     * @brief  Initialize the Systick clock.
     *
     * @retval none
     */
    static inline void SysTick_Init()
    {
        // Set SysTick to interrupt every ms
        SysTick->LOAD = 2000u - 1u; /* reload register - assuming running at 2MHz */
        SysTick->VAL  = 0u;         /* Load the SysTick Counter Value */
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk |
                        SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
    }

    /**
     * @brief  Initialize the clock before doing almost anything else.
     *
     * This function initializes the clock.  It should be called before any other
     * clock functions are called.
     *
     * @retval none
     */
    void clock_hal_init(void)
    {
        // disable and enable the 16bit timer clock
        p_syscon->ahbclkctrl &= ~((uint32_t)SYSCON_AHBCLKCTRL_CT16B_BIT & 0x1DBFFCu);
        p_syscon->ahbclkctrl |= (uint32_t)SYSCON_AHBCLKCTRL_CT16B_BIT & 0x1DBFFCu;

        clock_hal_set_clock_frequency(2000000u);
        SysTick_Init();

        // p_syscon->sysahbclkctrl |= (uint32_t)CLOCK_PERIPHERAL_GPIO & 0x1DBFFCu;
    }

    /**
     * @brief Set the clock divisor.
     * @param[in]       divisor The clock divisor
     * @return          none
     */
    void clock_hal_set_divisor(uint32_t divisor)
    {
        uint32_t hw_divisor = 0u;

        /* Divide-by-0 NOT allowed */
        // assert(divisor > 0u);

        /* If setting system clock divisor to 1, make sure that either flash wait states is set to 0
         * or flash high power mode is configured Else core might end up in hard fault (flash access
         * error). See "SW Clock Restrictions" */
        // assert((divisor > 1) || Chip_Flash_GetHighPowerMode() || (Chip_Flash_GetNumWaitStates() >
        // 0));

        /* Clip divisor to 128 beforehand to narrow down the maximum number of calculation cycles to
         * 8 */
        if (divisor > 128u)
        {
            divisor = 128u;
        }

        /* Calculate floor(log2(divisor)) */
        while ((divisor >>= 1u) > 0u)
        {
            hw_divisor++;
        }

        /* As the HW does not clear the SYSCLKUEN bit, we force it here to be cleared before setting
         * the divisor, otherwise it might happen that the clock frequency is immediately changed
         * when the divisor is set and not when the clock update sequence is done */
        p_syscon->sysclkuen = 0u;

        /* Set the new divisor */
        p_syscon->sysclkctrl =
            ((p_syscon->sysclkctrl & (~(0x7u << 1u))) | ((hw_divisor & 0x7u) << 1u));
        /* This 0-1 sequence updates the System clock divisor */
        p_syscon->sysclkuen = 0u;
        p_syscon->sysclkuen = 1u;
    }
    /**
     * @brief Set the clock frequency.
     * @param[in]       frequency The clock frequency in Hz
     * @return          none
     */
    void clock_hal_set_clock_frequency(const uint32_t frequency)
    {
        /* Divide-by-0 NOT allowed */
        // assert(frequency > 0u);

        /* A simple integer division, rounding down is OK: this will ensure the closest supported
         * value higher than or equal to 'frequency' is chosen. This due to the behavior of
         * Chip_Clock_System_SetClockDiv.
         */
        clock_hal_set_divisor(NSS_SFRO_FREQUENCY / frequency);
    }

    /**
     * @brief Get the clock divisor.
     * @return          uint32_t The clock divisor
     */
    uint32_t clock_hal_get_divisor(void)
    {
        /* Calculate 2 ^ "hw_divisor" */
        return 1u << ((p_syscon->sysclkctrl >> 1u) & 0x7u);
    }
    /**
     * @brief Get the clock frequency.
     * @return          uint32_t The clock frequency in Hz
     */
    uint32_t clock_hal_get_freq(void)
    {
        return NSS_SFRO_FREQUENCY / clock_hal_get_divisor();
    }
    /**
     * @brief Get the number of nanoseconds per tick.
     * @return          uint32_t The nanoseconds per tick
     */
    uint32_t clock_hal_get_ns_per_tick(void)
    {
        return 1000u * 1000u * 1000u / clock_hal_get_freq();
    }
    /**
     * @brief Delay for the specified number of microseconds.
     * @param[in]       us The number of microseconds to delay
     * @return          none
     */
    void clock_hal_delay_us(const uint32_t us)
    {
        uint32_t ticks_to_wait;
        // uint32_t ns_per_tick;

        /* Do not wait if us is less than or equal to 0 */
        if (us == 0u)
        {
            return;
        }
        /* Limit to 4 seconds, otherwise ticks_to_wait will overflow */
        assert(us <= 4u * 1000u * 1000u);

        /* Calculate the time taken (in ns) per tick */
        // ns_per_tick = (uint32_t)(1000 * 1000 * 1000 / Chip_Clock_System_GetClockFreq());

        /* Calculate the number of ticks to wait */
        ticks_to_wait = 1000u * us / clock_hal_get_ns_per_tick();

        /* Subtract the overhead of the function call (found by experiment) */
        ticks_to_wait -= 476u; // OE: was 480;

        /* Busy wait loop in assembler so that it will never be optimized out */
        __asm("movs r1, %[ticks]       \n"
              "_BUSYWAIT_LOOP:         \n"
              "sub  r1, #3             \n" /* instruction 1: 3 ticks per loop */
              "bgt _BUSYWAIT_LOOP      \n" /* instruction 2 */
              :                            /* no output */
              : [ticks] "r"(ticks_to_wait)
              : "r1", "cc");

        /* Recover r1 from the stack */
        //__asm ("pop {r1}");
    } /*lint !e438 !e550*/

    void clock_hal_delay(const uint32_t ms)
    {
        for (uint32_t i = 0u; i < ms; i++)
        {
            clock_hal_delay_us(1000u);
        }
    }

    uint32_t clock_hal_get_milliseconds(void)
    {
        return ms_ticks;
    }
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */