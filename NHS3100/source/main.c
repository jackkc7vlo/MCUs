/*lint -esym(793,__*)*/
/*lint -esym(793,SEGGER*)*/
#include <gpio_hal.h>
#include <hw_config.h>

// #include "device_gpio.h"
// #include "device_iocon.h"
// #include "device_syscon.h"

void delay(unsigned long count);

/**
 * @brief
 *
 * @return int
 */
int main(void) /*lint !e970*/
{
    // Enable GPIO and IOCON clock

#if HW_CONFIG_GPIO == 1
    p_gpio_hal_t gpio_handle = gpio_hal_create(0);
    gpio_handle->init(gpio_handle);
#endif // HW_CONFIG_GPIO
    delay(10000);
    while (1)
    {
        delay(10000);
#if HW_CONFIG_GPIO == 1
        gpio_handle->toggle(gpio_handle, 7);
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
