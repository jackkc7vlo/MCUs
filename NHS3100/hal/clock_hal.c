/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       clock_hal.c
 * @author     Jack Wilson
 * @brief
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-15-2023     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 *
 * ==============================================================================
 * @endverbatim
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************/

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/

/********************************************************************************
 * Includes
 ********************************************************************************/
#include <clock_hal.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <reg_hal.h>
#include <assert.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Functions
     ********************************************************************************/

    /**
     * @brief  Initialize the Systick clock.
     *
     * @retval none
     */
    static inline void SysTick_Init()
    {
        // Set SysTick to interrupt every ms
        SysTick->LOAD = 2000u - 1u; /* reload register - assuming running at 2MHz */
        SysTick->VAL = 0u;          /* Load the SysTick Counter Value */
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                        SysTick_CTRL_TICKINT_Msk |
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
        NSS_SYSCON->SYSAHBCLKCTRL &= ~((uint32_t)CLOCK_PERIPHERAL_16TIMER0 & 0x1DBFFCu);
        NSS_SYSCON->SYSAHBCLKCTRL |= (uint32_t)CLOCK_PERIPHERAL_16TIMER0 & 0x1DBFFCu;
        clock_hal_set_clock_frequency(2000000u);
        SysTick_Init();

        NSS_SYSCON->SYSAHBCLKCTRL |= (uint32_t)CLOCK_PERIPHERAL_GPIO & 0x1DBFFCu;
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
        assert(divisor > 0u);

        /* If setting system clock divisor to 1, make sure that either flash wait states is set to 0 or
         * flash high power mode is configured Else core might end up in hard fault (flash access error).
         * See "SW Clock Restrictions" */
        // assert((divisor > 1) || Chip_Flash_GetHighPowerMode() || (Chip_Flash_GetNumWaitStates() > 0));

        /* Clip divisor to 128 beforehand to narrow down the maximum number of calculation cycles to 8 */
        if (divisor > 128u)
        {
            divisor = 128u;
        }

        /* Calculate floor(log2(divisor)) */
        while ((divisor >>= 1u) > 0u)
        {
            hw_divisor++;
        }

        /* As the HW does not clear the SYSCLKUEN bit, we force it here to be cleared before setting the divisor,
         * otherwise it might happen that the clock frequency is immediately changed when the divisor is set and not
         * when the clock update sequence is done */
        NSS_SYSCON->SYSCLKUEN = 0u;

        /* Set the new divisor */
        NSS_SYSCON->SYSCLKCTRL = ((NSS_SYSCON->SYSCLKCTRL & (~(0x7u << 1u))) | ((hw_divisor & 0x7u) << 1u));

        /* This 0-1 sequence updates the System clock divisor */
        NSS_SYSCON->SYSCLKUEN = 0u;
        NSS_SYSCON->SYSCLKUEN = 1u;
    }
    /**
     * @brief Set the clock frequency.
     * @param[in]       frequency The clock frequency in Hz
     * @return          none
     */
    void clock_hal_set_clock_frequency(const uint32_t frequency)
    {
        /* Divide-by-0 NOT allowed */
        assert(frequency > 0u);

        /* A simple integer division, rounding down is OK: this will ensure the closest supported value higher than or
         * equal to 'frequency' is chosen. This due to the behavior of Chip_Clock_System_SetClockDiv.
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
        return 1u << ((NSS_SYSCON->SYSCLKCTRL >> 1u) & 0x7u);
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
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
