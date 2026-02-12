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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_hal.h"
#include "hw_config.h"
#include <esp_log.h>
#include <stdio.h>

#include "sdkconfig.h"

// static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
// static bool  s_led_state     = false;
static p_led_handle_t led_handle = NULL;

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

    while (1 == 1)
    {
        clock_hal_delay(1000U);
        // drv_led_toggle(led_handle);

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
