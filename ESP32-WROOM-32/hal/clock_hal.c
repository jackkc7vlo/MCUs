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
 * @file clock_hal.h
 * @brief Generic clock hal implementation.
 * @details Outlines the portable clock abstraction used to configure the clock and
 *           provide timing functions.  This is a very basic implementation that is
 *
 * @author  Jack Wilson
 * @date    February 2, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <assert.h>
#include <clock_hal.h>
#include <esp_clk_tree.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rom/ets_sys.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/
#define TAG "clock_hal"

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Functions
     ********************************************************************************/

    /**
     * @brief  Initialize the clock before doing almost anything else.
     *
     * This function initializes the clock.  It should be called before any other
     * clock functions are called.
     *
     * @retval none
     */
    void clock_hal_init(void)
    {
        ESP_LOGI(TAG, "Clock HAL initialized, CPU freq: %lu Hz", (unsigned long)clock_hal_get_freq());
    }

    /**
     * @brief Delay for the specified number of milliseconds.
     * @param[in]       ms The number of milliseconds to delay
     * @return          none
     */
    void clock_hal_delay(const uint32_t ms)
    {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }

    /**
     * @brief Delay for the specified number of microseconds.
     * @param[in]       us The number of microseconds to delay
     * @return          none
     */
    void clock_hal_delay_us(const uint32_t us)
    {
        ets_delay_us(us);
    }

    /**
     * @brief Set the clock divisor.
     * @param[in]       divisor The clock divisor
     * @return          none
     *
     * @note  On ESP32-S3, clock divisors are managed by ESP-IDF internally.
     *        Use menuconfig (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ) to set CPU frequency.
     */
    void clock_hal_set_divisor(uint32_t divisor)
    {
        (void)divisor;
        ESP_LOGW(TAG, "clock_hal_set_divisor: not applicable on ESP32-S3, use menuconfig");
    }

    /**
     * @brief Set the clock frequency.
     * @param[in]       frequency The clock frequency in Hz
     * @return          none
     *
     * @note  On ESP32-S3, CPU frequency is set at build time via menuconfig
     *        (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ). Dynamic frequency scaling
     *        can be enabled via CONFIG_PM_ENABLE.
     */
    void clock_hal_set_clock_frequency(const uint32_t frequency)
    {
        (void)frequency;
        ESP_LOGW(TAG, "clock_hal_set_clock_frequency: not applicable on ESP32-S3, use menuconfig");
    }

    /**
     * @brief Get the clock divisor.
     * @return          uint32_t The clock divisor (always 1 on ESP32-S3)
     */
    uint32_t clock_hal_get_divisor(void)
    {
        return 1u;
    }

    /**
     * @brief Get the clock frequency.
     * @return          uint32_t The clock frequency in Hz
     */
    uint32_t clock_hal_get_freq(void)
    {
        uint32_t freq_hz = 0u;
        esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &freq_hz);
        return freq_hz;
    }

    /**
     * @brief Get the number of nanoseconds per tick.
     * @return          uint32_t The nanoseconds per tick
     */
    uint32_t clock_hal_get_ns_per_tick(void)
    {
        return 1000u * 1000u * 1000u / clock_hal_get_freq();
    }

    /**
     * @brief Enable or disable a clock.
     * @param[in]       clock The clock to enable or disable
     * @param[in]       enable True to enable, false to disable
     * @return          none
     *
     * @note  On ESP32-S3, peripheral clocks are managed by ESP-IDF drivers.
     */
    void clock_hal_enable(const uint32_t clock, const bool enable)
    {
        (void)clock;
        (void)enable;
        ESP_LOGW(TAG, "clock_hal_enable: peripheral clocks managed by ESP-IDF drivers");
    }

    /**
     * @brief Get total milliseconds since system start.
     * @return          uint32_t The total milliseconds since system start
     */
    uint32_t clock_hal_get_milliseconds(void)
    {
        return (uint32_t)(esp_timer_get_time() / 1000ULL);
    }
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */