#include "device_gpio.h"
// #include "device_sysctl.h"
#include "clock_hal.h"
#include "drv_button.h"
#include "drv_led.h"
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

static p_led_handle_t red_led_handle   = NULL;
static p_led_handle_t green_led_handle = NULL;
static p_led_handle_t blue_led_handle  = NULL;

#if USE_BUTTON_GPIO == 1u
void button_callback(button_state_t button_state)
{

    if (button_state == BUTTON_PRESSED)
    {
        drv_led_toggle(red_led_handle);
    }
}
#endif

int main(void)
{
    // Needed small wait so debugger can connect before the clock is initialized and starts running
    for (volatile uint32_t i = 0; i < 100000; i++)
        ;
    clock_hal_init();
#if HW_CONFIG_GPIO == 1

    red_led_handle   = drv_led_create(LED_PORT, RED_LED_PIN);
    green_led_handle = drv_led_create(LED_PORT, GREEN_LED_PIN);
    blue_led_handle  = drv_led_create(LED_PORT, BLUE_LED_PIN);
    if (red_led_handle != NULL)
    {
        drv_led_init(red_led_handle);
    }
    if (green_led_handle != NULL)
    {
        drv_led_init(green_led_handle);
    }
    if (blue_led_handle != NULL)
    {
        drv_led_init(blue_led_handle);
    }

    p_button_handle_t p_button_handle = drv_button_create(BUTTON_PORT, BUTTON_PIN, button_callback);
    if (p_button_handle != NULL)
    {
        drv_button_init(p_button_handle);
    }
#endif // HW_CONFIG_GPIO

    while (1)
    {

#if HW_CONFIG_GPIO == 1

//        drv_led_toggle(blue_led_handle);
//       clock_hal_delay(1000);
// drv_led_toggle(green_led_handle);
#endif // HW_CONFIG_GPIO

        // clock_hal_delay(1000);
    }
}
