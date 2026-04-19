/**
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************
 * @file       drv_st7735.h
 * @author     Jack Wilson
 * @brief      Driver for the ST7735 LCD controller
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Jan-19-2024     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 * Besed on code from Alexsander Alekseev containing the following copywrite:
 * MIT License
 *
 * Copyright (c) 2018 Aleksander Alekseev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************/
/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef drv_st7735_h_
#define drv_st7735_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#if HAS_ST7735 == 1u
#include "drv_fonts.h"
#include "hw_config.h"
#include <gpio_hal.h>
#include <spi_hal.h>
#include <stdbool.h>
#include <stdint.h>
    /********************************************************************************
     * Defines
     ********************************************************************************/
#define ST7735_MADCTL_MY 0x80
#define ST7735_MADCTL_MX 0x40
#define ST7735_MADCTL_MV 0x20
#define ST7735_MADCTL_ML 0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH 0x04

    /****************************/

#define ST7735_NOP 0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN 0x10
#define ST7735_SLPOUT 0x11
#define ST7735_PTLON 0x12
#define ST7735_NORON 0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_GAMSET 0x26
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_PTLAR 0x30
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36

#define ST7735_RAMCTL 0xB0
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define ST7735_BLACK 0x0000
#define ST7735_BLUE 0x001F
#define ST7735_RED 0xF800
#define ST7735_GREEN 0x07E0
#define ST7735_CYAN 0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW 0xFFE0
#define ST7735_WHITE 0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    typedef enum
    {
        GAMMA_10 = 0x01,
        GAMMA_25 = 0x02,
        GAMMA_22 = 0x04,
        GAMMA_18 = 0x08
    } drv_st7735_gamma_t;

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    // call before initializing any SPI devices
    void drv_st7735_unselect();

    bool drv_st7735_init(p_spi_hal_t spi_device_handle, p_gpio_hal_t cs_gpio_handle, p_gpio_hal_t reset_gpio_handle,
                         p_gpio_hal_t dc_gpio_handle, p_gpio_hal_t bckl_gpio_handle);
    void drv_st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
    void drv_st7735_write_string(uint16_t x, uint16_t y, const char *str, drv_font_def_t font, uint16_t color,
                                 uint16_t bgcolor);
    void drv_st7735_fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void drv_st7735_fill_rectangle_fast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void drv_st7735_fill_screen(uint16_t color);
    void drv_st7735_fill_screen_fast(uint16_t color);
    void drv_st7735_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
    void drv_st7735_invert_colors(bool invert);
    void drv_st7735_set_gamma(drv_st7735_gamma_t gamma);

#if UNIT_TESTS == 1u
    void drv_st7735_unit_test(void);
#endif

#endif // HAS_ST7735
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* drv_st7735.h_ */
