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
#include <clock_hal.h>
#include <device_irq.h>
#include <device_rcc.h>
#include <device_tim.h>

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
        bool     enabled;           /*!< timer enabled */
        bool     auto_reload;       /*!< timer auto reload */
        uint32_t prescaler;         /*!< timer prescaler */
        uint32_t mseconds;          /*!< timer period in milliseconds */
        bool     time_out;          /*!< flag set when the timer completes */
        uint32_t hardware_timer_id; /*!< the hardware timer id */

        timer_callback_context_t callback; /**< Registered interrupt callbacks */
    };

    static timer_hal_t timer_hal_timers[NUM_TIMERS] = {0};

    /*
     * Platform-specific timer object for STM32L412RB
     * Example: map to TIM2 (hardware timer id = 2). This provides a ready-to-use
     * timer instance that can be referenced directly if needed.
     */
    /*
   static timer_hal_t stm32_tim2_timer = {
       .in_use            = false,
       .enabled           = false,
       .auto_reload       = false,
       .prescaler         = 0U,
       .mseconds          = 0U,
       .time_out          = false,
       .hardware_timer_id = 2U, // TIM2
       .callback =
           {
               .callback          = NULL,
               .p_callback_handle = NULL,
               .callback_context  = NULL,
           },
   };
   */
    void timer_hal_initialize(void)
    {
        if (!timers_initialized)
        {
            /* Enable TIM2 peripheral clock and configure NVIC for TIM2 */
            /* Turn on APB1 peripheral clock for TIM2 */
            *p_device_rcc_apb1enr |= RCC_APB1ENR_TIM2EN_BIT;

            /* Set a reasonable IRQ priority and enable the TIM2 interrupt */
            NVIC_SetPriority(TIM2_IRQN, 3U);
            NVIC_EnableIRQ(TIM2_IRQN);

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
                p_free_slot->hardware_timer_id = 2U; // for now, all timers map to TIM2. This can be extended to support
                                                     // more hardware timers as needed.

                p_free_slot->in_use = true;
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
            p_timer->enabled = enable;
            // perform hardware-specific operations to enable/disable the timer as needed
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
            if (p_timer->enabled == true)
            {
                p_timer->time_out = false; // reset timeout flag when starting the timer

                /* Hardware-specific: handle TIM2 when requested */
                if (p_timer->hardware_timer_id == 2U)
                {
                    /* Use a 1 kHz counter (1 tick = 1 ms) when possible so ARR = mseconds-1
                     * PSC = (timer_clk / 1000) - 1
                     */
                    const uint32_t timer_clk = clock_hal_get_timer_freq(); /* Hz - timer input clock on APB1 */
                    uint32_t       psc       = 0U;
                    uint32_t       arr       = 0U;

                    if (timer_clk >= 1000U)
                    {
                        psc = (timer_clk / 1000U);
                        if (psc > 0U)
                        {
                            psc = psc - 1U; /* PSC register is (divider - 1) */
                        }
                        if (psc > 0xFFFFU)
                        {
                            psc = 0xFFFFU; /* clamp if too large */
                        }
                        /* ARR in ticks (milliseconds) */
                        arr = (p_timer->mseconds > 0U) ? (p_timer->mseconds - 1U) : 0U;
                    }
                    else
                    {
                        /* timer clock < 1kHz: fall back to PSC=0 and compute ARR in timer ticks */
                        psc            = 0U;
                        uint64_t ticks = ((uint64_t)timer_clk * (uint64_t)p_timer->mseconds) / 1000ULL;
                        if (ticks == 0ULL)
                        {
                            ticks = 1ULL;
                        }
                        arr = (uint32_t)(ticks - 1ULL);
                    }

                    TIM2->CR1 &= ~TIM_CR1_CEN; /* ensure timer is stopped */
                    TIM2->PSC = psc;
                    TIM2->ARR = arr;
                    TIM2->CNT = 0U;
                    TIM2->SR  = 0U;             /* clear all status flags */
                    TIM2->DIER |= TIM_DIER_UIE; /* enable update interrupt */
                    TIM2->CR1 |= TIM_CR1_CEN;   /* start timer */
                }
                /* other hardware timers can be added similarly */
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
            /* mark disabled in the software object */
            p_timer->enabled = false;

            /* hardware-specific: stop TIM2 if mapped */
            if (p_timer->hardware_timer_id == 2U)
            {
                TIM2->CR1 &= ~TIM_CR1_CEN;   /* stop counter */
                TIM2->DIER &= ~TIM_DIER_UIE; /* disable update interrupt */
                TIM2->SR &= ~TIM_SR_UIF;     /* clear UIF */
                TIM2->CNT = 0U;              /* reset count */
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
            /* hardware-specific: reset the timer counter and status flags */
            if (p_timer->hardware_timer_id == 2U)
            {
                TIM2->CR1 &= ~TIM_CR1_CEN;
                TIM2->CNT = 0U;
                TIM2->SR &= ~TIM_SR_UIF;
            }
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
            /* hardware-specific: write 0 to CNT register for the peripheral */
            if (p_timer->hardware_timer_id == 2U)
            {
                TIM2->CNT = 0U;
                TIM2->SR &= ~TIM_SR_UIF;
            }
        }
    }

    /* TIM2 IRQ handler: clear UIF, set timeout and call callbacks for mapped timers */
    void TIM2_IRQHandler(void)
    {
        if (TIM2->SR & TIM_SR_UIF)
        {
            TIM2->SR &= ~TIM_SR_UIF;

            for (uint32_t i = 0; i < NUM_TIMERS; i++)
            {
                if (timer_hal_timers[i].in_use && timer_hal_timers[i].hardware_timer_id == 2U)
                {
                    timer_hal_timers[i].time_out = true;
                    if (timer_hal_timers[i].callback.callback)
                    {
                        timer_hal_timers[i].callback.callback(timer_hal_timers[i].callback.p_callback_handle,
                                                              timer_hal_timers[i].callback.callback_context);
                    }
                    if (!timer_hal_timers[i].auto_reload)
                    {
                        TIM2->CR1 &= ~TIM_CR1_CEN; /* stop one-shot */
                    }
                }
            }
        }
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
