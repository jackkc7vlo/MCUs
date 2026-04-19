/**
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************
 * @file       drv_st7735.c
 * @author     Jack Wilson
 * @brief      Driver for the ST7735 LCD controller
 *
 * @note       At present this driver onle support a single ST7735 display
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Jan-19-2024     Jack Wilson                     jackkc7vlo@gmail.com
 *
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
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <hw_config.h>
#if HAS_ST7735 == 1u

#include "drv_st7735.h"
// #include "esp_system.h" // ESP SPECIFIC
#include <gpio_hal.h>
#include <stdint.h>
// #include "drv_gpio.h"
#include <assert.h>
#include <clock_hal.h>
#include <spi_hal.h>
#include <stdbool.h>
#include <stdlib.h> /*lint -e129*/
#include <string.h> /*lint -e129*/

    /********************************************************************************
     * Defines
     ********************************************************************************/
#if UNIT_TESTS == 1u
#include <unittest.h>
#endif

    /********************************************************************************
     * Defines
     ********************************************************************************/
#define DELAY 0x80
#ifndef ST7735_SPI_CS_PIN
#pragma message "ST7735_SPI_CS_PIN not defined in hw_config.h"
#endif

#ifndef ST7735_SPI_RESET_PIN
#pragma message "ST7735_SPI_RESET_PIN not defined in hw_config.h"
#endif

#ifndef ST7735_SPI_DC_PIN
#pragma message "ST7735_SPI_DC_PIN not defined in hw_config.h"
#endif

