/*lint -esym(793,__*)*/
/*lint -esym(793,SEGGER*)*/
// #include "device_irq.h"
#include "drv_button.h"
#include "drv_led.h"
#include <clock_hal.h>
#include <gpio_hal.h>
#include <hw_config.h>
#include <stddef.h>
#include <stdint.h>
// #include "device_gpio.h"
// #include "device_iocon.h"
// #include "device_syscon.h"

void                  delay(unsigned long count);
static p_led_handle_t led_handle = NULL;

#if USE_BUTTON_GPIO == 1
void button_callback(button_state_t button_state)
{

    if (button_state == BUTTON_PRESSED)
    {
        drv_led_toggle(led_handle);
    }
}
#endif // USE_BUTTON_GPIO
/*
 * @brief
 *
 * @return int
 */
int main(void) /*lint !e970*/
{
    // Enable GPIO and IOCON clock
    clock_hal_init();

#if HW_CONFIG_GPIO == 1
    led_handle = drv_led_create(LED_PORT, LED_PIN);
    if (led_handle != NULL)
    {
        drv_led_init(led_handle);
    }
#if USE_BUTTON_GPIO == 1
    p_button_handle_t p_button_handle = drv_button_create(BUTTON_PORT, BUTTON_PIN, button_callback);
    if (p_button_handle != NULL)
    {
        drv_button_init(p_button_handle);
    }
#endif // USE_BUTTON_GPIO
#endif // HW_CONFIG_GPIO
    // delay(10000);

    while (1)
    {
        clock_hal_delay(1000);
        drv_led_toggle(led_handle);
#if HW_CONFIG_GPIO == 1
#if 0
        gpio_hal_toggle_state(gpio_handle, LED_PIN);

        bool button_state = drv_button_is_pressed(p_button_handle);
        if (button_state)

        {
            gpio_hal_set_state(gpio_handle, LED_PIN, true);
        }
        else
        {
            gpio_hal_set_state(gpio_handle, LED_PIN, false);
        }
#endif

#endif // HW_CONFIG_GPIO
    }
}

/*lint -e956 -e754 -e785*/
// these symbols are defined in the linker script
extern unsigned int __valid_user_code_checksum; /*lint !e970*/
extern unsigned int _vStackTop;                 /*lint !e970*/

// setup the interrupt vector table
__attribute__((section(".interrupt_vector_table"))) struct
{
    void *stack;               /*lint !e754*/
    int (*reset)(void);        /*lint !e970*/
    void        *_unused[5];   /*lint !e754*/
    unsigned int checksum;     /*lint !e754*/
    void        *_xunused[40]; /*lint !e754*/
} interrupt_vector_table = {
    /*lint !e956*/
    .stack    = &_vStackTop,
    .reset    = main,
    .checksum = (unsigned int)&__valid_user_code_checksum, /*lint !e970*/
};
