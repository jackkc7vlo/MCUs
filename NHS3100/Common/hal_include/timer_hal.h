/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software ***********************
 * @file       timer_hal.h
 * @author     Jack Wilson
 * @brief      defines timer interface
 *
 * @note
 *
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-08-2023     Jack Wilson                     jackkc7vlo@gmail.com
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
 * ****************************(C) COPYRIGHT 2023 Old Man Software **************
 */

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef TIMER_HAL_H_
#define TIMER_HAL_H_

/********************************************************************************
 * Includes
 ********************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <hw_config.h>

#if USE_TIMERS == 1u
/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs & Enums
 ********************************************************************************/

//<! Pointer to an incomplete type (hides implementation)
typedef struct timer_hal_s *p_timer_hal_t;

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/// *****************************************************************************
/// @brief Allocate memory for the timer
/// @param[in]       timer_num The timer index
/// @param[in]       hardware_timer_id The hardware specific id for the timer
/// @param[in]       mseconds The number of milliseconds to set the timer
/// @param[in]       reload true to automatically reload the timer after expiration
/// @param[in]       callback optional function to call when the timer expires
/// @return          uint32_t The clock divisor
/// *****************************************************************************
p_timer_hal_t timer_hal_allocate(const uint32_t timer_num, const uint32_t hardware_timer_id,
                                 const uint32_t mseconds, const bool reload,
                                 void (*callback)(const uint32_t timer_num));

/// *****************************************************************************
/// @brief free the memory for the allocated timer
/// @param[in]       p_timer The timer that was allocated @see timer_hal_allocate
/// *****************************************************************************
void timer_hal_free(const p_timer_hal_t p_timer);

/// *****************************************************************************
/// @brief enable the timer
/// @param[in]       p_timer The timer that was allocated @see timer_hal_allocate
/// *****************************************************************************
void timer_hal_enable(const p_timer_hal_t p_timer, bool enable);

/// *****************************************************************************
/// @brief start the timer
/// @param[in]       p_timer The timer that was allocated @see timer_hal_allocate
/// *****************************************************************************
void timer_hal_start(const p_timer_hal_t p_timer);

/// *****************************************************************************
/// @brief stop the timer
/// @param[in]       p_timer The timer that was allocated @see timer_hal_allocate
/// *****************************************************************************
void timer_hal_stop(const p_timer_hal_t p_timer);

/// *****************************************************************************
/// @brief reset the timer
/// @param[in]       p_timer The timer that was allocated @see timer_hal_allocate
/// *****************************************************************************
void timer_hal_reset(const p_timer_hal_t p_timer);

/// *****************************************************************************
/// @brief get if the timer has been expired
/// @param[in]       p_timer The timer that was allocated @see timer_hal_allocate
/// *****************************************************************************
bool timer_hal_get_overflow(const p_timer_hal_t p_timer);

/// *****************************************************************************
/// @brief reset the timer count
/// @param[in]       p_timer The timer that was allocated @see timer_hal_allocate
/// *****************************************************************************
void timer_hal_reset_count(const p_timer_hal_t p_timer);

#endif /* USE_TIMERS */
#endif /* TIMER_HAL_H_ */