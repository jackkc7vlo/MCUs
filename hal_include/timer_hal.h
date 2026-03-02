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
 * Multiple include protection
 ********************************************************************************/
#ifndef TIMER_HAL_H
#define TIMER_HAL_H

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h> /*lint -e129*/

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /**
     * @typedef timer_hal_callback_t
     * @brief Prototype for timer callbacks.
     *
     * Implementations invoke the registered callback when a platform-specific
     * timer timeout occurs. The @p handle argument points to the same context
     * passed during registration, allowing the callback to access hardware state.
     */
    typedef void (*timer_hal_callback_t)(void *handle, void *callback_context);
    /* Opaque timer_hal type: definition is private and resides in the .c
     * implementation file. Consumers should only use the pointer type. */
    typedef struct timer_hal  timer_hal_t;
    typedef struct timer_hal *p_timer_hal_t;

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /********************************************************************************
     * @brief          Initialization function for the I2C driver
     * @retval         None
     ********************************************************************************/
    void timer_hal_initialize(void);

    /********************************************************************************
     * @brief          Check if the I2C driver is initialized
     * @retval         true if initialized, false otherwise
     ********************************************************************************/
    bool timer_hal_is_initialized(void);

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
    p_timer_hal_t timer_hal_create(const uint32_t mseconds, const bool reload, timer_hal_callback_t callback);

    /******************************************************************************
     * @brief enable the timer
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    void timer_hal_enable(const p_timer_hal_t p_timer, bool enable);

    /******************************************************************************
     * @brief start the timer
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    void timer_hal_start(const p_timer_hal_t p_timer);

    /******************************************************************************
     * @brief stop the timer
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    void timer_hal_stop(const p_timer_hal_t p_timer);

    /******************************************************************************
     * @brief reset the timer
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    void timer_hal_reset(const p_timer_hal_t p_timer);

    /******************************************************************************
     * @brief get if the timer has been expired
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    bool timer_hal_get_overflow(const p_timer_hal_t p_timer);

    /******************************************************************************
     * @brief reset the timer count
     * @param[in]       p_timer The timer that was created @see timer_hal_create
     *****************************************************************************/
    void timer_hal_reset_count(const p_timer_hal_t p_timer);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* TIMER_HAL_H */