/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "clock_hal.h"
#include "drv_button.h"
#include "drv_led.h"
#include "esp_log.h"
// include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_hal.h"
#include "hw_config.h"
#include "i2c_hal.h"
#include <drv_ili9341.h>
#include <esp_log.h>
#include <spi_hal.h>
#include <stdio.h>
#include <timer_hal.h>

#include "sdkconfig.h"

// static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
// static bool  s_led_state     = false;
static p_led_handle_t led_handle   = NULL;
static p_timer_hal_t  timer_handle = NULL;

#if HAS_ILI9341 == 1u
// for TFT display
static p_gpio_hal_t cs_gpio_handle    = NULL;
static p_gpio_hal_t reset_gpio_handle = NULL;
static p_gpio_hal_t dc_gpio_handle    = NULL;
static p_gpio_hal_t bckl_gpio_handle  = NULL;
#endif

void button_callback(button_state_t button_state)
{
    // button_handle_t p_button_handle = (button_handle_t)p_button;
    if (button_state == BUTTON_PRESSED)
    {
        drv_led_toggle(led_handle);
    }
}

void app_main(void)
{
    clock_hal_init();
    timer_handle = timer_hal_create(1000U, false, NULL);
#if HW_CONFIG_GPIO == 1 && USE_LED_GPIO == 1
    led_handle = drv_led_create(LED_PORT, LED_PIN);
    if (led_handle != NULL)
    {
        drv_led_init(led_handle);
    }
#endif // HW_CONFIG_GPIO AND USE_LED_GPIO
#if HW_CONFIG_GPIO == 1 && USE_BUTTON_GPIO == 1
    p_button_handle_t p_button_handle = drv_button_create(BUTTON_PORT, BUTTON_PIN, button_callback);
    if (p_button_handle != NULL)
    {
        drv_button_init(p_button_handle);
    }

#endif // HW_CONFIG_GPIO AND USE_BUTTON_GPIO
#if HAS_ILI9341 == 1u
    cs_gpio_handle = gpio_hal_create(ILI9341_SPI_CS_PORT);
    if (cs_gpio_handle != NULL)
    {
        gpio_hal_init(cs_gpio_handle);
        gpio_hal_pin_direction(cs_gpio_handle, ILI9341_SPI_CS_PIN, PIN_DIRECTION_OUTPUT);
    }
    reset_gpio_handle = gpio_hal_create(ILI9341_SPI_RESET_PORT);
    if (reset_gpio_handle != NULL)
    {
        gpio_hal_init(reset_gpio_handle);
        gpio_hal_pin_direction(reset_gpio_handle, ILI9341_SPI_RESET_PIN, PIN_DIRECTION_OUTPUT);
    }
    dc_gpio_handle = gpio_hal_create(ILI9341_SPI_DC_PORT);
    if (dc_gpio_handle != NULL)
    {
        gpio_hal_init(dc_gpio_handle);
        gpio_hal_pin_direction(dc_gpio_handle, ILI9341_SPI_DC_PIN, PIN_DIRECTION_OUTPUT);
    }
    bckl_gpio_handle = gpio_hal_create(ILI9341_SPI_BCKL_PORT);
    if (bckl_gpio_handle != NULL)
    {
        gpio_hal_init(bckl_gpio_handle);
        gpio_hal_pin_direction(bckl_gpio_handle, ILI9341_SPI_BCKL_PIN, PIN_DIRECTION_OUTPUT);
    }
    p_spi_hal_t spi_device_handle = spi_hal_create_device(SPI_BUS_1, NULL);
    if (spi_device_handle != NULL)
    {
        spi_hal_initialize();
        spi_hal_set_baud(spi_device_handle, SPI_HAL_BAUD_30MHz);
        spi_hal_enable(spi_device_handle, true);
        if (drv_ili9341_init(spi_device_handle, cs_gpio_handle, reset_gpio_handle, dc_gpio_handle, bckl_gpio_handle))
        {
            drv_ili9341_set_rotation(1);
            drv_ili9341_fill_screen(ILI9341_BLUE);
            drv_ili9341_draw_empty_rect(ILI9341_YELLOW, 10, 30, 310, 230);
            drv_ili9341_draw_empty_rect(ILI9341_YELLOW, 310, 230, 10, 30);
            // Write something
            drv_ili9341_draw_string(10, 10, ILI9341_WHITE, ILI9341_BLACK, "Welcome!", 2);
        }
    }
#endif
    timer_hal_enable(timer_handle, true);

    uint32_t last_found_address = 8U;

    while (last_found_address != 0U)
    {
        last_found_address =
            i2c_hal_scan(0U, I2C1_SDA_PORT, I2C1_SDA_PIN, I2C1_SCL_PORT, I2C1_SCL_PIN, last_found_address);
        if (last_found_address != 0U)
        {
            // Device found at last_found_address
            volatile uint32_t dummy = last_found_address; // Set breakpoint here to inspect detected device address
        }
    }

    while (1 == 1)
    {

        while (!timer_hal_get_overflow(timer_handle))
        {
            vTaskDelay(1); // Delay 1ms, feeds watchdog
        }
        timer_hal_reset_count(timer_handle);
        timer_hal_start(timer_handle);
        // clock_hal_delay(1000U);
        drv_led_toggle(led_handle);

        // drv_button_is_pressed(p_button_handle);
        // bool pressed = drv_button_is_pressed(p_button_handle);
        // ESP_LOGI("TAG", "Button is %s", pressed ? "PRESSED" : "RELEASED");

        // gpio_hal_toggle_state(gpio_led_handle, LED_PIN);
        /*if (pressed)
        {
            gpio_hal_set_state(gpio_led_handle, LED_PIN, true);
        }
        else
        {
            gpio_hal_set_state(gpio_led_handle, LED_PIN, false);
        }
            */
    }
}

void delay(unsigned long count)
{
    vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
}
