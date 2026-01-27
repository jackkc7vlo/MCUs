#include "device_gpio.h"
#include "device_sysctl.h"
#include "drv_button.h"
#include "gpio_hal.h"
#include "tm4c123gh6pm.h"
#include <hw_config.h>
#include <stddef.h>
#include <stdint.h>

//---PORT-F I/O---//
#define PF1 0x02
#define PF2 0x04
#define PF3 0x08
#define PF4 0x10
#define PF5 0x20
#define PF6 0x40
#define PF7 0x80

//---USER FUNCTION'S---//
void                delay(unsigned long);
static p_gpio_hal_t gpio_handle = NULL;

#if USE_BUTTON_GPIO == 1u
void button_callback(button_state_t button_state)
{

    if (button_state == BUTTON_PRESSED)
    {
        gpio_hal_toggle_state(gpio_handle, RED_LED_PIN);
    }
}
#endif

int main(void)
{
#if HW_CONFIG_GPIO == 1
    gpio_handle = gpio_hal_create(GPIOF_PORT);
    gpio_hal_init(gpio_handle);
    gpio_hal_pin_direction(gpio_handle, RED_LED_PIN, PIN_DIRECTION_OUTPUT);
    gpio_hal_pin_direction(gpio_handle, BLUE_LED_PIN, PIN_DIRECTION_OUTPUT);
    gpio_hal_pin_direction(gpio_handle, GREEN_LED_PIN, PIN_DIRECTION_OUTPUT);

    p_button_handle_t p_button_handle = drv_button_create(BUTTON_PORT, BUTTON_PIN, button_callback);
    if (p_button_handle != NULL)
    {
        drv_button_init(p_button_handle);
    }
#endif // HW_CONFIG_GPIO

    while (1)
    {

#if HW_CONFIG_GPIO == 1
        /*
                bool button_state = drv_button_is_pressed(p_button_handle);
                if (button_state)
                // if (gpio_hal_get_state(gpioc_handle, BUTTON_PIN))
                {
                    gpio_hal_set_state(gpio_handle, RED_LED_PIN, true);
                }
                else
                {
                    gpio_hal_set_state(gpio_handle, RED_LED_PIN, false);
                }
        */
        // gpio_hal_toggle_state(gpio_handle, RED_LED_PIN);
        // delay(1000000);
        // gpio_hal_toggle_state(gpio_handle, BLUE_LED_PIN);
        // delay(1000000);
        // gpio_hal_toggle_state(gpio_handle, GREEN_LED_PIN);
#endif // HW_CONFIG_GPIO

        delay(1000000);
    }
}

void delay(unsigned long count)
{
    unsigned long i = 0;
    for (i = 0; i < count; i++)
        ;
}