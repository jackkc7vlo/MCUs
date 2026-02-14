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
#include <tm4c123gh6pm.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    static bool timers_initialized = false;
    /********************************************************************************
     * Defines
     ********************************************************************************/

    // support for Timer callbacks
    typedef struct timer_callback_context_s
    {                                           /**< The GPIO pin number */
        timer_hal_callback_t callback;          /**< Registered interrupt callback */
        void                *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
        void                *callback_context;  /**< User callback context */
    } timer_callback_context_t;

    /********************************************************************************
     * Private TIMER HAL struct (definition kept in the implementation file).
     ********************************************************************************/
    struct timer_hal
    {
        bool in_use; /*!< Indicates if this timer instance is in use */
        // uint32_t timer_num;         /*!< timer index */
        bool     enabled;     /*!< timer enabled */
        bool     auto_reload; /*!< timer auto reload */
        uint32_t prescaler;   /*!< timer prescaler */
        // volatile TIMER_TypeDef *timer;             /*!< pointer to the timer configuration registers */
        uint32_t mseconds;          /*!< timer period in milliseconds */
        bool     time_out;          /*!< flag set when the timer completes */
        uint32_t hardware_timer_id; /*!< the hardware timer id */

        timer_callback_context_t callback; /**< Registered interrupt callbacks */
    };

    static timer_hal_t timer_hal_timers[NUM_TIMERS] = {0};

    void timer_hal_initialize(void)
    {
        if (!timers_initialized)
        {
            SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0; /* enable clock to TIMER1 */
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
    p_timer_hal_t timer_hal_create(const uint32_t mseconds, const bool reload, timer_hal_callback_t callback)
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
                p_free_slot                             = (p_timer_hal_t)&timer_hal_timers[i]; /* Track first free */
                p_free_slot->callback.callback          = callback;
                p_free_slot->callback.p_callback_handle = NULL;
                p_free_slot->callback.callback_context  = (void *)i; // pass the timer index as the callback context
                p_free_slot->auto_reload                = reload;
                p_free_slot->mseconds                   = mseconds;
                p_free_slot->enabled                    = false;
                p_free_slot->hardware_timer_id          = 0U; // current implementation uses TIMER0 hardware.

                p_free_slot->in_use = true;

                switch (p_free_slot->hardware_timer_id)
                {
                case 0:
                    p_free_slot->prescaler = 0;
                    // p_free_slot->timer     = TIMER0;
                    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;           /* disable TIMER0A during configuration */
                    TIMER0_CFG_R   = TIMER_CFG_32_BIT_TIMER;   /* configure for 32-bit timer mode */
                    TIMER0_TAMR_R  = TIMER_TAMR_TAMR_1_SHOT;   /* configure for one-shot mode and down-count */
                    TIMER0_TAPR_R  = 0;                        /* no prescaler in 32-bit mode */
                    TIMER0_TAILR_R = (mseconds * 80000UL) - 1; /* set timer interval: mseconds * 80000 cycles @ 80MHz */
                    TIMER0_ICR_R   = TIMER_ICR_TATOCINT;       /* clear the TIMER0A timeout flag */
                    TIMER0_IMR_R |= TIMER_IMR_TATOIM;          /* enable TIMER0A timeout interrupt */
                    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | (1 << 30); /* set TIMER0A interrupt priority to 2 */
                    NVIC_EN0_R |= (1 << 19);                              /* enable IRQ 19 in NVIC (TIMER0A) */

                    break;
                case 1:
                    // p_free_slot->timer     = TIMER1;
                    p_free_slot->prescaler = 0;
                    break;
                case 2:
                    // p_free_slot->timer     = TIMER2;
                    p_free_slot->prescaler = 0;
                    break;
                case 3:
                    // p_free_slot->timer     = TIMER3;
                    p_free_slot->prescaler = 0;
                    break;
                case 4:
                    // p_free_slot->timer     = TIMER4;
                    p_free_slot->prescaler = 0;
                    break;
                default:
                    // assert(false); /*lint !e506 */
                    break;
                }
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
            p_timer->enabled = true;
            switch (p_timer->hardware_timer_id)
            {
            case 0:
                TIMER0_CTL_R |= TIMER_CTL_TAEN; /* enable TIMER0A */
                break;
            case 1:
                // TIMER1_CTL_R |= TIMER_CTL_TAEN; /* enable TIMER1A */
                break;
            case 2:
                // TIMER2_CTL_R |= TIMER_CTL_TAEN; /* enable TIMER2A */
                break;
            case 3:
                // TIMER3_CTL_R |= TIMER_CTL_TAEN; /* enable TIMER3A */
                break;
            case 4:
                // TIMER4_CTL_R |= TIMER_CTL_TAEN; /* enable TIMER4A */
                break;
            default:
                // assert(false); /*lint !e506 */
                break;
            }
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
            p_timer->enabled = false;
            switch (p_timer->hardware_timer_id)
            {
            case 0:
                TIMER0_CTL_R &= ~TIMER_CTL_TAEN; /* disable TIMER0A */
                break;
            case 1:
                // TIMER1_CTL_R &= ~TIMER_CTL_TAEN; /* disable TIMER1A */
                break;
            case 2:
                // TIMER2_CTL_R &= ~TIMER_CTL_TAEN; /* disable TIMER2A */
                break;
            case 3:
                // TIMER3_CTL_R &= ~TIMER_CTL_TAEN; /* disable TIMER3A */
                break;
            case 4:
                // TIMER4_CTL_R &= ~TIMER_CTL_TAEN; /* disable TIMER4A */
                break;
            default:
                assert(false); /*lint !e506 */
                break;
            }
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
        }
    }

    /*
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
    }
        */

    /*
static void timer_hal_IRQ_handler(const TIMER_TypeDef *timer)
{

    timer_hal_t *p_timer = timer_find(timer);
    if (p_timer != NULL)
    {
    }
}
*/

    /* TIMER0 IRQ handler: handle overflow interrupt */
    /********************************************************************************/
    void Timer0IntHandler(void)
    {
        timer_hal_timers[0].time_out = true; /* set the timer timeout flag for the first timer instance (current
                                                implementation only supports one timer) */
        TIMER0_ICR_R = TIMER_ICR_TATOCINT;   /* clear the TIMER0A timeout flag */

        // timer_hal_IRQ_handler(TIMER0);
    }

    /********************************************************************************/
    void TIMER1_IRQHandler(void)
    {
        // timer_hal_IRQ_handler(TIMER1);
    }

    /********************************************************************************/
    void TIMER2_IRQHandler(void)
    {
        // timer_hal_IRQ_handler(TIMER2);
    }

    /********************************************************************************/
    void TIMER3_IRQHandler(void)
    {
        // timer_hal_IRQ_handler(TIMER3);
    }

    /********************************************************************************/
    void TIMER4_IRQHandler(void)
    {
        // timer_hal_IRQ_handler(TIMER4);
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
