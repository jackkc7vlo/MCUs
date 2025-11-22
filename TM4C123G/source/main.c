#include "device_gpio.h"
#include "device_sysctl.h"
#include "gpio_hal.h"
#include "tm4c123gh6pm.h"
#include <hw_config.h>
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
void delay(unsigned long);

int main(void)
{
#if HW_CONFIG_GPIO == 1
    p_gpio_hal_t gpio_handle = gpio_hal_create(GPIOF_PORT);
    gpio_handle->init(gpio_handle);
#endif // HW_CONFIG_GPIO

    while (1)
    {

#if HW_CONFIG_GPIO == 1
        gpio_handle->write(gpio_handle, 0);
        gpio_handle->set(gpio_handle, PF1, true);

        delay(1000000);
        gpio_handle->write(gpio_handle, 0);
        gpio_handle->set(gpio_handle, PF2, true);

        delay(1000000);
        gpio_handle->write(gpio_handle, 0);
        gpio_handle->set(gpio_handle, PF3, true);

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