/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_hal.h"
#include "esp_log.h"

#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
static bool s_led_state = false;

#define LED_PIN 2

void app_main(void)
{
#if HW_CONFIG_GPIO == 1
    p_gpio_hal_t gpio_handle = gpio_hal_create(0);
    gpio_handle->init(gpio_handle);
#endif // HW_CONFIG_GPIO


    while (1) {
        #if HW_CONFIG_GPIO == 1
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        //gpio_set_level(LED_PIN, s_led_state);
        gpio_handle->toggle(gpio_handle, LED_PIN);
        
        //
        // blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        #endif
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
