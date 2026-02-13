/*****************************************************************************
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
 * @file timer_hal.c
 * @brief Generic Timer interface definition.
 * @details Outlines the portable Timer abstraction used to configure and
 * interact with Timer peripherals on any supported platform.
 *
 * @author  Jack Wilson
 * @date    February 12, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <hw_config.h>
#include <stdbool.h>
#include <stddef.h> /* for NULL */
#include <stdint.h>
#include <timer_hal.h>
/* Device-specific registers and IRQ helpers */
#include <assert.h>
#include <clock_hal.h>
#include <device_reg.h>
#include <efm32pg22c200f512im40.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

#define TIMER_CMU CMU_NS
#define TIMER_PERIPH TIMER0_NS

    static bool timers_initialized = false;
    /********************************************************************************
     * Defines
     ********************************************************************************/

    // support for Timer callbacks
    typedef struct timer_callback_context_s
    {                                  /**< The GPIO pin number */
        timer_hal_callback_t callback; /**< Registered interrupt callback */
        void *p_callback_handle;       /**< Pointer to callback context (i.e. Button handle)*/
        void *callback_context;        /**< User callback context */
    } timer_callback_context_t;

    /********************************************************************************
     * Private TIMER HAL struct (definition kept in the implementation file).
     ********************************************************************************/
    struct timer_hal
    {
        bool in_use; /*!< Indicates if this timer instance is in use */
        // uint32_t timer_num;         /*!< timer index */
        bool                    enabled;     /*!< timer enabled */
        bool                    auto_reload; /*!< timer auto reload */
        uint32_t                prescaler;   /*!< timer prescaler */
        volatile TIMER_TypeDef *timer;       /*!< pointer to the timer configuration registers */
        uint32_t                mseconds;    /*!< timer period in milliseconds */
        bool                    time_out;    /*!< flag set when the timer completes */
        uint32_t                hardware_timer_id; /*!< the hardware timer id */

        timer_callback_context_t callback; /**< Registered interrupt callbacks */
    };

    static timer_hal_t timer_hal_timers[NUM_TIMERS] = {0};

    void timer_hal_initialize(void)
    {
        if (!timers_initialized)
        {

            timers_initialized = true;
        }
    }

    /******************************************************************************

     *****************************************************************************/
    bool timer_hal_is_initialized(void)
    {
        return timers_initialized;
    }

    /******************************************************************************

     *****************************************************************************/
    p_timer_hal_t timer_hal_create(const uint32_t mseconds, const bool reload,
                                   timer_hal_callback_t callback)
    {
        p_timer_hal_t p_free_slot = NULL;
        if (!timers_initialized)
        {
            timer_hal_initialize();
        }
        for (uint32_t i = 0; i < NUM_TIMERS; i++)
        {
            if (timer_hal_timers[i].in_use == true)
            {
                continue;
            }
            else if (p_free_slot == NULL)
            {
                p_free_slot = (p_timer_hal_t)&timer_hal_timers[i]; /* Track first free */
                p_free_slot->callback.callback          = callback;
                p_free_slot->callback.p_callback_handle = NULL;
                p_free_slot->callback.callback_context =
                    (void *)i; // pass the timer index as the callback context
                p_free_slot->auto_reload       = reload;
                p_free_slot->mseconds          = mseconds;
                p_free_slot->enabled           = false;
                p_free_slot->hardware_timer_id = 0U; // current implementation uses TIMER0 hardware.

                p_free_slot->in_use = true;

                switch (p_free_slot->hardware_timer_id)
                {
                case 0:
                    p_free_slot->prescaler = _TIMER_CFG_PRESC_DIV8;
                    p_free_slot->timer     = TIMER0;
                    break;
                case 1:
                    p_free_slot->timer     = TIMER1;
                    p_free_slot->prescaler = _TIMER_CFG_PRESC_DIV1024;
                    break;
                case 2:
                    p_free_slot->timer     = TIMER2;
                    p_free_slot->prescaler = _TIMER_CFG_PRESC_DIV1024;
                    break;
                case 3:
                    p_free_slot->timer     = TIMER3;
                    p_free_slot->prescaler = _TIMER_CFG_PRESC_DIV1024;
                    break;
                case 4:
                    p_free_slot->timer     = TIMER4;
                    p_free_slot->prescaler = _TIMER_CFG_PRESC_DIV1024;
                    break;
                default:
                    assert(false); /*lint !e506 */
                    break;
                }

                // set up and enable the timer's clock in the CMU

                // Since its the "SET" register, we don't need to read it first
                CMU->CLKEN0_SET = (1u << (p_free_slot->hardware_timer_id +
                                          4u)); // General timers start at bit 4 of CLKEN0
                // was  CMU_ClockEnable(cmuClock_TIMER0, true);

                while (((p_free_slot->timer->EN & TIMER_EN_EN) != 0U) &&
                       ((p_free_slot->timer->STATUS & TIMER_STATUS_SYNCBUSY) != 0U)) /*lint !e835*/
                {
                    // wait for synchronization to complete
                }
                p_free_slot->timer->EN_CLR = TIMER_EN_EN; /*lint !e835*/

                // TODO find a way to best determine the prescaler
                // const uint32_t prescaler = _TIMER_CFG_PRESC_DIV1;

                // p_timer->prescaler = prescaler;
                p_free_slot->timer->CFG =
                    p_free_slot->prescaler << _TIMER_CFG_PRESC_SHIFT |
                    _TIMER_CFG_CLKSEL_PRESCEM01GRPACLK << _TIMER_CFG_CLKSEL_SHIFT | /*lint !e835*/
                    _TIMER_CFG_MODE_UP << _TIMER_CFG_MODE_SHIFT; /*lint !e835 !e845*/

                p_free_slot->timer->EN_SET = TIMER_EN_EN; /*lint !e835*/

                p_free_slot->enabled = false;

                p_free_slot->timer->CMD = TIMER_CMD_STOP;

                p_free_slot->timer->CNT = _TIMER_CNT_RESETVALUE;
            }
        }

        return p_free_slot;
    } /*lint !e953 */

    /******************************************************************************

     *****************************************************************************/
    void timer_hal_enable(const p_timer_hal_t p_timer, bool enable)
    {
        if (p_timer != NULL)
        {
            if (enable)
            {
                timer_hal_start(p_timer);
            }
            p_timer->timer->CMD = enable ? TIMER_CMD_START : TIMER_CMD_STOP; /*lint !e835*/
        }
    }

    /******************************************************************************
     * @brief start the timer
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    void timer_hal_start(const p_timer_hal_t p_timer)
    {
        if (p_timer != NULL)
        {
            const uint32_t timerfreq   = SystemHFXOClockGet() / (p_timer->prescaler + 1u);
            const uint32_t timerTopval = timerfreq * (p_timer->mseconds / 1000u);

            if (p_timer->hardware_timer_id > 0u)
            {
                assert(p_timer->mseconds <= 1000u); // 16bit timer limiit
            }

            p_timer->timer->TOP = timerTopval;
            // TIMER_TopSet(TIMER0, timerTopval);

            const uint32_t flags   = p_timer->timer->IF;
            p_timer->timer->IF_CLR = flags;
            // Enable the Interrupt when the timer overflows (TIMER_IEN_OF)
            p_timer->timer->IEN |= TIMER_IEN_OF; /*lint !e835*/

            // TIMER_IntEnable(TIMER0, TIMER_IEN_OF);

            // Enable the Timer interrupt (IRQ 7- See Reference manual) -
            // on the EMF32PG22 the timers start at 7
            // TIMER0_IRQn            = 7,
            // TIMER1_IRQn            = 8,
            // TIMER2_IRQn            = 9,
            // TIMER3_IRQn            = 10,
            // TIMER4_IRQn            = 11,
            NVIC_EnableIRQ((IRQn_Type)(p_timer->hardware_timer_id + 7u));

            /* other hardware timers can be added similarly */
        }
    }

    /******************************************************************************
     * @brief stop the timer
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    void timer_hal_stop(const p_timer_hal_t p_timer)
    {
        if (p_timer != NULL)
        {
            p_timer->timer->CMD = TIMER_CMD_STOP;
        }
    }

    /******************************************************************************
     * @brief reset the timer
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    void timer_hal_reset(const p_timer_hal_t p_timer)
    {
        if (p_timer != NULL)
        {
            /* reset software-visible state */
            p_timer->time_out = false;
        }
    }

    /******************************************************************************
     * @brief get if the timer has been expired
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    bool timer_hal_get_overflow(const p_timer_hal_t p_timer)
    {
        if (p_timer == NULL)
        {
            return false;
        }
        return p_timer->time_out;
    }

    /******************************************************************************
     * @brief reset the timer count
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    void timer_hal_reset_count(const p_timer_hal_t p_timer)
    {
        if (p_timer != NULL)
        {
            /* clear timeout flag and (where applicable) reset the hardware counter */
            p_timer->time_out = false;
            /* hardware-specific: reset TIMER0 counter */
            TIMER_PERIPH->CNT = 0U;
        }
    }

    static timer_hal_t *timer_find(const TIMER_TypeDef *timer)
    {
        for (uint32_t i = 0u; i < NUM_TIMERS; i++)
        {
            if (timer_hal_timers[i].timer != NULL)
            {
                if (timer_hal_timers[i].timer == timer)
                {
                    return &timer_hal_timers[i];
                }
            }
        }
        return NULL;
    } /*lint !e952 */

    static void timer_hal_IRQ_handler(const TIMER_TypeDef *timer)
    {
        timer_hal_t *p_timer = timer_find(timer);
        if (p_timer != NULL)
        {
            // Note: This is clearing all flags, not just overflow.  We might want to just clear the
            // OF flag
            const uint32_t flags   = p_timer->timer->IF;
            p_timer->timer->IF_CLR = flags;
            p_timer->time_out      = true;
            if (p_timer->callback.callback != NULL)
            {
                p_timer->callback.callback(p_timer, p_timer->callback.callback_context);
            }
        }
    } /*lint !e818 !e953 !e952*/

    /* TIMER0 IRQ handler: handle overflow interrupt */
    /********************************************************************************/
    void TIMER0_IRQHandler(void)
    {
        timer_hal_IRQ_handler(TIMER0);
    }

    /********************************************************************************/
    void TIMER1_IRQHandler(void)
    {
        timer_hal_IRQ_handler(TIMER1);
    }

    /********************************************************************************/
    void TIMER2_IRQHandler(void)
    {
        timer_hal_IRQ_handler(TIMER2);
    }

    /********************************************************************************/
    void TIMER3_IRQHandler(void)
    {
        timer_hal_IRQ_handler(TIMER3);
    }

    /********************************************************************************/
    void TIMER4_IRQHandler(void)
    {
        timer_hal_IRQ_handler(TIMER4);
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
