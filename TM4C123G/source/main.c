#include "device_gpio.h"
// #include "device_sysctl.h"
#include "clock_hal.h"
#include "drv_button.h"
#include "drv_led.h"
#include "gpio_hal.h"
#include "tm4c123gh6pm.h"
#include <hw_config.h>
#include <i2c_hal.h>
#include <stddef.h>
#include <stdint.h>
#include <timer_hal.h>

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
static p_timer_hal_t  timer_handle     = NULL;
// static p_i2c_hal_t    i2c_handle       = NULL;
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
    timer_handle = timer_hal_create(1000U, false, NULL);
    //   i2c_handle = i2c_hal_create_device(0, NULL);
    //   i2c_hal_enable(i2c_handle, true, 0);
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
    timer_hal_enable(timer_handle, true);
#endif // HW_CONFIG_GPIO

    timer_hal_start(timer_handle);

    uint32_t last_found_address = 8U;

    while (last_found_address != 0U)
    {
        last_found_address =
            i2c_hal_scan(1u, I2C1_SDA_PORT, I2C1_SDA_PIN, I2C1_SCL_PORT, I2C1_SCL_PIN, last_found_address);
        if (last_found_address != 0U)
        {
            // Device found at last_found_address
            volatile uint32_t dummy = last_found_address; // Set breakpoint here to inspect detected device address
            (void)dummy;
        }
    }
    while (1)
    {

#if HW_CONFIG_GPIO == 1

        //       drv_led_toggle(blue_led_handle);
        //       clock_hal_delay(1000);
// drv_led_toggle(green_led_handle);
#endif // HW_CONFIG_GPIO
#if 1
        timer_hal_start(timer_handle);
        while (!timer_hal_get_overflow(timer_handle))
        {
        }
        timer_hal_reset_count(timer_handle);
        // clock_hal_delay(1000U);
        drv_led_toggle(blue_led_handle);
#endif
    }
}
