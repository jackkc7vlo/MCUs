/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       clock_hal.h
 * @author     Jack Wilson
 * @brief      Contains the clock hardware abstraction layer.  This is the lowest
 *             level of the clock driver and is responsible for initializing the
 *             clock and providing the clock frequency and period.
 *
 *
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
#ifndef clock_hal_h_
#define clock_hal_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdint.h>
#include <stdbool.h>
#include <hw_config.h>
    /********************************************************************************
     * Defines
     ********************************************************************************/
#define CLOCK_GPIO 58u

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /// *****************************************************************************
    /// @brief  Initialize the clock before doing almost anything else.
    ///
    /// This function initializes the clock.  It should be called before any other
    /// clock functions are called.
    ///
    /// @retval none
    /// *****************************************************************************
    void clock_hal_init(void);

    /// *****************************************************************************
    /// @brief Get the clock divisor.
    /// @return          uint32_t The clock divisor
    /// *****************************************************************************
    uint32_t clock_hal_get_divisor(void);

    /// *****************************************************************************
    /// @brief Get the clock frequency.
    /// @return          uint32_t The clock frequency in Hz
    /// *****************************************************************************
    uint32_t clock_hal_get_freq(void);

    /// *****************************************************************************
    /// @brief Delay for the specified number of milliseconds.
    /// @param[in]       ms The number of milliseconds to delay
    /// @return          none
    /// *****************************************************************************
    void clock_hal_delay(const uint32_t ms);

    /// *****************************************************************************
    /// @brief Delay for the specified number of microseconds.
    /// @param[in]       us The number of microseconds to delay
    /// @return          none
    /// *****************************************************************************
    void clock_hal_delay_us(const uint32_t us);

    /// *****************************************************************************
    /// @brief Set the clock frequency.
    /// @param[in]       frequency The clock frequency in Hz
    /// @return          none
    /// *****************************************************************************
    void clock_hal_set_clock_frequency(const uint32_t frequency);

    /// *****************************************************************************
    /// @brief Set the clock divisor.
    /// @param[in]       divisor The clock divisor
    /// @return          none
    /// *****************************************************************************
    void clock_hal_set_divisor(const uint32_t divisor);

    /// *****************************************************************************
    /// @brief Enable or disable a clock.
    /// @param[in]       clock The clock to enable or disable
    /// @param[in]       enable True to enable, false to disable
    /// @return          none
    /// *****************************************************************************
    void clock_hal_enable(const uint32_t clock, const bool enable);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* clock_hal_h_ */
