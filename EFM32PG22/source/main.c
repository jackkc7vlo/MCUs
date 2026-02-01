/*lint -esym(793,__*)*/
/*lint -esym(793,SEGGER*)*/
// #include "device_irq.h"
#include "drv_button.h"
#include <device_clock.h>
#include <gpio_hal.h>
#include <hw_config.h>
#include <stddef.h>
#include <stdint.h>
// #include "device_gpio.h"
// #include "device_iocon.h"
// #include "device_syscon.h"

void                delay(unsigned long count);
static p_gpio_hal_t gpio_handle = NULL;

#if USE_BUTTON_GPIO == 1
void button_callback(button_state_t button_state)
{

    if (button_state == BUTTON_PRESSED)
    {
        gpio_hal_toggle_state(gpio_handle, LED_PIN);
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

#if HW_CONFIG_GPIO == 1
    gpio_handle = gpio_hal_create(0);
    gpio_hal_init(gpio_handle);
    gpio_hal_pin_direction(gpio_handle, LED_PIN, PIN_DIRECTION_OUTPUT);
    gpio_hal_pin_mode(gpio_handle, LED_PIN, PULLUP);

#if USE_BUTTON_GPIO == 1
    p_button_handle_t p_button_handle = drv_button_create(BUTTON_PORT, BUTTON_PIN, button_callback);
    if (p_button_handle != NULL)
    {
        drv_button_init(p_button_handle);
    }
#endif // USE_BUTTON_GPIO
#endif // HW_CONFIG_GPIO
    delay(10000);

    while (1)
    {
        device_clock_delay(1000);
#if HW_CONFIG_GPIO == 1
#if 0
        // gpio_hal_toggle_state(gpio_handle, LED_PIN);

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

void delay(unsigned long count)
{
    unsigned long i = 0;
    for (i = 0; i < count; i++)
        ;
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
