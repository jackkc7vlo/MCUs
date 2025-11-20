#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "sysctl.h"
#include "gpio.h"

//---PORT-F I/O---//
#define PF1 0x02
#define PF2 0x04
#define PF3 0x08

//---USER FUNCTION'S---//
void delay(unsigned long);

int main(void)
{
    p_sysctl->rcgcgpio |= 0x20; // Enable clock for PORTF
    //SYSCTL_RCGCGPIO_R |= 0x20; // Enable clock for PORTF
    p_gpio_f->den = 0x0E; // Enable PORTF [PF3:PF1] as digital pins
    //GPIO_PORTF_DEN_R  = 0x0E;  // Enable PORTF [PF3:PF1] as digital pins
    p_gpio_f->dir = 0x0E; // Configure PORTF [PF3:PF1] as digital output pins
    //GPIO_PORTF_DIR_R  = 0x0E;  // Configure PORTF [PF3:PF1] as digital output pins

    while (1) {

        p_gpio_f->data = 0; 
        p_gpio_f->data = PF1; // turn on LED_R

        //GPIO_PORTF_DATA_R = 0;
        //GPIO_PORTF_DATA_R |= PF1; // turn on LED_R
        delay(1000000);
        p_gpio_f->data = 0; 
        p_gpio_f->data = PF2; // turn on LED_R

        //GPIO_PORTF_DATA_R = 0;
        //GPIO_PORTF_DATA_R |= PF2; // turn on LED_B
        delay(1000000);
        p_gpio_f->data = 0; 
        p_gpio_f->data = PF3; // turn on LED_R

        //GPIO_PORTF_DATA_R = 0;
        //GPIO_PORTF_DATA_R |= PF3; // turn on LED_G
        delay(1000000);
    }
}

void delay(unsigned long count) {
   unsigned long i=0;
   for(i=0; i<count; i++);
}