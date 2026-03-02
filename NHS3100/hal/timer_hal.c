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

/********************************************************************************
 * Includes
 ********************************************************************************/

#include "timer_hal.h"
#include "clock_hal.h"
#include "device_ct16b.h"
#include "device_irq.h"
#include "device_syscon.h"
#include <hw_config.h>
#include <stdbool.h>
#include <stddef.h> /*lint -e129*/
#include <stdint.h> /*lint -e129*/

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
    // volatile TIMER_TypeDef *timer;             /*!< pointer to the timer configuration
    // registers */
    uint32_t mseconds;          /*!< timer period in milliseconds */
    bool     time_out;          /*!< flag set when the timer completes */
    uint32_t hardware_timer_id; /*!< the hardware timer id */

    timer_callback_context_t callback; /**< Registered interrupt callbacks */
};

static timer_hal_t timer_hal_timers[NUM_TIMERS] = {0};
/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * @brief          Initialization function for the I2C driver
 * @retval         None
 ********************************************************************************/
void timer_hal_initialize(void)
{
    if (!timers_initialized)
    {
        /* Enable CT16B peripheral clock in SYSCON AHB clock control register */
        p_syscon->ahbclkctrl |= SYSCON_AHBCLKCTRL_CT16B_BIT;

        /* Configure prescaler so TC increments at 1 kHz (1 tick = 1 ms).
         * PR = (PCLK / 1000) - 1  (fits in 16 bits for PCLK <= 65.535 MHz) */
        uint32_t pclk = clock_hal_get_timer_freq();
        p_ct16b0->pr  = (pclk / 1000U) - 1U;

        /* Start the counter – TC now free-runs, wrapping at 0xFFFF */
        p_ct16b0->tcr = CT16B_TCR_ENABLE_BIT;

        /* Enable CT16B0 interrupt in the NVIC */
        NVIC_ClearPendingIRQ(CT16B0_IRQn);
        NVIC_EnableIRQ(CT16B0_IRQn);

        timers_initialized = true;
    }
}

/********************************************************************************
 * @brief          Check if the I2C driver is initialized
 * @retval         true if initialized, false otherwise
 ********************************************************************************/
bool timer_hal_is_initialized(void)
{
    /* Placeholder implementation */
    return timers_initialized;
}

/*****************************************************************************
 * @brief Create a timer instance
 *
 * @details Allocates or returns a handle representing a timer configured with
 * the specified parameters. The returned handle is used with all other HAL
 * functions to interact with the timer.
 *
 * @param[in]       mseconds The number of milliseconds to set the timer
 * @param[in]       reload true to automatically reload the timer after expiration
 * @param[in]       callback optional function to call when the timer expires
 * @return Pointer to a `timer_hal_t` instance on success, or NULL on failure.
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
            p_free_slot                    = &timer_hal_timers[i];
            p_free_slot->in_use            = true;
            p_free_slot->mseconds          = mseconds;
            p_free_slot->auto_reload       = reload;
            p_free_slot->callback.callback = callback;
            p_free_slot->hardware_timer_id = i; /* MR0 for timer 0, MR1 for timer 1 */
            p_free_slot->enabled           = false;
            p_free_slot->time_out          = false;
        }
    }
    return p_free_slot;
}

/******************************************************************************
 * @brief enable the timer
 * @param[in]       p_timer The timer that was created @see timer_hal_create
 *****************************************************************************/
void timer_hal_enable(const p_timer_hal_t p_timer, bool enable)
{
    if (p_timer == NULL)
    {
        return;
    }

    if (enable)
    {
        /* Re-arm the match register relative to the current TC value */
        uint32_t id       = p_timer->hardware_timer_id;
        uint32_t match    = (p_ct16b0->tc + p_timer->mseconds) & CT16B_MR_MASK;
        p_ct16b0->mrs[id] = match;

        /* Enable the interrupt for this match channel */
        if (id == 0U)
        {
            p_ct16b0->mcr |= CT16B_MCR_MR0I_BIT;
        }
        else if (id == 1U)
        {
            p_ct16b0->mcr |= CT16B_MCR_MR1I_BIT;
        }
        else
        { /* not reached for NUM_TIMERS == 2 */
        }

        p_timer->enabled  = true;
        p_timer->time_out = false;
    }
    else
    {
        /* Disable the interrupt for this match channel */
        uint32_t id = p_timer->hardware_timer_id;
        if (id == 0U)
        {
            p_ct16b0->mcr &= ~CT16B_MCR_MR0I_BIT;
        }
        else if (id == 1U)
        {
            p_ct16b0->mcr &= ~CT16B_MCR_MR1I_BIT;
        }
        else
        { /* not reached for NUM_TIMERS == 2 */
        }

        p_timer->enabled = false;
    }
}

