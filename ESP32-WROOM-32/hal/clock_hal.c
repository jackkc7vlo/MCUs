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
#include "freeRTOS\freeRTOS.h"
#include "freeRTOS\task.h"
#include <rom/ets_sys.h>
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*lint -esym(793,__*)*/
// #include <stdint.h>
#include <reg_hal.h>
#include <assert.h>
// #include <stdbool.h>
#include <stdlib.h> /*lint -e129*/
    // #include <efm32pg22c200f512im40.h>
    /********************************************************************************
     * Defines
     ********************************************************************************/
/*lint -e750 -e9059 -e835*/
#define CMU_EN_BIT_POS 0U
#define CMU_EN_BIT_MASK 0x1FU
#define CMU_NO_EN_REG 0u

#define CMU_CLKEN0_EN_REG 1u
#define CMU_CLKEN1_EN_REG 2u
#define CMU_CRYPTOACCCLKCTRL_EN_REG 3u
#define CMU_EN_REG_POS 5u
#define CMU_EN_REG_MASK 0x3u

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    volatile uint32_t loops_per_jiffy = 98400u;
    /********************************************************************************
     * Functions
     ********************************************************************************/

    /********************************************************************************/

    /********************************************************************************/
    /*
    static void clock_hal_init_clocks(void)
    {
        // sli_em_cmu_HFXOSetForceEnable();

        clock_hal_enable(CLOCK_GPIO, (bool)true);
    }
    */

    /********************************************************************************/
    void clock_hal_init(void)
    {
    }

    /*******************************************************************************/
    void clock_hal_set_divisor(const uint32_t divisor)
    {
        (void)divisor;
        assert(false); /*lint !e506 Not implemented yet */
    }

    /********************************************************************************/
    void clock_hal_set_clock_frequency(const uint32_t frequency)
    {
        (void)frequency;
        assert(false); /*lint !e506*/
        /* Divide-by-0 NOT allowed */
    }

    /********************************************************************************/
    uint32_t clock_hal_get_divisor(void)
    {
        return 1u;
    }

    /********************************************************************************/
    uint32_t clock_hal_get_freq(void)
    {
        const uint32_t ret = ets_get_cpu_frequency();

        return ret;
    }

    /********************************************************************************/
    static void clock_hal_delay_us(const uint32_t us)
    {
        ets_delay_us(us);

    } /*lint !e715*/

    void clock_hal_delay(const uint32_t ms)
    {
        vTaskDelay(ms / portTICK_PERIOD_MS);
        /*for (uint32_t i = 0u; i < ms; i++)
        {
            clock_hal_delay_us(1000u);
        } */
    }

    /********************************************************************************/
    void clock_hal_enable(const uint32_t clock, const bool enable)
    {
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
