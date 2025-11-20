/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software ***********************
 * @file       timer_hal.c
 * @author     Jack Wilson
 * @brief
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-10-2023     Jack Wilson                     jackkc7vlo@gmail.com
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
 * ****************************(C) COPYRIGHT 2023 Old Man Software ***********************/

/********************************************************************************
 * Includes
 *********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*lint -esym(793,__*)*/
#include <timer_hal.h>
#include <clock_hal.h>
#include <stdlib.h> /*lint -e129*/
#include "reg_hal.h"
#include <assert.h>
#//include <cmsis.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /**
     * @brief A structure to repesent a hardware timer
     * @retval
     */
    typedef struct timer_hal_s
    {
        uint32_t timer_num;                   /*!< timer index */
        bool enabled;                         /*!< timer enabled */
        bool auto_reload;                     /*!< timer auto reload */
        uint32_t prescaler;                   /*!< timer prescaler */
        uint32_t mseconds;                    /*!< timer period in seconds */
        NSS_TIMER_T *timer;                   /*!< pointer to the timer configuration registers */
        bool time_out;                        /*!< flag set when the timer completes */
        uint32_t hardware_timer_id;           /*!< the hardware timer id */
        void (*callback)(uint32_t timer_num); /*!< time interrupt callback */
    } timer_hal_t;

    /* TODO make this a linked list*/
    static volatile timer_hal_t *p_hal_timers[TIMER_HAL_MAX_TIMERS] = {NULL}; /*lint !e956*/
    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    static void timer_hal_init(const p_timer_hal_t p_timer); // forward reference

    p_timer_hal_t timer_hal_allocate(const uint32_t timer_num, const uint32_t hardware_timer_id, const uint32_t mseconds, const bool reload, void (*callback)(const uint32_t timer_num))
    {
        if (hardware_timer_id >= TIMER_HAL_MAX_TIMERS)
        {
            return NULL;
        }
        p_timer_hal_t p_timer = malloc(sizeof(timer_hal_t));

        if (p_timer != NULL)
        {
            p_timer->timer_num = timer_num;
            p_timer->hardware_timer_id = hardware_timer_id;
            p_timer->callback = callback;
            p_timer->auto_reload = reload;
            p_timer->mseconds = mseconds;
            if (hardware_timer_id == 0u)
            {
                p_timer->timer = NSS_TIMER16_0;
            }
            else
            {
                p_timer->timer = NSS_TIMER32_0;
            }
            p_hal_timers[timer_num] = p_timer;

            timer_hal_init(p_timer);
        }
        return p_timer;
    } /*lint !e953 !e952*/

    void timer_hal_free(const p_timer_hal_t p_timer)
    {
        if (p_timer != NULL)
        {

            timer_hal_stop(p_timer);
            p_hal_timers[p_timer->timer_num] = NULL;
            free(p_timer);
        }
    } /*lint !e818 */

    void timer_hal_reset(const p_timer_hal_t p_timer)
    {
        uint32_t reg;

        /* Disable timer, set terminal count to non-0 */
        reg = p_timer->timer->TCR;
        p_timer->timer->TCR = 0u;
        p_timer->timer->TC = 1u;

        /* Reset timer counter */
        p_timer->timer->TCR = ((1u << 1u));

        while (p_timer->timer->TC != 0u)
        {
            /* Wait for terminal count to clear */
        }

        /* Restore timer state */
        p_timer->timer->TCR = reg;
        // p_timer->timer->CMD = TIMER_CMD_CLEAR;
    } /*lint !e818 */

    static void timer_hal_init(const p_timer_hal_t p_timer)
    {

        // enable the 16 bit timer clock interrupt

        // set up the prescaler
        const uint32_t prescale = (clock_hal_get_freq() / 100u) - 1u;
        p_timer->timer->PR = prescale;

        // Set up Match Register 0

        // right now the prescaler is set 10s of milliseconds so we need to deal with that
        const uint32_t mseconds = p_timer->mseconds / 10u;
        // set the match register
        p_timer->timer->MR[0] = mseconds; // - 1u;
        // Enable the interrupt for the match register 0
        // p_timer->timer->MCR |= (1u << ((0u) * 3u));
        // Stop on Match Disable
        p_timer->timer->MCR &= ~(2u);
        // Reset on Match Enable
        p_timer->timer->MCR |= (1u);

        // Dont use Match Register 1

        // disable the interrupt for Match Register 1
        p_timer->timer->MCR &= ~(1u << ((1u) * 3u));
        p_timer->timer->MCR &= ~(1u << (((1u) * 3u) + 2u));
        p_timer->timer->MCR &= ~(1u << (((1u) * 3u) + 1u));

        // disable the interrupt for Match Register 2
        p_timer->timer->MCR &= ~(1u << ((2u) * 3u));
        p_timer->timer->MCR &= ~(1u << (((2u) * 3u) + 2u));
        p_timer->timer->MCR &= ~(1u << (((2u) * 3u) + 1u));

        // disable the interrupt for Match Register 3
        p_timer->timer->MCR &= ~(1u << ((3u) * 3u));
        p_timer->timer->MCR &= ~(1u << (((3u) * 3u) + 2u));
        p_timer->timer->MCR &= ~(1u << (((3u) * 3u) + 1u));

        p_timer->time_out = false;
        timer_hal_reset(p_timer);
        // p_timer->timer->TCR |= ((1u << 0));
    } /*lint !e818 */

    void timer_hal_enable(const p_timer_hal_t p_timer, const bool enable)
    {
        if (enable)
        {
            timer_hal_start(p_timer);
        }
        else
        {
            timer_hal_stop(p_timer);
        }

    } /*lint !e818 */

    void timer_hal_start(const p_timer_hal_t p_timer)
    {
        NVIC_EnableIRQ(CT16B0_IRQn);
        p_timer->timer->TCR |= 0x01u;
    } /*lint !e818 */

    void timer_hal_stop(const p_timer_hal_t p_timer)
    {
        NVIC_DisableIRQ(CT16B0_IRQn);
        p_timer->timer->TCR &= ~0x01u;
    } /*lint !e818 */

    bool timer_hal_get_overflow(const p_timer_hal_t p_timer)
    {
        return p_timer->time_out;
    } /*lint !e818 */

    void timer_hal_reset_count(const p_timer_hal_t p_timer)
    {
        p_timer->time_out = false;
    } /*lint !e818 */

    void CT16B0_IRQHandler(void)
    {
        if (((NSS_TIMER16_0->IR & 0x01u) != 0u))
        {
            NSS_TIMER16_0->IR = 1u;

            ((timer_hal_t *)p_hal_timers[0])->time_out = true;

            if (((timer_hal_t *)p_hal_timers[0])->callback != NULL)
            {
                ((timer_hal_t *)p_hal_timers[0])->callback(((timer_hal_t *)p_hal_timers[0])->timer_num);
            }
        }
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
