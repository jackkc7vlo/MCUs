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
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /*lint -e129*/
#if HAS_ILI9341 == 1u

#include "characters.h"
// #include <assert.h>
#include <clock_hal.h>
// #include <drv_gpio.h>
#include <drv_ili9341.h>
#include <gpio_hal.h>
#include <spi_hal.h>
#include <string.h> /*lint -e129*/
#if USE_PRINTF == 1u
#include <printf_hal.h>
#endif
#if UNIT_TESTS == 1u
#include <unittest.h>
#endif
/********************************************************************************
 * Defines
 ********************************************************************************/
#ifndef ILI9341_SPI_CS_PIN
#pragma message "ILI9341_SPI_CS_PIN not defined in hw_config.h"
#endif

#ifndef ILI9341_SPI_RESET_PIN
#pragma message "ILI9341_SPI_RESET_PIN not defined in hw_config.h"
#endif

#ifndef ILI9341_SPI_DC_PIN
#pragma message "ILI9341_SPI_DC_PIN not defined in hw_config.h"
#endif

#ifndef ILI9341_SPI_BCKL_PIN
#pragma message "ILI9341_SPI_BCKL_PIN not defined in hw_config.h"
#endif
    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    // static p_gpio_hal_t p_gpio = gpio_hal_create(0);
    //  p_cs_gpio = NULL;
    //  static p_gpio_def_t p_reset_gpio = NULL;
    //  static p_gpio_def_t p_dc_gpio = NULL;
    //  static p_gpio_def_t p_bckl_gpio = NULL;
    //  static p_spi_hal_def_t p_spi = NULL;

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    static void before_sending_data(void);
    static void before_sending_command(void);
    static void drv_ili9341_send_data(uint8_t data);
    static void drv_ili9341_send_command(uint8_t command);
    static void drv_ili9341_send_burst(uint16_t color, uint32_t len);
    // static void ILI9341_Draw_Double_Pixel(int x, int y, unsigned int color1,
    // unsigned int color2);
    static void drv_ili9341_draw_char(uint32_t x, uint32_t y, uint32_t color, uint32_t phone, uint8_t charcode,
                                      uint8_t size);

    static uint32_t X_SIZE = 240u;
    static uint32_t Y_SIZE = 320u;

    unsigned char hh;

    static p_spi_hal_t  p_spi        = NULL;
    static p_gpio_hal_t p_dc_gpio    = NULL;
    static p_gpio_hal_t p_reset_gpio = NULL;
    static p_gpio_hal_t p_bckl_gpio  = NULL;
    static p_gpio_hal_t p_cs_gpio    = NULL;

    /********************************************************************************
     * Functions
     ********************************************************************************/

    bool drv_ili9341_init(p_spi_hal_t spi_device_handle, p_gpio_hal_t cs_gpio_handle, p_gpio_hal_t reset_gpio_handle,
                          p_gpio_hal_t dc_gpio_handle, p_gpio_hal_t bckl_gpio_handle)
    {

        p_spi        = spi_device_handle;
        p_dc_gpio    = dc_gpio_handle;
        p_reset_gpio = reset_gpio_handle;
        p_bckl_gpio  = bckl_gpio_handle;
        p_cs_gpio    = cs_gpio_handle;

        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                               true); // CS is opposite, so true deselects the display
        }

        if (ILI9341_SPI_RESET_PIN != 0xff)
        {
            gpio_hal_set_state(p_reset_gpio, ILI9341_SPI_RESET_PIN,
                               true); //
            drv_ili9341_reset();
        }

        if (ILI9341_SPI_BCKL_PIN != 0xff)
        {
            gpio_hal_set_state(p_bckl_gpio, ILI9341_SPI_BCKL_PIN,
                               true); //
        }

        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
        // drv_gpio_set_state(p_dc_gpio, false);

        drv_ili9341_send_command(0x01);

        /* Power Control A */
        drv_ili9341_send_command(0xCB);
        drv_ili9341_send_data(0x39);
        drv_ili9341_send_data(0x2C);
        drv_ili9341_send_data(0x00);
        drv_ili9341_send_data(0x34);
        drv_ili9341_send_data(0x02);

        /* Power Control B */
        drv_ili9341_send_command(0xCF);
        drv_ili9341_send_data(0x00);
        drv_ili9341_send_data(0xC1);
        drv_ili9341_send_data(0x30);

        /* Driver timing control A */
        drv_ili9341_send_command(0xE8);
        drv_ili9341_send_data(0x85);
        drv_ili9341_send_data(0x00);
        drv_ili9341_send_data(0x78);

        /* Driver timing control B */
        drv_ili9341_send_command(0xEA);
        drv_ili9341_send_data(0x00);
        drv_ili9341_send_data(0x00);

        /* Power on Sequence control */
        drv_ili9341_send_command(0xED);
        drv_ili9341_send_data(0x64);
        drv_ili9341_send_data(0x03);
        drv_ili9341_send_data(0x12);
        drv_ili9341_send_data(0x81);

        /* Pump ratio control */
        drv_ili9341_send_command(0xF7);
        drv_ili9341_send_data(0x20);

        /* Power Control 1 */
        drv_ili9341_send_command(0xC0);
        drv_ili9341_send_data(0x10);

        /* Power Control 2 */
        drv_ili9341_send_command(0xC1);
        drv_ili9341_send_data(0x10);

        /* VCOM Control 1 */
        drv_ili9341_send_command(0xC5);
        drv_ili9341_send_data(0x3E);
        drv_ili9341_send_data(0x28);

        /* VCOM Control 2 */
        drv_ili9341_send_command(0xC7);
        drv_ili9341_send_data(0x86);

        /* VCOM Control 2 */
        drv_ili9341_send_command(0x36);
        drv_ili9341_send_data(0x48);

        /* Pixel Format Set */
        drv_ili9341_send_command(0x3A);
        drv_ili9341_send_data(0x55); // 16bit

        drv_ili9341_send_command(0xB1);
        drv_ili9341_send_data(0x00);
        drv_ili9341_send_data(0x18);

        /* Display Function Control */
        drv_ili9341_send_command(0xB6);
        drv_ili9341_send_data(0x08);
        drv_ili9341_send_data(0x82);
        drv_ili9341_send_data(0x27);

        /* 3GAMMA FUNCTION DISABLE */
        drv_ili9341_send_command(0xF2);
        drv_ili9341_send_data(0x00);

        /* GAMMA CURVE SELECTED */
        drv_ili9341_send_command(0x26); // Gamma set
        drv_ili9341_send_data(0x01);    // Gamma Curve (G2.2)

        // Positive Gamma  Correction
        drv_ili9341_send_command(0xE0);
        drv_ili9341_send_data(0x0F);
        drv_ili9341_send_data(0x31);
        drv_ili9341_send_data(0x2B);
        drv_ili9341_send_data(0x0C);
        drv_ili9341_send_data(0x0E);
        drv_ili9341_send_data(0x08);
        drv_ili9341_send_data(0x4E);
        drv_ili9341_send_data(0xF1);
        drv_ili9341_send_data(0x37);
        drv_ili9341_send_data(0x07);
        drv_ili9341_send_data(0x10);
        drv_ili9341_send_data(0x03);
        drv_ili9341_send_data(0x0E);
        drv_ili9341_send_data(0x09);
        drv_ili9341_send_data(0x00);

        // Negative Gamma  Correction
        drv_ili9341_send_command(0xE1);
        drv_ili9341_send_data(0x00);
        drv_ili9341_send_data(0x0E);
        drv_ili9341_send_data(0x14);
        drv_ili9341_send_data(0x03);
        drv_ili9341_send_data(0x11);
        drv_ili9341_send_data(0x07);
        drv_ili9341_send_data(0x31);
        drv_ili9341_send_data(0xC1);
        drv_ili9341_send_data(0x48);
        drv_ili9341_send_data(0x08);
        drv_ili9341_send_data(0x0F);
        drv_ili9341_send_data(0x0C);
        drv_ili9341_send_data(0x31);
        drv_ili9341_send_data(0x36);
        drv_ili9341_send_data(0x0F);

        // EXIT SLEEP
        drv_ili9341_send_command(0x11);

        // TURN ON DISPLAY
        drv_ili9341_send_command(0x29);
        drv_ili9341_send_data(0x2C);
        // spi_hal_set_baud(p_spi, 1000000u);
        return true;

        //(p_spi, 1000000u);
        return false;
    }

    void drv_ili9341_spi_send(unsigned char data)
    {
        // HAL_SPI_Transmit(&lcd_spi, &data, 1, 1);
        spi_hal_write(p_spi, &data, 1);
    }

    void drv_ili9341_spi_send_multiple(unsigned char data, int size)
    {
        // HAL_SPI_Transmit(&lcd_spi, &data, size, 10);
        spi_hal_write(p_spi, &data, size);
    }

    void drv_ili9341_spi_send_32(uint8_t command, uint32_t data)
    {
        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                               (bool)false); // CS is opposite, so true deselects the display
        }

        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
        // drv_gpio_set_state(p_dc_gpio, false);       // DC pin is low for commands
        drv_ili9341_spi_send(command);
        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for commands
        // drv_gpio_set_state(p_dc_gpio, true);
        drv_ili9341_spi_send(data >> 24);
        drv_ili9341_spi_send(data >> 16);
        drv_ili9341_spi_send(data >> 8);
        drv_ili9341_spi_send(data);
        // spi_hal_chip_select(p_spi, (bool)false);
    }

    void drv_ili9341_set_address(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
    {
        uint32_t t;
        t = x1;
        t <<= 16;
        t |= x2;
        drv_ili9341_spi_send_32(0x2A, t); // Column Address Set
        t = y1;
        t <<= 16;
        t |= y2;
        drv_ili9341_spi_send_32(0x2B, t); // Page Address Set
    }

    void drv_ili9341_reset(void)
    {
        if (ILI9341_SPI_RESET_PIN != 0xff)
        {
            gpio_hal_set_state(p_reset_gpio, ILI9341_SPI_RESET_PIN,
                               false); //
            // drv_gpio_set_state(p_reset_gpio, false); // low resets it
            clock_hal_delay(50u);
            gpio_hal_set_state(p_reset_gpio, ILI9341_SPI_RESET_PIN, true);
            // drv_gpio_set_state(p_reset_gpio, true);
        }
        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                               (bool)false); // CS is opposite, so true deselects the display
        }
        // drv_gpio_set_state(p_cs_gpio, (bool)false); // Selects the display
        clock_hal_delay(50u);

        drv_ili9341_send_command(0x01);
        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                               (bool)true); // CS is opposite, so true deselects the display
        }
        // drv_gpio_set_state(p_cs_gpio, (bool)true); // deselects the display
    }

    void drv_ili9341_set_rotation(uint8_t rotation)
    {
        drv_ili9341_send_command(0x36);
        switch (rotation)
        {
        case 0:
            drv_ili9341_send_data(0x48);
            X_SIZE = 240;
            Y_SIZE = 320;
            break;
        case 1:
            drv_ili9341_send_data(0x28);
            X_SIZE = 320;
            Y_SIZE = 240;
            break;
        case 2:
            drv_ili9341_send_data(0x88);
            X_SIZE = 240;
            Y_SIZE = 320;
            break;
        case 3:
            drv_ili9341_send_data(0xE8);
            X_SIZE = 320;
            Y_SIZE = 240;
            break;
        }
    }

    void drv_ili9341_fill_screen(uint32_t color)
    {
        drv_ili9341_set_address(0, 0, X_SIZE - 1, Y_SIZE - 1);
        drv_ili9341_send_burst(color, (long)X_SIZE * (long)Y_SIZE);
    }

    void drv_ili9341_draw_pixel(uint32_t x, uint32_t y, uint32_t color)
    {

        if ((x >= X_SIZE) || (y >= Y_SIZE))
        {

            return;
        }
        drv_ili9341_set_address(x, y, X_SIZE - 1u, Y_SIZE - 1u);
        // drv_gpio_set_state(p_dc_gpio, false); // DC pin is low for commands;
        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
        drv_ili9341_spi_send(0x2C);
        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for data
        // drv_gpio_set_state(p_dc_gpio, true); // DC pin is high for data
        drv_ili9341_spi_send(color >> 8);
        drv_ili9341_spi_send(color);
    }

    void drv_ili9341_draw_double_pixel(uint32_t x, uint32_t y, uint32_t color1, uint32_t color2)
    {
        (void)x;
        (void)y;
        (void)color1;
        (void)color2;
    }

    void drv_ili9341_draw_circle(uint32_t x0, uint32_t y0, int32_t r, uint32_t color, uint8_t flood)
    {
        int f     = 1 - r;
        int ddF_x = 1;
        int ddF_y = -2 * r;
        int x     = 0;
        int y     = r;
        if (flood == 0)
        {
            drv_ili9341_draw_pixel(x0, y0 + r, color);
            drv_ili9341_draw_pixel(x0, y0 - r, color);
            drv_ili9341_draw_pixel(x0 + r, y0, color);
            drv_ili9341_draw_pixel(x0 - r, y0, color);
            while (x < y)
            {
                if (f >= 0)
                {
                    y--;
                    ddF_y += 2;
                    f += ddF_y;
                }
                x++;
                ddF_x += 2;
                f += ddF_x;
                drv_ili9341_draw_pixel(x0 + x, y0 + y, color);
                drv_ili9341_draw_pixel(x0 - x, y0 + y, color);
                drv_ili9341_draw_pixel(x0 + x, y0 - y, color);
                drv_ili9341_draw_pixel(x0 - x, y0 - y, color);
                drv_ili9341_draw_pixel(x0 + y, y0 + x, color);
                drv_ili9341_draw_pixel(x0 - y, y0 + x, color);
                drv_ili9341_draw_pixel(x0 + y, y0 - x, color);
                drv_ili9341_draw_pixel(x0 - y, y0 - x, color);
            }
        }
        else
        {
            drv_ili9341_draw_pixel(x0, y0 + r, color);
            drv_ili9341_draw_pixel(x0, y0 - r, color);
            drv_ili9341_set_address(x0 - r, y0, x0 + r, y0);
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
            // drv_gpio_set_state(p_dc_gpio, false); // DC pin is low for commands;
            drv_ili9341_spi_send(0x2C);
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for commands
            // drv_gpio_set_state(p_dc_gpio, true); // DC pin is high for data
            for (uint32_t fff = 0; fff < r * 2u + 1u; fff++)
            {
                drv_ili9341_spi_send(color >> 8);
                drv_ili9341_spi_send(color);
            }
            while (x < y)
            {
                if (f >= 0)
                {
                    y--;
                    ddF_y += 2;
                    f += ddF_y;
                }
                x++;
                ddF_x += 2;
                f += ddF_x;
                drv_ili9341_set_address(x0 - x, y0 + y, x0 + x, y0 + y);
                // drv_gpio_set_state(p_dc_gpio, false); // DC pin is low for commands;
                gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
                drv_ili9341_spi_send(0x2C);
                gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for data
                // drv_gpio_set_state(p_dc_gpio, true); // DC pin is high for data
                for (uint32_t fff = 0; fff < x * 2u + 1u; fff++)
                {
                    drv_ili9341_spi_send(color >> 8);
                    drv_ili9341_spi_send(color);
                }
                drv_ili9341_set_address(x0 - x, y0 - y, x0 + x, y0 - y);
                // drv_gpio_set_state(p_dc_gpio, false); // DC pin is low for commands;
                gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
                drv_ili9341_spi_send(0x2C);
                gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for commands
                // drv_gpio_set_state(p_dc_gpio, true); // DC pin is high for data
                for (uint32_t fff = 0; fff < x * 2u + 1u; fff++)
                {
                    drv_ili9341_spi_send(color >> 8);
                    drv_ili9341_spi_send(color);
                }
                drv_ili9341_set_address(x0 - y, y0 + x, x0 + y, y0 + x);
                // drv_gpio_set_state(p_dc_gpio, false); // DC pin is low for commands;
                gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
                drv_ili9341_spi_send(0x2C);
                gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for commands
                // drv_gpio_set_state(p_dc_gpio, true); // DC pin is high for data
                for (uint32_t fff = 0; fff < y * 2u + 1u; fff++)
                {
                    drv_ili9341_spi_send(color >> 8);
                    drv_ili9341_spi_send(color);
                }
                drv_ili9341_set_address(x0 - y, y0 - x, x0 + y, y0 - x);
                // drv_gpio_set_state(p_dc_gpio, false); // DC pin is low for commands;
                gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
                drv_ili9341_spi_send(0x2C);
                // drv_gpio_set_state(p_dc_gpio, true); // DC pin is high for data
                gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for data
                for (uint32_t fff = 0; fff < y * 2u + 1u; fff++)
                {
                    drv_ili9341_spi_send(color >> 8);
                    drv_ili9341_spi_send(color);
                }
            }
        }
    }
    void drv_ili9341_draw_line(uint32_t color, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        int steep = abs(y2 - y1) > abs(x2 - x1);

        if (steep)
        {
            swap(x1, y1);
            swap(x2, y2);
        }

        if (x1 > x2)
        {
            swap(x1, x2);
            swap(y1, y2);
        }

        int dx, dy;
        dx      = (x2 - x1);
        dy      = abs(y2 - y1);
        int err = dx / 2;
        int ystep;
        if (y1 < y2)
        {
            ystep = 1;
        }
        else
        {
            ystep = -1;
        }
        for (; x1 <= x2; x1++)
        {
            if (steep)
            {
                drv_ili9341_draw_pixel(y1, x1, color);
            }
            else
            {
                drv_ili9341_draw_pixel(x1, y1, color);
            }
            err -= dy;
            if (err < 0)
            {
                y1 += ystep;
                err = dx;
            }
        }
    }

    void drv_ili9341_draw_filled_rect(uint32_t color, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
    {
        drv_ili9341_set_address(x1, y1, x2, y2);
        drv_ili9341_send_burst(color, (long)(x2 - x1 + 1) * (long)(y2 - y1 + 1));
    }

    void drv_ili9341_draw_empty_rect(uint32_t color, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
    {
        drv_ili9341_draw_line(color, x1, y1, x2, y1);
        drv_ili9341_draw_line(color, x2, y1, x2, y2);
        drv_ili9341_draw_line(color, x1, y1, x1, y2);
        drv_ili9341_draw_line(color, x1, y2, x2, y2);
    }
    void drv_ili9341_draw_string(uint32_t x, uint32_t y, uint32_t color, uint32_t phone, char *str, uint8_t size)
    {
        switch (size)
        {
        case 1:
            while (*str)
            {
                if ((x + (size * 8)) > X_SIZE)
                {
                    x = 1;
                    y = y + (size * 8);
                }
                drv_ili9341_draw_char(x, y, color, phone, *str, size);
                x += size * 8 - 2;
                str++;
            }
            break;
        case 2:
            hh = 1;
            while (*str)
            {
                if ((x + (size * 8)) > X_SIZE)
                {
                    x = 1;
                    y = y + (size * 8);
                }
                drv_ili9341_draw_char(x, y, color, phone, *str, size);
                x += hh * 8;
                str++;
            }
            break;
        }
    }

    static void drv_ili9341_send_command(uint8_t command)
    {
        before_sending_command();
        drv_ili9341_spi_send(command);
    }

    static void drv_ili9341_send_data(uint8_t data)
    {
        before_sending_data();
        drv_ili9341_spi_send(data);
    }

    static void drv_ili9341_send_burst(uint16_t color, uint32_t len)
    {
        uint16_t blocks;
        uint8_t  i;
        uint8_t  high_bit = color >> 8;
        uint8_t  low_bit  = color;
        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                               (bool)false); // CS is opposite, so true deselects the display
        }
        // drv_gpio_set_state(p_cs_gpio, (bool)false); // Selects the display
        // drv_gpio_set_state(p_dc_gpio, false);       // DC pin is low for commands
        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
        drv_ili9341_spi_send(0x2C);
        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true);
        drv_ili9341_spi_send(high_bit);
        drv_ili9341_spi_send(low_bit);
        len--;
        blocks = (uint16_t)(len / 64); // 64 pixels/block
        if (high_bit == low_bit)
        {
            while (blocks--)
            {
                // i = 16;
                uint8_t data[8 * 16];
                for (uint32_t j = 0; j < 8 * 16; j += 8)
                {
                    data[j]     = low_bit;
                    data[j + 1] = low_bit;
                    data[j + 2] = low_bit;
                    data[j + 3] = low_bit;
                    data[j + 4] = low_bit;
                    data[j + 5] = low_bit;
                    data[j + 6] = low_bit;
                    data[j + 7] = low_bit;
                }
                spi_hal_write(p_spi, data, 8 * 16);
                /*
                                do
                                {

                                    spi_hal_write(p_spi, &data, 8);

                                    drv_ili9341_spi_send(low_bit);

                                    drv_ili9341_spi_send(low_bit);

                                    drv_ili9341_spi_send(low_bit);

                                    drv_ili9341_spi_send(low_bit);

                                    drv_ili9341_spi_send(low_bit);

                                    drv_ili9341_spi_send(low_bit);

                                    drv_ili9341_spi_send(low_bit);

                                    drv_ili9341_spi_send(low_bit);


                                } while (--i);
                                */
            }
            // Fill any remaining pixels(1 to 64)
            for (i = (unsigned char)len & 63; i--;)
            {
                drv_ili9341_spi_send(low_bit);
                drv_ili9341_spi_send(low_bit);
            }
        }
        else
        {
            while (blocks--)
            {
                i = 16;
                uint8_t data[8 * 16];
                for (uint32_t j = 0; j < 8 * 16; j += 8)
                {
                    data[j]     = high_bit;
                    data[j + 1] = low_bit;
                    data[j + 2] = high_bit;
                    data[j + 3] = low_bit;
                    data[j + 4] = high_bit;
                    data[j + 5] = low_bit;
                    data[j + 6] = high_bit;
                    data[j + 7] = low_bit;
                }

                spi_hal_write(p_spi, data, 8 * 16);
                /*
                do
                {
                    uint8_t data[8] = {high_bit, low_bit, high_bit, low_bit, high_bit,
                low_bit, high_bit, low_bit}; spi_hal_write(p_spi, &data, 8);

                    drv_ili9341_spi_send(high_bit);
                    drv_ili9341_spi_send(low_bit);
                    drv_ili9341_spi_send(high_bit);
                    drv_ili9341_spi_send(low_bit);
                    drv_ili9341_spi_send(high_bit);
                    drv_ili9341_spi_send(low_bit);
                    drv_ili9341_spi_send(high_bit);
                    drv_ili9341_spi_send(low_bit);

                } while (--i);
                */
            }
            // Fill any remaining pixels(1 to 64)
            for (i = (unsigned char)len & 63; i--;)
            {
                drv_ili9341_spi_send(high_bit);
                drv_ili9341_spi_send(low_bit);
            }
        }
        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                               (bool)true); // CS is opposite, so true deselects the display
        }
        // drv_gpio_set_state(p_cs_gpio, (bool)true); // deselects the display
    }

    static void drv_ili9341_draw_char(uint32_t x, uint32_t y, uint32_t color, uint32_t phone, uint8_t charcode,
                                      uint8_t size)
    {
        uint32_t i, h;
        switch (size)
        {
        case 1:
            drv_ili9341_set_address(x, y, x + 5, y + 8);
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
            // drv_gpio_set_state(p_dc_gpio, false); // DC pin is low for commands
            drv_ili9341_spi_send(0x2C);
            // drv_gpio_set_state(p_dc_gpio, true); // DC pin is high for data
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for data
            for (h = 0; h < 8; h++)
            {
                for (i = 2; i < 8; i++)
                {
                    if ((chars8[charcode - 0x20][h] >> (7 - i)) & 0x01)
                    {
                        drv_ili9341_spi_send(color >> 8);
                        drv_ili9341_spi_send(color);
                    }
                    else
                    {
                        drv_ili9341_spi_send(phone >> 8);
                        drv_ili9341_spi_send(phone);
                    }
                }
            }
            break;
        case 2:
            drv_ili9341_set_address(x, y, x + 7, y + 16);
            // drv_gpio_set_state(p_dc_gpio, false); // DC pin low for command
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
            drv_ili9341_spi_send(0x2C);
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for data
            // drv_gpio_set_state(p_dc_gpio, true); // DC pin high for data

            for (h = 0; h < 16; h++)
            {
                for (i = 0; i < 8; i++)
                {
                    if ((chars16[charcode - 0x20][h] >> (7 - i)) & 0x01)
                    {
                        drv_ili9341_spi_send(color >> 8);
                        drv_ili9341_spi_send(color);
                    }
                    else
                    {
                        drv_ili9341_spi_send(phone >> 8);
                        drv_ili9341_spi_send(phone);
                    }
                }
            }
            break;
        }
    }

    static void before_sending_data()
    {
        // drv_gpio_set_state(p_dc_gpio, true);
        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true); // for commands
        if (p_cs_gpio != NULL)                                   // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                               (bool)false); // CS is opposite, so true deselects the display
        }
        // drv_gpio_set_state(p_cs_gpio, (bool)false); // Selects the display

        // HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    }

    static void before_sending_command()
    {
        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false); // for commands
        // drv_gpio_set_state(p_dc_gpio, false);
        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                               (bool)false); // CS is opposite, so true deselects the display
        }
        // drv_gpio_set_state(p_cs_gpio, (bool)false); // Selects the display
        //  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
        //  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    }
