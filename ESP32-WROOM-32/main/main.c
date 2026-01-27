/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "drv_button.h"
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
p_gpio_hal_t gpio_led_handle = NULL;

void delay(unsigned long count);
// #define LED_PIN 2

void button_callback(button_state_t button_state)
{
    // button_handle_t p_button_handle = (button_handle_t)p_button;
    if (button_state == BUTTON_PRESSED)
    {
        gpio_hal_toggle_state(gpio_led_handle, LED_PIN);
    }
    /*   {
           gpio_hal_set_state(gpio_led_handle, LED_PIN, true);

       }
       else
       {
           gpio_hal_set_state(gpio_led_handle, LED_PIN, false);

       } */
}

void app_main(void)
{

#if HW_CONFIG_GPIO == 1 && USE_BUTTON_GPIO == 1
    //    p_gpio_hal_t gpioc_handle = gpio_hal_create(GPIOC_PORT);
    //    if (gpioc_handle != NULL)
    //    {
    //        gpio_hal_init(gpioc_handle);
    //        gpio_hal_pin_direction(gpioc_handle, BUTTON_PIN, INPUT);
    //    }
    p_button_handle_t p_button_handle = drv_button_create(BUTTON_PORT, BUTTON_PIN, button_callback);
    if (p_button_handle != NULL)
    {
        drv_button_init(p_button_handle);
    }
    gpio_led_handle = gpio_hal_create(GPIOA_PORT);
    if (gpio_led_handle != NULL)
    {
        gpio_hal_init(gpio_led_handle);
        gpio_hal_pin_direction(gpio_led_handle, LED_PIN, PIN_DIRECTION_OUTPUT);
    }
#endif // HW_CONFIG_GPIO AND USE_BUTTON_GPIO

    while (1 == 1)
    {
        delay(1000);
        // drv_button_is_pressed(p_button_handle);
        bool pressed = drv_button_is_pressed(p_button_handle);
        ESP_LOGI("TAG", "Button is %s", pressed ? "PRESSED" : "RELEASED");
        bool led_state = gpio_hal_get_state(gpio_led_handle, LED_PIN);
        ESP_LOGI("TAG", "LED is %s", led_state ? "ON" : "OFF");

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
