/*lint -esym(793,__*)*/
/*lint -esym(793,SEGGER*)*/
#include <hw_config.h>
#include "gpio.h"
#include "iocon.h"
#include "syscon.h"

void delay(unsigned long count);

/**
 * @brief 
 * 
 * @return int 
 */
int main(void) /*lint !e970*/
{
    // Enable GPIO and IOCON clock
    p_syscon->ahbclkctrl |= (SYSCON_AHBCLKCTRL_GPIO_BIT | SYSCON_AHBCLKCTRL_IOCON_BIT) & SYSCON_AHBCLKCTRL_BITS_THAT_CAN_BE_SET;;  // Enable GPIO and IOCON clock
 
    p_iocon->pio0_7 = IOCON_FUNC_GPIO | IOCON_MODE_PULLUP; // Configure PIO0_7 as GPIO (CT16B_M1 function disabled)

    p_gpio->dir |= GPIO_PIN7_BIT; // Set PIO0_7 as output
 
    p_gpio->data[GPIO_PIN7_BIT] = GPIO_PIN7_BIT; // Clear PIO0_7
    //SET_GPIO_BIT(1,  p_gpio->data[GPIO_PIN7_BIT]);
    //CLEAR_GPIO_BIT(1,  p_gpio->data[GPIO_PIN7_BIT]);

    delay(10000);
    while (1)
    {
        p_gpio->data[GPIO_PIN7_BIT] = 0; // Turn on LED (set PIO0_7 high)
        delay(10000);
        p_gpio->data[GPIO_PIN7_BIT] = GPIO_PIN7_BIT; // Turn off LED (set PIO0_7 low)
        delay(10000);


    }

}

void delay(unsigned long count) {
   unsigned long i=0;
   for(i=0; i<count; i++);
}

/*lint -e956 -e754 -e785*/
// these symbols are defined in the linker script
extern unsigned int __valid_user_code_checksum; /*lint !e970*/
extern unsigned int _vStackTop;                 /*lint !e970*/

// setup the interrupt vector table
__attribute__((section(".interrupt_vector_table"))) struct
{
    void *stack;           /*lint !e754*/
    int (*reset)(void);    /*lint !e970*/
    void *_unused[5];      /*lint !e754*/
    unsigned int checksum; /*lint !e754*/
    void *_xunused[40];    /*lint !e754*/
} interrupt_vector_table = {
    /*lint !e956*/
    .stack = &_vStackTop,
    .reset = main,
    .checksum = (unsigned int)&__valid_user_code_checksum, /*lint !e970*/
};
