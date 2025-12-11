/**
 * ****************************(C) COPYRIGHT 2024 Old Man Software
 * *****************
 * @file       drv_ili9341.h
 * @author     Jack Wilson
 * @brief      Ili9341 driver header
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Jan-24-2024     Jack Wilson jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 * Based on STM Hal driver for ili9341
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
 * ****************************(C) COPYRIGHT 2024 Old Man Software
 * *****************/

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef drv_ili9341_h_
#define drv_ili9341_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>

#if HAS_ILI9341 == 1u
#include "gpio_hal.h"
#include "spi_hal.h"

/********************************************************************************
 * Defines
 ********************************************************************************/
#define ILI9341_BLACK 0x0000u
#define ILI9341_NAVY 0x000Fu
#define ILI9341_DARKGREEN 0x03E0u
#define ILI9341_DARKCYAN 0x03EFu
#define ILI9341_MAROON 0x7800u
#define ILI9341_PURPLE 0x780Fu
#define ILI9341_OLIVE 0x7BE0u
#define ILI9341_LIGHTGREY 0xC618u
#define ILI9341_DARKGREY 0x7BEFu
#define ILI9341_BLUE 0x001Fu
#define ILI9341_GREEN 0x07E0u
#define ILI9341_CYAN 0x07FFu
#define ILI9341_RED 0xF800u
#define ILI9341_MAGENTA 0xF81Fu
#define ILI9341_YELLOW 0xFFE0u
#define ILI9341_WHITE 0xFFFFu
#define ILI9341_ORANGE 0xFD20u
#define ILI9341_GREENYELLOW 0xAFE5u
#define ILI9341_PINK 0xF81Fu

#define swap(a, b)                                                                                                     \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a         = b;                                                                                                 \
        b         = t;                                                                                                 \
    }
#define BURST_MAX_SIZE 500
    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    bool drv_ili9341_init(p_spi_hal_device_t spi_device_handle, p_gpio_hal_t cs_gpio_handle,
                          p_gpio_hal_t reset_gpio_handle, p_gpio_hal_t dc_gpio_handle, p_gpio_hal_t bckl_gpio_handle);
    void drv_ili9341_reset(void);
    void drv_ili9341_set_rotation(uint8_t rotation);
    void drv_ili9341_fill_screen(uint32_t color);

    void drv_ili9341_draw_pixel(uint32_t x, uint32_t y, uint32_t color);
    void drv_ili9341_draw_double_pixel(uint32_t x, uint32_t y, uint32_t color1, uint32_t color2);
    void drv_ili9341_draw_circle(uint32_t x0, uint32_t y0, int32_t r, uint32_t color, uint8_t flood);
    void drv_ili9341_draw_line(uint32_t color, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void drv_ili9341_draw_filled_rect(uint32_t color, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
    void drv_ili9341_draw_empty_rect(uint32_t color, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
    void drv_ili9341_draw_string(uint32_t x, uint32_t y, uint32_t color, uint32_t phone, char *str, uint8_t size);

#if UNIT_TESTS == 1u
    void drv_ili9341_unit_test(void);
#endif
#endif /* HAS_ILI9341 */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* drv_ili9341.h_ */
