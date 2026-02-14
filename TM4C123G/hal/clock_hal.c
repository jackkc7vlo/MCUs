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

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <assert.h>
#include <clock_hal.h>
#include <tm4c123gh6pm.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Functions
     ********************************************************************************/
    static volatile uint32_t ms_counter      = 0u;
    static volatile uint32_t delay_ms        = 0u;
    static volatile uint32_t SystemCoreClock = 16000000; // 16 MHz default clock speed

    //! The interrupt handler for the SysTick module
    void SysTick_Handler(void)
    {

        ms_counter++;

        // GPIO_PORTF_DATA_R ^= (1 << 2);

        if (delay_ms > 0)
        {
            delay_ms--;
        }
    }
/**
 * @brief  Initialize the Systick clock.
 *
 * @retval none
 */
#define SYSDIV2 4u
    void PLL_Init(void)
    {
        SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;
        SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;

        // Disable legacy divider
        SYSCTL_RCC_R &= ~SYSCTL_RCC_USESYSDIV;

        SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;
        SYSCTL_RCC_R |= SYSCTL_RCC_XTAL_16MHZ;

        SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;
        SYSCTL_RCC2_R |= SYSCTL_RCC2_OSCSRC2_MO;

        SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;

        SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;
        SYSCTL_RCC2_R &= ~SYSCTL_RCC2_SYSDIV2LSB;
        SYSCTL_RCC2_R &= ~SYSCTL_RCC2_SYSDIV2_M;
        SYSCTL_RCC2_R |= (SYSDIV2 << 22); // 80 MHz
        SystemCoreClock = 80000000u;

        while ((SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0)
        {
            ;
        }
        SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
    }

    static inline void SysTick_Init(void)
    {
        /*
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // Enable clock for Port F
        while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5) == 0)
            ; // Wait until ready

        GPIO_PORTF_DIR_R |= (1 << 2);    // PF2 output
        GPIO_PORTF_DEN_R |= (1 << 2);    // Digital enable
        GPIO_PORTF_AFSEL_R &= ~(1 << 2); // GPIO function
        GPIO_PORTF_AMSEL_R &= ~(1 << 2); // Disable analog
        */
        PLL_Init();

        uint32_t period   = (SystemCoreClock / 1000u) - 1u; // period for 1ms interrupts
        NVIC_ST_CTRL_R    = 0;                              // disable SysTick during setup
        NVIC_ST_RELOAD_R  = period;
        NVIC_ST_CURRENT_R = 0;                            // any write to current clears it
        NVIC_SYS_PRI3_R   = NVIC_SYS_PRI3_R & 0x00FFFFFF; // priority 0
        NVIC_ST_CTRL_R    = NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_CLK_SRC;
    }

    void clock_hal_delay(const uint32_t ms)
    {
        delay_ms = ms;
        while (delay_ms > 0)
        {
        }
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

        // clock_hal_set_clock_frequency(2000000u);
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
    }
    /**
     * @brief Set the clock frequency.
     * @param[in]       frequency The clock frequency in Hz
     * @return          none
     */
    void clock_hal_set_clock_frequency(const uint32_t frequency)
    {
    }

    /**
     * @brief Get the clock divisor.
     * @return          uint32_t The clock divisor
     */
    uint32_t clock_hal_get_divisor(void)
    {
        return 1u;
    }
    /**
     * @brief Get the clock frequency.
     * @return          uint32_t The clock frequency in Hz
     */
    uint32_t clock_hal_get_freq(void)
    {
        return 80000000u;
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

    static void systick_wait(unsigned long delay)
    {
        volatile unsigned long elapsed_time = 0;
        volatile unsigned long start_time   = NVIC_ST_CURRENT_R;
        do
        {
            volatile unsigned long current_time = NVIC_ST_CURRENT_R;
            elapsed_time                        = (start_time - current_time) & 0x00FFFFFFu;
        } while (elapsed_time < delay);
    }

    void clock_hal_delay_us(const uint32_t us)
    {
        systick_wait(80 * us);
    } /*lint !e438 !e550*/

    uint32_t clock_hal_get_milliseconds(void)
    {
        return ms_counter;
    }
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */