/*lint -esym(793,__*)*/
/*lint -esym(793,SEGGER*)*/
#include "device_irq.h"
#include "drv_button.h"
#include "drv_led.h"
#include <clock_hal.h>
#include <gpio_hal.h>
#include <hw_config.h>
#include <i2c_hal.h>
#include <stddef.h>
#include <stdint.h>
#include <timer_hal.h>
// #include "device_gpio.h"
// #include "device_iocon.h"
// #include "device_syscon.h"

static p_led_handle_t led_handle      = NULL;
static p_timer_hal_t  timer_handle    = NULL;
static p_gpio_hal_t   chip_pwr_handle = NULL;

#if USE_BUTTON_GPIO == 1U
void button_callback(button_state_t button_state)
{

    if (button_state == BUTTON_PRESSED)
    {
        drv_led_toggle(led_handle);
    }
}
#endif

/**
 * @brief
 *
 * @return int
 */
int main(void) /*lint !e970*/
{
    clock_hal_init();
    timer_handle    = timer_hal_create(1000U, false, NULL);
    chip_pwr_handle = gpio_hal_create(0);
    if (chip_pwr_handle != NULL)
    {
        gpio_hal_init(chip_pwr_handle);
        gpio_hal_pin_direction(chip_pwr_handle, CHIP_PWR_PIN, PIN_DIRECTION_OUTPUT);
        gpio_hal_set_state(chip_pwr_handle, CHIP_PWR_PIN, true);
    }

#if HW_CONFIG_GPIO == 1 && USE_LED_GPIO == 1
    led_handle = drv_led_create(LED_PORT, GREEN_LED_PIN);
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

#endif // HW_CONFIG_GPIO
    // delay(10000);
    // clock_hal_delay(1000000); // delay 1 second to allow debugger to connect
    uint32_t last_found_address = 8U;

    while (last_found_address != 0U)
    {
        last_found_address = i2c_hal_scan(0u, I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN,
                                          last_found_address);
        if (last_found_address != 0U)
        {
            // Device found at last_found_address
            volatile uint32_t dummy =
                last_found_address; // Set breakpoint here to inspect detected device address
            (void)dummy;
        }
    }
    while (1)
    {
        // clock_hal_delay(1000); // delay 1 second to allow debugger to connect
        timer_hal_start(timer_handle);
        while (!timer_hal_get_overflow(timer_handle))
        {
        }
        timer_hal_reset_count(timer_handle);
#if HW_CONFIG_GPIO == 1
        drv_led_toggle(led_handle);
        //  gpio_hal_toggle_state(gpio_handle, LED_PIN);
        /*
        bool button_state = drv_button_is_pressed(p_button_handle);
        if (button_state)
        // if (gpio_hal_get_state(gpioc_handle, BUTTON_PIN))
        {
            gpio_hal_set_state(gpio_handle, LED_PIN, true);
        }
        else
        {
            gpio_hal_set_state(gpio_handle, LED_PIN, false);
        }
            */
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