/******************************************************************************
 * @brief start the timer
 * @param[in]       p_timer The timer that was created @see timer_hal_create
 *****************************************************************************/
void timer_hal_start(const p_timer_hal_t p_timer)
{
    if (p_timer == NULL)
    {
        return;
    }

    /* Program match register relative to the current TC value, then enable
     * the match interrupt for this channel */
    uint32_t id       = p_timer->hardware_timer_id;
    uint32_t match    = (p_ct16b0->tc + p_timer->mseconds) & CT16B_MR_MASK;
    p_ct16b0->mrs[id] = match;

    /* Clear any stale interrupt flag before arming */
    p_ct16b0->ir = (CT16B_IR_MR0INT_BIT << id);

    if (id == 0U)
    {
        p_ct16b0->mcr |= CT16B_MCR_MR0I_BIT;
    }
    else if (id == 1U)
    {
        p_ct16b0->mcr |= CT16B_MCR_MR1I_BIT;
    }
    else
    { /* not reached for NUM_TIMERS == 2 */
    }

    p_timer->enabled  = true;
    p_timer->time_out = false;
}

/******************************************************************************
 * @brief stop the timer
 * @param[in]       p_timer The timer that was created @see timer_hal_create
 *****************************************************************************/
void timer_hal_stop(const p_timer_hal_t p_timer)
{
    if (p_timer == NULL)
    {
        return;
    }

    /* Disable the match interrupt for this channel */
    uint32_t id = p_timer->hardware_timer_id;
    if (id == 0U)
    {
        p_ct16b0->mcr &= ~CT16B_MCR_MR0I_BIT;
    }
    else if (id == 1U)
    {
        p_ct16b0->mcr &= ~CT16B_MCR_MR1I_BIT;
    }
    else
    { /* not reached for NUM_TIMERS == 2 */
    }

    p_timer->enabled = false;
}

/******************************************************************************
 * @brief reset the timer
 * @param[in]       p_timer The timer that was created @see timer_hal_create
 *****************************************************************************/
void timer_hal_reset(const p_timer_hal_t p_timer)
{
    if (p_timer == NULL)
    {
        return;
    }

    /* Stop the timer, clear the timeout flag, and re-arm from the current
     * TC position so it behaves as if freshly started */
    timer_hal_stop(p_timer);
    p_timer->time_out = false;
    timer_hal_start(p_timer);
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
    if (p_timer == NULL)
    {
        return;
    }
    /* Clear the software timeout flag without disturbing the hardware TC,
     * which is shared between all active timer instances */
    p_timer->time_out = false;
}

/******************************************************************************
 * @brief CT16B0 interrupt service routine
 *
 * @details Handles match events for all active timer instances.  On each
 * match the interrupt flag is cleared, the timeout flag is set, the
 * registered callback is invoked (if any), and the match register is
 * advanced for auto-reload timers.  One-shot timers have their match
 * interrupt disabled after the first expiry.
 *****************************************************************************/
void CT16B0_IRQHandler(void)
{
    uint32_t ir = p_ct16b0->ir; /* snapshot pending interrupt flags */

    for (uint32_t i = 0U; i < NUM_TIMERS; i++)
    {
        if (!timer_hal_timers[i].in_use || !timer_hal_timers[i].enabled)
        {
            continue;
        }

        uint32_t id     = timer_hal_timers[i].hardware_timer_id;
        uint32_t ir_bit = CT16B_IR_MR0INT_BIT << id; /* MR0INT << 0 or << 1 */

        if ((ir & ir_bit) == 0U)
        {
            continue;
        }

        /* Acknowledge the interrupt by writing 1 to the flag bit */
        p_ct16b0->ir = ir_bit;

        /* Signal expiry to the software layer */
        timer_hal_timers[i].time_out = true;

        if (timer_hal_timers[i].auto_reload)
        {
            /* Advance match by one period for the next expiry */
            p_ct16b0->mrs[id] = (p_ct16b0->mrs[id] + timer_hal_timers[i].mseconds) & CT16B_MR_MASK;
        }
        else
        {
            /* One-shot: disable the match interrupt */
            if (id == 0U)
            {
                p_ct16b0->mcr &= ~CT16B_MCR_MR0I_BIT;
            }
            else if (id == 1U)
            {
                p_ct16b0->mcr &= ~CT16B_MCR_MR1I_BIT;
            }
            else
            { /* not reached for NUM_TIMERS == 2 */
            }

            timer_hal_timers[i].enabled = false;
        }

        /* Invoke the registered callback, if any */
        if (timer_hal_timers[i].callback.callback != NULL)
        {
            timer_hal_timers[i].callback.callback(timer_hal_timers[i].callback.p_callback_handle,
                                                  timer_hal_timers[i].callback.callback_context);
        }
    }
}