#if UNIT_TESTS == 1u

#include <drv_ili9341_data.h>

    void Display_Square_Icon_40x40(const unsigned int icon[], unsigned int x0, unsigned int y0)
    {
        uint64_t k = 0;

        drv_ili9341_set_address(x0, y0, x0 + 39, y0 + 39);
        // drv_gpio_set_state(p_cs_gpio, (bool)false); // Selects the display
        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                               (bool)false); // CS is opposite, so true deselects the display
        }

        // drv_gpio_set_state(p_dc_gpio, false);       // DC pin is low for commands
        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false);
        drv_ili9341_spi_send(0x2C);
        gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true);
        // drv_gpio_set_state(p_dc_gpio, true);
        uint8_t data[40 * 2];
        // for (uint32_t j = y0; j < y0 + 40; j++)
        for (uint32_t i = 0; i < 40; i++)
        {
            // for (uint32_t i = x0; i < x0 + 40; i++)
            for (uint32_t j = 0; j < 80; j += 2)
            {
                data[j]     = icon[k] >> 8;
                data[j + 1] = icon[k] & 0xff;
                // drv_ili9341_draw_pixel(i, j, icon[k]);
                k++;
            }
            spi_hal_write(p_spi, data, 80);
        }
    }

    void Display_Round_Icon_40x40(const unsigned int icon[], unsigned int x0, unsigned int y0, unsigned int r)
    {
        // uint64_t k = 0;

        drv_ili9341_draw_circle(x0, y0, r, ILI9341_BLUE, 1);

        Display_Square_Icon_40x40(icon, x0 - 19, y0 - 19);
        /*
        for (uint32_t j = y0 - 19; j < y0 + 21; j++)
        {
            for (uint32_t i = x0 - 19; i < x0 + 21; i++)
            {
                drv_ili9341_draw_pixel(i, j, icon[k]);
                k++;
            }
        }
        */
    }

    void Display_Round_Icon_40x40_2(const uint32_t icon[], uint32_t x0, uint32_t y0, uint32_t color)
    {
        // uint64_t k = 0;
        uint8_t data[80];
        // drv_ili9341_draw_circle(x0, y0, r, ILI9341_BLUE, 1);
        uint32_t radius = 40u;
        int32_t  x;
        int32_t  y;
        uint32_t i = 0;

        drv_ili9341_set_address(x0, y0, x0 + 40, y0 + 40);
        for (y = -radius; y <= radius; y++)
        {
            for (x = -radius; x <= radius; x++)
            {
                // if (x * x + y * y <= radius * radius)
                {
                    data[i]     = color >> 8;
                    data[i + 1] = color & 0xff;
                    i += 2;
                    // printf("*");
                }
                // else
                //{
                //     printf(" ");
                // }
            }

            // drv_gpio_set_state(p_cs_gpio, (bool)false); // Selects the display
            if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
            {
                gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                                   (bool)false); // CS is opposite, so true deselects the display
            }
            // drv_gpio_set_state(p_dc_gpio, false);       // DC pin is low for commands
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false);
            drv_ili9341_spi_send(0x2C);
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true);
            // drv_gpio_set_state(p_dc_gpio, true);

            spi_hal_write(p_spi, data, 80);
            if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
            {
                gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                                   (bool)true); // CS is opposite, so true deselects the display
            }
            // drv_gpio_set_state(p_cs_gpio, (bool)true); // Selects the display
            i = 0;
            // printf("\n");
        }

        // Display_Square_Icon_40x40(icon, x0 - 19, y0 - 19);
        /*
        for (uint32_t j = y0 - 19; j < y0 + 21; j++)
        {
            for (uint32_t i = x0 - 19; i < x0 + 21; i++)
            {
                drv_ili9341_draw_pixel(i, j, icon[k]);
                k++;
            }
        }
        */
    }

    void Display_Round_Icon_40x40_3(const unsigned int icon[], const unsigned int x0, const unsigned int y0,
                                    uint32_t color)
    {
        uint32_t radius       = 40;
        uint32_t k            = 0;
        uint8_t  data[80 * 2] = {0};
        for (int32_t y = -(int32_t)radius; y <= (int32_t)radius; y++)
        {

            // drv_gpio_set_state(p_cs_gpio, (bool)false); // Selects the display

            for (int32_t x = -(int32_t)radius; x <= (int32_t)radius; x++)
            {

                if (x * x + y * y <= radius * radius)
                {
                    data[k]     = (uint8_t)color >> 8;
                    data[k + 1] = (uint8_t)color & 0xff;
                }

                k += 2;
            };

            drv_ili9341_set_address(x0 - 40, y + (int32_t)y0, x0 + 39, y + (int32_t)y0 + 1);
            if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
            {
                gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                                   (bool)false); // CS is opposite, so true deselects the display
            }
            // drv_gpio_set_state(p_dc_gpio, false); // DC pin is low for commands
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, false);
            drv_ili9341_spi_send(0x2C);
            gpio_hal_set_state(p_dc_gpio, ILI9341_SPI_DC_PIN, true);
            // drv_gpio_set_state(p_dc_gpio, true);
            k = 0;
            spi_hal_write(p_spi, data, 160);
            if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
            {
                gpio_hal_set_state(p_cs_gpio, ILI9341_SPI_CS_PIN,
                                   (bool)true); // CS is opposite, so true deselects the display
            }
            // drv_gpio_set_state(p_cs_gpio, (bool)true); // Selects the display

            memset(data, 0, 160);
        }

        Display_Square_Icon_40x40(icon, x0 - 19, y0 - 19);
    }

    void drv_ili9341_unit_test(void)
    {
        /* Counting through all the bytes of those icons */
        uint64_t k = 0;
        unittest_start_test("drv_ili9341_unit_test");
        unittest_start_testcase("drv_ili9341_init");

        (void)unittest_check_true(drv_ili9341_init(SPI_DEVICE_1, ILI9341_SPI_CS_PORT, ILI9341_SPI_CS_PIN,
                                                   ILI9341_SPI_RESET_PORT, ILI9341_SPI_RESET_PIN, ILI9341_SPI_DC_PORT,
                                                   ILI9341_SPI_DC_PIN, ILI9341_SPI_BCKL_PORT, ILI9341_SPI_BCKL_PIN));
        unittest_start_test("drawing");
        drv_ili9341_set_rotation(0);

        clock_hal_delay(5u);

        // drv_ili9341_draw_pixel(10, 10, ILI9341_BLUE);

        // drv_ili9341_fill_screen(ILI9341_GREEN);

        drv_ili9341_fill_screen(ILI9341_RED);
        clock_hal_delay(5u);

        drv_ili9341_fill_screen(ILI9341_GREEN);
        clock_hal_delay(5u);
        drv_ili9341_fill_screen(ILI9341_BLUE);
        clock_hal_delay(5u);
        drv_ili9341_set_rotation(3);

        // Refresh the screen to black background
        drv_ili9341_fill_screen(ILI9341_BLACK);
        clock_hal_delay(5);

        // Counting through all the bytes of those icons
        // uint64_t k = 0;
        // Draw border for the menu
        drv_ili9341_draw_empty_rect(ILI9341_YELLOW, 10, 30, 310, 230);
        drv_ili9341_draw_empty_rect(ILI9341_YELLOW, 310, 230, 10, 30);
        // Write something
        drv_ili9341_draw_string(10, 10, ILI9341_WHITE, ILI9341_BLACK, "Welcome!", 2);

        // Battery Icon in the top right corner
        for (uint32_t j = 10; j < 20; j++)
        {
            for (uint32_t i = 280; i < 300; i++)
            {
                drv_ili9341_draw_pixel(i, j, battery_icon[k]);
                k++;
            }
        }

        // =================================List of
        // Icons=================================

        // ===========================Icon No.1===========================
        Display_Round_Icon_40x40(home_icon_40x40, 59, 79, ILI9341_BLUE);

        // ===========================Icon No.2===========================
        Display_Round_Icon_40x40(music_icon_40x40, 159, 79, ILI9341_BLUE);

        // ===========================Icon No.3===========================
        Display_Round_Icon_40x40(note_icon_40x40, 259, 79, ILI9341_BLUE);

        // ===========================Icon No.4===========================
        Display_Round_Icon_40x40(game_icon_40x40, 59, 179, ILI9341_BLUE);

        // ===========================Icon No.5===========================
        Display_Round_Icon_40x40(facebook_icon_40x40, 159, 179, ILI9341_BLUE);

        // ===========================Icon No.6===========================
        Display_Round_Icon_40x40(image_icon_40x40, 259, 179, ILI9341_BLUE);
        /*
                // Refresh the screen to black background
                drv_ili9341_fill_screen(ILI9341_BLACK);
                clock_hal_delay(5);

                // Draw border for the menu
                drv_ili9341_draw_empty_rect(ILI9341_YELLOW, 10, 30, 310, 230);

                // Write something
                drv_ili9341_draw_string(20, 40, ILI9341_WHITE, ILI9341_BLACK, "Hello
           User!", 2);

                drv_ili9341_draw_string(20, 60, ILI9341_WHITE, ILI9341_BLACK, "This is
           the test for TFT LCD!", 2);

                drv_ili9341_draw_string(20, 80, ILI9341_WHITE, ILI9341_BLACK, "For
           more information, please visit:", 2);

                drv_ili9341_draw_string(20, 100, ILI9341_WHITE, ILI9341_BLACK, "
           aweirdolife.wordpress.com    ", 2);

                Display_Square_Icon_40x40(back_icon_40x40, 0, 200);
        */
        unittest_check_true(true);

        drv_gpio_set_state(p_cs_gpio, (bool)true); // deselects the display
        spi_hal_set_baud(p_spi, 1000000u);
    }
#endif
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
