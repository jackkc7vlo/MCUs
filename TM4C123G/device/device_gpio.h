/**
 * @file device_gpio.h
 * @author Jack Wilson (jackkc7vlo@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-15
 *
 * @copyright Copyright (c) 2025
 *
 * General Purpose Input/Output (GPIO) definitions for the TI TM4C123GH6PM microcontroller.
 */

#include <stdint.h>
#ifndef DEVICE_GPIO_H
#define DEVICE_GPIO_H
// lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
// lint -restore
// lint -esym(751,p_gpio_*) There are definitions they may not be used
// lint -esym(754,p_gpio_*) There are definitions they may not be used

#define GPIO_PIN0_BIT (1U << 0U) //!< Bit mask for GPIO Pin 0
#define GPIO_PIN1_BIT (1U << 1U) //!< Bit mask for GPIO Pin 1
#define GPIO_PIN2_BIT (1U << 2U) //!< Bit mask for GPIO Pin 2
#define GPIO_PIN3_BIT (1U << 3U) //!< Bit mask for GPIO Pin 3
#define GPIO_PIN4_BIT (1U << 4U) //!< Bit mask for GPIO Pin 4
#define GPIO_PIN5_BIT (1U << 5U) //!< Bit mask for GPIO Pin 5
#define GPIO_PIN6_BIT (1U << 6U) //!< Bit mask for GPIO Pin 6
#define GPIO_PIN7_BIT (1U << 7U) //!< Bit mask for GPIO Pin 7

// GPIO Base Addresses
#define GPIO_PORTA_APB_BASE_ADDR 0x40004000U
#define GPIO_PORTB_APB_BASE_ADDR 0x40005000U
#define GPIO_PORTC_APB_BASE_ADDR 0x40006000U
#define GPIO_PORTD_APB_BASE_ADDR 0x40007000U
#define GPIO_PORTE_APB_BASE_ADDR 0x40024000U
#define GPIO_PORTF_APB_BASE_ADDR 0x40025000U
#define GPIO_PORTA_AHB_BASE_ADDR 0x40058000U
#define GPIO_PORTB_AHB_BASE_ADDR 0x40059000U
#define GPIO_PORTC_AHB_BASE_ADDR 0x4005A000U
#define GPIO_PORTD_AHB_BASE_ADDR 0x4005B000U
#define GPIO_PORTE_AHB_BASE_ADDR 0x4005C000U
#define GPIO_PORTF_AHB_BASE_ADDR 0x4005D000U

/**
 * @brief GPIO registers
 */
typedef struct
{
    volatile uint32_t data_bits[255]; //!< GPIO Data Bits Register                offset 0x000 - 0x3F8
    volatile uint32_t data;           //!< GPIO Data Register                      offset 0x3FC
    volatile uint32_t dir;            //!< GPIO Direction Register                 offset 0x400
    volatile uint32_t is;             //!< GPIO Interrupt Sense Register           offset 0x404
    volatile uint32_t ibe;            //!< GPIO Interrupt Both Edge Register       offset 0x408
    volatile uint32_t iev;            //!< GPIO Interrupt Event Register           offset 0x40C
    volatile uint32_t im;             //!< GPIO Interrupt Mask Register            offset 0x410
    volatile uint32_t ris;            //!< GPIO Raw Interrupt Status Register      offset 0x414
    volatile uint32_t mis;            //!< GPIO Masked Interrupt Status Register   offset 0x418
    volatile uint32_t icr;            //!< GPIO Interrupt Clear Register           offset 0x41C
    volatile uint32_t afsel;          //!< GPIO Alternate Function Select Register offset 0x420
    volatile uint32_t reserved1[55];  //!< Reserved                                offset 0x424 - 0x4FC
    volatile uint32_t dr2r;           //!< GPIO 2-mA Drive Select Register         offset 0x500
    volatile uint32_t dr4r;           //!< GPIO 4-mA Drive Select Register         offset 0x504
    volatile uint32_t dr8r;           //!< GPIO 8-mA Drive Select Register         offset 0x508
    volatile uint32_t odr;            //!< GPIO Open Drain Enable Register         offset 0x50C
    volatile uint32_t pur;            //!< GPIO Pull-Up Select Register            offset 0x510
    volatile uint32_t pdr;            //!< GPIO Pull-Down Select Register          offset 0x514
    volatile uint32_t slr;            //!< GPIO Slew Rate Control Register         offset 0x518
    volatile uint32_t den;            //!< GPIO Digital Enable Register            offset 0x51C
    volatile uint32_t lock;           //!< GPIO Lock Register                      offset 0x520
    volatile uint32_t cr;             //!< GPIO Commit Register                    offset 0x524
    volatile uint32_t amsel;          //!< GPIO Analog Mode Select Register        offset 0x528
    volatile uint32_t pctl;           //!< GPIO Port Control Register              offset 0x52C
    volatile uint32_t adcctl;         //!< GPIO ADC Control Register               offset 0x530
    volatile uint32_t dmactl;         //!< GPIO DMA Control Register               offset 0x534
    volatile uint32_t reserved2[678]; //!< Reserved                                offset 0x538 - 0xFCC
    volatile uint32_t periphID[8];    //!< GPIO Peripheral Identification Registers offset 0xFD0 - 0xFEC
    volatile uint32_t pcellID[4];     //!< GPIO PrimeCell Identification Registers offset 0xFF0 - 0xFFC
} reg_gpio_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_gpio_t *p_device_gpio_a     = (reg_gpio_t *const)GPIO_PORTA_APB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_b     = (reg_gpio_t *const)GPIO_PORTB_APB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_c     = (reg_gpio_t *const)GPIO_PORTC_APB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_d     = (reg_gpio_t *const)GPIO_PORTD_APB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_e     = (reg_gpio_t *const)GPIO_PORTE_APB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_f     = (reg_gpio_t *const)GPIO_PORTF_APB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_a_ahb = (reg_gpio_t *const)GPIO_PORTA_AHB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_b_ahb = (reg_gpio_t *const)GPIO_PORTB_AHB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_c_ahb = (reg_gpio_t *const)GPIO_PORTC_AHB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_d_ahb = (reg_gpio_t *const)GPIO_PORTD_AHB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_e_ahb = (reg_gpio_t *const)GPIO_PORTE_AHB_BASE_ADDR;
static reg_gpio_t *p_device_gpio_f_ahb = (reg_gpio_t *const)GPIO_PORTF_AHB_BASE_ADDR;
#pragma GCC diagnostic pop

#endif // DEVICE_GPIO_H