#ifndef ST7735_SPI_BCKL_PIN
#pragma message "ST7735_SPI_BCKL_PIN not defined in hw_config.h"
#endif

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    // static p_gpio_def_t p_cs_gpio = NULL;
    // static p_gpio_def_t p_reset_gpio = NULL;
    // static p_gpio_def_t p_dc_gpio = NULL;
    // static p_spi_hal_def_t p_spi = NULL;

    // based on Adafruit ST7735 library for Arduino
    DRAM_ATTR static const uint8_t init_cmdsai[] = {
        // Init for 7735R, part 1 (red or green tab)
        5, // number of commands in list:
        ST7735_MADCTL,
        1,
        ST7735_ROTATION, // Memory Access Control
        ST7735_COLMOD,
        1,
        ST7735_COLOR_MODE, // Color Mode
        ST7735_RAMCTL,
        2,
        0x00,
        0xF0, // RAM Control
        ST7735_SLPOUT,
        DELAY,
        255, // SLEEP Out
        ST7735_DISPON,
        DELAY,
        100}; // Display On

    static p_spi_hal_t  p_spi        = NULL;
    static p_gpio_hal_t p_dc_gpio    = NULL;
    static p_gpio_hal_t p_reset_gpio = NULL;
    static p_gpio_hal_t p_bckl_gpio  = NULL;
    static p_gpio_hal_t p_cs_gpio    = NULL;

    /********************************************************************************
     * Functions
     ********************************************************************************/
    static void drv_st7735_select()
    {
        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ST7735_SPI_CS_PIN,
                               false); // CS is opposite, so true deselects the display
        }
        // drv_gpio_set_state(p_cs_gpio, (bool)false); // Selects the display
    }

    // call before initializing any SPI devices
    void drv_st7735_unselect()
    {
        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ST7735_SPI_CS_PIN,
                               true); // CS is opposite, so true deselects the display
        }
        // drv_gpio_set_state(p_cs_gpio, (bool)true); // Deselects the display
    }

    static void drv_st7735_reset()
    {
        // puling the reset line low then high causes a reset of the display
        gpio_hal_set_state(p_reset_gpio, ST7735_SPI_RESET_PIN, false);
        clock_hal_delay(10u);
        gpio_hal_set_state(p_reset_gpio, ST7735_SPI_RESET_PIN, true);
        clock_hal_delay(10u);
    }

    static void drv_st7735_write_command(uint8_t cmd)
    {
        // for commands, D/C is low
        gpio_hal_set_state(p_dc_gpio, ST7735_SPI_DC_PIN, false);
        spi_hal_write(p_spi, &cmd, sizeof(cmd));
    }

    static void drv_st7735_write_data(uint8_t *buff, size_t buff_size)
    {
        // for data, D/C is high
        p_dc_gpio->set(p_dc_gpio, ST7735_SPI_DC_PIN, true);
        spi_hal_write(p_spi, buff, buff_size);
    }

    static void drv_st7735_execute_command_list(const uint8_t *addr)
    {
        uint8_t  numCommands, numArgs;
        uint16_t ms;

        numCommands = *addr++;
        while (numCommands--)
        {
            uint8_t cmd = *addr++;

            drv_st7735_write_command(cmd);

            numArgs = *addr++;
            // If high bit set, delay follows args
            ms = numArgs & DELAY;
            numArgs &= ~DELAY;
            if (numArgs)
            {
                drv_st7735_write_data((uint8_t *)addr, numArgs);
                addr += numArgs;
            }

            if (ms)
            {
                ms = *addr++;
                if (ms == 255)
                    ms = 500;
                clock_hal_delay(ms);
            }
        }
    }

    static void drv_st7735_set_address_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
    {
        // column address set
        drv_st7735_write_command(ST7735_CASET);
        uint8_t data[] = {0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART};
        drv_st7735_write_data(data, sizeof(data));

        // row address set
        drv_st7735_write_command(ST7735_RASET);
        data[1] = y0 + ST7735_YSTART;
        data[3] = y1 + ST7735_YSTART;
        drv_st7735_write_data(data, sizeof(data));

        // write to RAM
        drv_st7735_write_command(ST7735_RAMWR);
    }

    bool drv_st7735_init(p_spi_hal_t spi_device_handle, p_gpio_hal_t cs_gpio_handle, p_gpio_hal_t reset_gpio_handle,
                         p_gpio_hal_t dc_gpio_handle, p_gpio_hal_t bckl_gpio_handle)
    {
        p_spi        = spi_device_handle;
        p_dc_gpio    = dc_gpio_handle;
        p_reset_gpio = reset_gpio_handle;
        p_bckl_gpio  = bckl_gpio_handle;
        p_cs_gpio    = cs_gpio_handle;

        // drv_gpio_set_state(p_cs_gpio, (bool)true); // CS is opposite, so true deselects the display

        if (p_cs_gpio != NULL) // CS is sometimes handled by the SPI driver
        {
            gpio_hal_set_state(p_cs_gpio, ST7735_SPI_CS_PIN,
                               true); // CS is opposite, so true deselects the display
        }

        // p_dc_gpio = drv_gpio_create_device(ST7735_SPI_AO_PORT, ST7735_SPI_AO_PIN, GPIO_HAL_MODE_OUTPUT, 0, NULL);

        if (ST7735_SPI_RESET_PIN != 0xff)
        {
            gpio_hal_set_state(p_reset_gpio, ST7735_SPI_RESET_PIN, true);
        }

        if (p_spi != NULL && p_dc_gpio != NULL)
        {
            // drv_gpio_init(p_dc_gpio);
            // p_bckl_gpio->set(p_bckl_gpio, ST7735_SPI_BCKL_PIN, true);
            // drv_gpio_set_state(p_dc_gpio, false);
            gpio_hal_set_state(p_dc_gpio, ST7735_SPI_DC_PIN, false);

            drv_st7735_select();
            if (ST7735_SPI_RESET_PIN != 0xff)
            {
                drv_st7735_reset();
            }
            drv_st7735_execute_command_list(init_cmdsai);

            drv_st7735_unselect();
            return true;
        }
        return false;
    }

    void drv_st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
    {
        if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
            return;

        drv_st7735_select();

        drv_st7735_set_address_window(x, y, x + 1, y + 1);
        uint8_t data[] = {color >> 8, color & 0xFF};
        drv_st7735_write_data(data, sizeof(data));

        drv_st7735_unselect();
    }

    static void drv_st7735_write_char(uint16_t x, uint16_t y, char ch, drv_font_def_t font, uint16_t color,
                                      uint16_t bgcolor)
    {
        uint32_t i, b, j;

        drv_st7735_set_address_window(x, y, x + font.width - 1, y + font.height - 1);

        for (i = 0; i < font.height; i++)
        {
            b = font.data[(ch - 32) * font.height + i];
            for (j = 0; j < font.width; j++)
            {
                if ((b << j) & 0x8000)
                {
                    uint8_t data[] = {color >> 8, color & 0xFF};
                    drv_st7735_write_data(data, sizeof(data));
                }
                else
                {
                    uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
                    drv_st7735_write_data(data, sizeof(data));
                }
            }
        }
    }

    void drv_st7735_write_string(uint16_t x, uint16_t y, const char *str, drv_font_def_t font, uint16_t color,
                                 uint16_t bgcolor)
    {
        drv_st7735_select();

        while (*str)
        {
            if (x + font.width >= ST7735_WIDTH)
            {
                x = 0;
                y += font.height;
                if (y + font.height >= ST7735_HEIGHT)
                {
                    break;
                }

                if (*str == ' ')
                {
                    // skip spaces in the beginning of the new line
                    str++;
                    continue;
                }
            }

            drv_st7735_write_char(x, y, *str, font, color, bgcolor);
            x += font.width;
            str++;
        }

        drv_st7735_unselect();
    }
    void drv_st7735_fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
    {
        // clipping
        if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
            return;
        if ((x + w - 1) >= ST7735_WIDTH)
            w = ST7735_WIDTH - x;
        if ((y + h - 1) >= ST7735_HEIGHT)
            h = ST7735_HEIGHT - y;

        drv_st7735_select();
        drv_st7735_set_address_window(x, y, x + w - 1, y + h - 1);

        uint8_t data[] = {color >> 8, color & 0xFF};
        gpio_hal_set_state(p_dc_gpio, ST7735_SPI_DC_PIN, true);

        for (y = h; y > 0; y--)
        {
            for (x = w; x > 0; x--)
            {
                spi_hal_write(p_spi, data, sizeof(data));
            }
        }

        drv_st7735_unselect();
    }
    void drv_st7735_fill_rectangle_fast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
    {
        // clipping
        if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
            return;
        if ((x + w - 1) >= ST7735_WIDTH)
            w = ST7735_WIDTH - x;
        if ((y + h - 1) >= ST7735_HEIGHT)
            h = ST7735_HEIGHT - y;

        drv_st7735_select();
        drv_st7735_set_address_window(x, y, x + w - 1, y + h - 1);

        // Prepare whole line in a single buffer
        uint8_t  pixel[] = {color >> 8, color & 0xFF};
        uint8_t *line    = malloc(w * sizeof(pixel));
        for (x = 0; x < w; ++x)
            memcpy(line + x * sizeof(pixel), pixel, sizeof(pixel));

        // drv_gpio_set_state(p_dc_gpio, true);
        gpio_hal_set_state(p_dc_gpio, ST7735_SPI_DC_PIN, true);

        for (y = h; y > 0; y--)
        {
            spi_hal_write(p_spi, line, w * sizeof(pixel));
        }

        free(line);
        drv_st7735_unselect();
    }
    void drv_st7735_fill_screen(uint16_t color)
    {
        drv_st7735_fill_rectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
    }
    void drv_st7735_fill_screen_fast(uint16_t color)
    {
        drv_st7735_fill_rectangle_fast(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
    }
    void drv_st7735_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
    {
        if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
            return;
        if ((x + w - 1) >= ST7735_WIDTH)
            return;
        if ((y + h - 1) >= ST7735_HEIGHT)
            return;

        drv_st7735_select();
        drv_st7735_set_address_window(x, y, x + w - 1, y + h - 1);
        drv_st7735_write_data((uint8_t *)data, sizeof(uint16_t) * w * h);
        drv_st7735_unselect();
    }
    void drv_st7735_invert_colors(bool invert)
    {
        drv_st7735_select();
        drv_st7735_write_command(invert ? ST7735_INVON : ST7735_INVOFF);
        drv_st7735_unselect();
    }
    void drv_st7735_set_gamma(drv_st7735_gamma_t gamma)
    {
        drv_st7735_select();
        drv_st7735_write_command(ST7735_GAMSET);
        drv_st7735_write_data((uint8_t *)&gamma, sizeof(gamma));
        drv_st7735_unselect();
    }
#if UNIT_TESTS == 1u
    void drv_st7735_unit_test(void)
    {
        unittest_start_test("drv_st7735_unit_test");
        unittest_start_testcase("drv_st7735_init");
        // spi_hal_init();

        (void)unittest_check_true(drv_st7735_init());

        // Check border
        drv_st7735_fill_screen(ST7735_BLUE);

        for (int x = 0; x < ST7735_WIDTH; x++)
        {
            drv_st7735_draw_pixel(x, 0, ST7735_RED);
            drv_st7735_draw_pixel(x, ST7735_HEIGHT - 1, ST7735_RED);
        }

        drv_st7735_write_string(20, 20, "Hello World!", drv_font_7x10, ST7735_WHITE, ST7735_BLACK);

        unittest_end_test();
    }
#endif
#endif // HAS_ST7735 == 1u

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
