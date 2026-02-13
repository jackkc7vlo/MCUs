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
#include "driver/gptimer.h"
#include "esp_log.h"
#include "esp_system.h"
#include <assert.h>
#include <clock_hal.h>

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
    {                                           /**< The GPIO pin number */
        timer_hal_callback_t callback;          /**< Registered interrupt callback */
        void                *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
        void                *callback_context;  /**< User callback context */
    } timer_callback_context_t;

    gptimer_config_t timer_config = {
        .clk_src       = GPTIMER_CLK_SRC_DEFAULT,
        .direction     = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000 // 1MHz, 1 tick=1us
    };

    /********************************************************************************
     * Private TIMER HAL struct (definition kept in the implementation file).
     ********************************************************************************/
    struct timer_hal
    {
        bool             in_use;            /*!< Indicates if this timer instance is in use */
        uint32_t         timer_num;         /*!< timer index */
        bool             enabled;           /*!< timer enabled */
        bool             auto_reload;       /*!< timer auto reload */
        uint32_t         prescaler;         /*!< timer prescaler */
        gptimer_handle_t timer;             /*!< pointer to the timer configuration registers */
        uint32_t         mseconds;          /*!< timer period in milliseconds */
        bool             time_out;          /*!< flag set when the timer completes */
        uint32_t         hardware_timer_id; /*!< the hardware timer id */

        timer_callback_context_t callback; /**< Registered interrupt callbacks */
    };

    static bool IRAM_ATTR example_timer_on_alarm_cb_v1(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata,
                                                       void *user_data)
    {
        (void)edata;
        timer_hal_t *p_timer = (timer_hal_t *)user_data;
        // stop timer immediately
        gptimer_stop(timer);
        p_timer->time_out = (bool)true;

        // return whether we need to yield at the end of ISR
        return (true);
    }

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

                (void)gptimer_new_timer(&timer_config, &p_free_slot->timer);

                gptimer_event_callbacks_t cbs = {
                    .on_alarm = example_timer_on_alarm_cb_v1,
                };
                ESP_ERROR_CHECK(gptimer_register_event_callbacks(p_free_slot->timer, &cbs, (void *)p_free_slot));

                gptimer_alarm_config_t alarm_config1 = {
                    .reload_count               = 0,
                    .alarm_count                = p_free_slot->mseconds * 1000,
                    .flags.auto_reload_on_alarm = p_free_slot->auto_reload,
                };
                ESP_ERROR_CHECK(gptimer_set_alarm_action(p_free_slot->timer, &alarm_config1));
                break;
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
                ESP_ERROR_CHECK(gptimer_enable(p_timer->timer));
                timer_hal_start(p_timer);
            }
            else
            {
                gptimer_disable(p_timer->timer);
                timer_hal_stop(p_timer);
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
            ESP_ERROR_CHECK(gptimer_start(p_timer->timer));
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
            ESP_ERROR_CHECK(gptimer_stop(p_timer->timer));
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
            ESP_ERROR_CHECK(gptimer_set_raw_count(p_timer->timer, 0));
        }
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
