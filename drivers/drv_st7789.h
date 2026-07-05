/**
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************
 * @file       drv_st7789.h
 * @author     Jack Wilson
 * @brief      Driver for the ST7789 LCD controller
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Jul-05-2026     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 * Based on ST7735 driver and adapted for ST7789 TFT controller
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
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************/
/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef drv_st7789_h_
#define drv_st7789_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#if HAS_ST7789 == 1u
#include "drv_fonts.h"
#include "hw_config.h"
#include <gpio_hal.h>
#include <spi_hal.h>
#include <stdbool.h>
#include <stdint.h>
    /********************************************************************************
     * Defines
     ********************************************************************************/
#define ST7789_MADCTL_MY 0x80
#define ST7789_MADCTL_MX 0x40
#define ST7789_MADCTL_MV 0x20
#define ST7789_MADCTL_ML 0x10
#define ST7789_MADCTL_RGB 0x00
#define ST7789_MADCTL_BGR 0x08
#define ST7789_MADCTL_MH 0x04

    /****************************/

#define ST7789_NOP 0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID 0x04
#define ST7789_RDDST 0x09

#define ST7789_SLPIN 0x10
#define ST7789_SLPOUT 0x11
#define ST7789_PTLON 0x12
#define ST7789_NORON 0x13

#define ST7789_INVOFF 0x20
#define ST7789_INVON 0x21
#define ST7789_GAMSET 0x26
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON 0x29
#define ST7789_CASET 0x2A
#define ST7789_RASET 0x2B
#define ST7789_RAMWR 0x2C
#define ST7789_RAMRD 0x2E

#define ST7789_PTLAR 0x30
#define ST7789_COLMOD 0x3A
#define ST7789_MADCTL 0x36

#define ST7789_RAMCTL 0xB0
#define ST7789_FRMCTR1 0xB1
#define ST7789_FRMCTR2 0xB2
#define ST7789_FRMCTR3 0xB3
#define ST7789_INVCTR 0xB4
#define ST7789_DISSET5 0xB6

#define ST7789_PWCTR1 0xC0
#define ST7789_PWCTR2 0xC1
#define ST7789_PWCTR3 0xC2
#define ST7789_PWCTR4 0xC3
#define ST7789_PWCTR5 0xC4
#define ST7789_VMCTR1 0xC5
#define ST7789_PORCTRL 0xB2
#define ST7789_GCTRL 0xB7
#define ST7789_VCOMS 0xBB
#define ST7789_LCMCTRL 0xC0
#define ST7789_VDVVRHEN 0xC2
#define ST7789_VRHS 0xC3
#define ST7789_VDVS 0xC4
#define ST7789_FRCTRL2 0xC6
#define ST7789_PWCTRL1 0xD0

#define ST7789_RDID1 0xDA
#define ST7789_RDID2 0xDB
#define ST7789_RDID3 0xDC
#define ST7789_RDID4 0xDD

#define ST7789_PWCTR6 0xFC

#define ST7789_GMCTRP1 0xE0
#define ST7789_GMCTRN1 0xE1

// Color definitions
#define ST7789_BLACK 0x0000
#define ST7789_BLUE 0x001F
#define ST7789_RED 0xF800
#define ST7789_GREEN 0x07E0
#define ST7789_CYAN 0x07FF
#define ST7789_MAGENTA 0xF81F
#define ST7789_YELLOW 0xFFE0
#define ST7789_WHITE 0xFFFF
#define ST7789_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    typedef enum
    {
        GAMMA_10 = 0x01,
        GAMMA_25 = 0x02,
        GAMMA_22 = 0x04,
        GAMMA_18 = 0x08
    } drv_st7789_gamma_t;

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    // call before initializing any SPI devices
    void drv_st7789_unselect();

    bool drv_st7789_init(p_spi_hal_t spi_device_handle, p_gpio_hal_t cs_gpio_handle, p_gpio_hal_t reset_gpio_handle,
                         p_gpio_hal_t dc_gpio_handle, p_gpio_hal_t bckl_gpio_handle);
    void drv_st7789_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
    void drv_st7789_write_string(uint16_t x, uint16_t y, const char *str, drv_font_def_t font, uint16_t color,
                                 uint16_t bgcolor);
    void drv_st7789_fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void drv_st7789_fill_rectangle_fast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void drv_st7789_fill_screen(uint16_t color);
    void drv_st7789_fill_screen_fast(uint16_t color);
    void drv_st7789_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
    void drv_st7789_invert_colors(bool invert);
    void drv_st7789_set_gamma(drv_st7789_gamma_t gamma);

#if UNIT_TESTS == 1u
    void drv_st7789_unit_test(void);
#endif

#endif // HAS_ST7789
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* drv_st7789.h_ */
