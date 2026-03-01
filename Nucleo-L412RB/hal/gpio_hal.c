/******************************************************************************
 * Copyright (C) 2025 by Jack Wilson
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 ************************(C) COPYRIGHT 2025 Jack Wilson **********************/
/**
 * @file gpio_hal.h
 * @brief Generic GPIO HAL interface definition.
 * @details Outlines the portable GPIO abstraction used to configure and interact with digital pins
 * on any supported platform.
 *
 * @author  Jack Wilson
 * @date    September 26, 2025
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*lint -esym(793,__*)*/
#include <hw_config.h>
#if HW_CONFIG_GPIO == 1
#include <device_gpio.h>
#include <device_irq.h>
#include <device_rcc.h>
#include <device_scb.h>
#include <device_syscfg.h>
#include <gpio_hal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /*lint -e129*/

#include <assert.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    // support for GPIO interrupt callbacks
    typedef struct callback_context_s
    {                                                    /**< The GPIO pin number */
        gpio_hal_interrupt_callback_t callback;          /**< Registered interrupt callback */
        void                         *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
        void                         *callback_context;  /**< User callback context */
    } callback_context_t;

    /********************************************************************************
     * Private GPIO HAL struct (definition kept in the implementation file).
     ********************************************************************************/
    struct gpio_hal
    {
        uint32_t in_use_count;  /**< Indicates if this GPIO port instance is in use */
        uint32_t port_number;   /**< The GPIO port number */
        void    *config_handle; /**< Pointer to platform-specific context passed to the
                                   HAL implementation. */
        void *p_device_gpio;    /**< Pointer to the GPIO port device registers. */

        callback_context_t callbacks[GPIOS_INTERRUPTS]; /**< Registered interrupt callbacks */
    };

    static gpio_hal_t gpio_hal_ports[NUMBER_GPIOS_PORTS] = {0};

    /* Common EXTI handler for a range of pins */
    static inline void handle_exti_range(uint8_t pin_start, uint8_t pin_end)
    {
        /* Read pending register once and mask to pin range */
        uint32_t mask    = ((1U << (pin_end - pin_start + 1U)) - 1U) << pin_start;
        uint32_t pending = p_device_exti->pr1 & mask;

        if (pending == 0U)
        {
            return; /* Early exit - no pending interrupts */
        }

        /* Iterate through pins, processing only those with pending interrupts */
        for (uint32_t pin = pin_start; pin <= pin_end; pin++)
        {
            if ((pending & (1U << pin)) == 0U)
            {
                continue; /* Skip non-pending pins */
            }

            /* Find the GPIO port that has a callback registered for this pin */
            for (uint32_t j = 0; j < NUMBER_GPIOS_PORTS; j++)
            {
                p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)&gpio_hal_ports[j];

                if (p_gpio_hal->callbacks[pin].callback != NULL && p_gpio_hal->in_use_count > 0U)
                {
                    /* Invoke the callback */
                    p_gpio_hal->callbacks[pin].callback(p_gpio_hal->callbacks[pin].p_callback_handle,
                                                        p_gpio_hal->callbacks[pin].callback_context);
                    break; /* Each EXTI line maps to one port; stop searching */
                }
            }

            /* Clear the pending bit by writing 1 */
            p_device_exti->pr1 = (1U << pin);
        }
    }

    void EXTI9_5_IRQHandler(void)
    {
        handle_exti_range(5U, 9U);
    }

    void EXTI15_10_IRQHandler(void)
    {
        handle_exti_range(10U, 15U);
    }

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    reg_gpio_t *get_gpio_registers(uint32_t port)
    {
        switch (port)
        {
        case GPIOA_PORT:
            return p_device_gpio_a;
        case GPIOB_PORT:
            return p_device_gpio_b;
        case GPIOC_PORT:
            return p_device_gpio_c;
        case GPIOD_PORT:
            return p_device_gpio_d;
        case GPIOE_PORT:
            return p_device_gpio_e;
        case GPIOH_PORT:
            return p_device_gpio_h;
        default:
            assert(false); // invalid port
            return NULL;
        }
    }

    p_gpio_hal_t gpio_hal_create(uint32_t port)
    {
        p_gpio_hal_t p_free_slot = NULL;

        /* Single pass: check for existing instance and track first free slot */
        for (uint32_t i = 0; i < NUMBER_GPIOS_PORTS; i++)
        {
            if (gpio_hal_ports[i].in_use_count > 0U)
            {
                if (gpio_hal_ports[i].port_number == port)
                {
                    gpio_hal_ports[i].in_use_count++;
                    return (p_gpio_hal_t)&gpio_hal_ports[i]; /* Return existing */
                }
            }
            else if (p_free_slot == NULL)
            {
                p_free_slot = (p_gpio_hal_t)&gpio_hal_ports[i]; /* Track first free */
            }
        }

        /* Initialize free slot if found */
        if (p_free_slot != NULL)
        {
            p_free_slot->in_use_count++;
            p_free_slot->port_number   = port;
            p_free_slot->p_device_gpio = get_gpio_registers(port);
        }

        return p_free_slot;
    } /*lint !e818*/

    void gpio_hal_remove(p_gpio_hal_t p_handle)
    {
        if (p_handle != NULL && p_handle->in_use_count > 0U)
        {
            p_handle->in_use_count--;

            if (p_handle->in_use_count == 0U)
            {
                /* Deregister any callbacks for this port and disable EXTI lines if unused */
                for (uint32_t pin = 0U; pin < 16U; pin++)
                {
                    if (p_handle->callbacks[pin].callback != NULL ||
                        p_handle->callbacks[pin].p_callback_handle != NULL ||
                        p_handle->callbacks[pin].callback_context != NULL)
                    {
                        /* Clear this handle's callback info */
                        p_handle->callbacks[pin].callback          = NULL;
                        p_handle->callbacks[pin].p_callback_handle = NULL;
                        p_handle->callbacks[pin].callback_context  = NULL;

                        /* If no other port has a callback for this EXTI line, disable it */
                        bool used_elsewhere = false;
                        for (uint32_t j = 0U; j < NUMBER_GPIOS_PORTS; j++)
                        {
                            p_gpio_hal_t p_other = (p_gpio_hal_t)&gpio_hal_ports[j];
                            if (p_other == p_handle)
                            {
                                continue;
                            }
                            if (p_other->in_use_count > 0U && p_other->callbacks[pin].callback != NULL)
                            {
                                used_elsewhere = true;
                                break;
                            }
                        }

                        if (!used_elsewhere)
                        {
                            p_device_exti->imr1 &= ~(1U << pin);
                            p_device_exti->rtsr1 &= ~(1U << pin);
                            p_device_exti->ftsr1 &= ~(1U << pin);
                            p_device_exti->pr1 = (1U << pin); /* clear pending */

                            /* Clear SYSCFG mapping for this EXTI line */
                            p_device_syscfg->exticr[pin / 4U] &= ~(0x0FU << (4U * (pin % 4U)));
                        }
                    }
                }

                /* If no callbacks remain anywhere, disable NVIC IRQs for EXTI lines */
                bool any0_4   = false;
                bool any5_9   = false;
                bool any10_15 = false;

                for (uint32_t j = 0U; j < NUMBER_GPIOS_PORTS; j++)
                {
                    p_gpio_hal_t p_other = (p_gpio_hal_t)&gpio_hal_ports[j];
                    if (p_other->in_use_count == 0U)
                    {
                        continue;
                    }

                    for (uint32_t pin = 0U; pin <= 4U; pin++)
                    {
                        if (p_other->callbacks[pin].callback != NULL)
                        {
                            any0_4 = true;
                            break;
                        }
                    }
                    for (uint32_t pin = 5U; pin <= 9U; pin++)
                    {
                        if (p_other->callbacks[pin].callback != NULL)
                        {
                            any5_9 = true;
                            break;
                        }
                    }
                    for (uint32_t pin = 10U; pin <= 15U; pin++)
                    {
                        if (p_other->callbacks[pin].callback != NULL)
                        {
                            any10_15 = true;
                            break;
                        }
                    }

                    if (any0_4 && any5_9 && any10_15)
                    {
                        break;
                    }
                }

                if (!any0_4)
                {
                    NVIC_DisableIRQ(EXTI0_IRQN);
                    NVIC_DisableIRQ(EXTI1_IRQN);
                    NVIC_DisableIRQ(EXTI2_IRQN);
                    NVIC_DisableIRQ(EXTI3_IRQN);
                    NVIC_DisableIRQ(EXTI4_IRQN);
                }
                if (!any5_9)
                {
                    NVIC_DisableIRQ(EXTI9_5_IRQN);
                }
                if (!any10_15)
                {
                    NVIC_DisableIRQ(EXTI15_10_IRQN);
                }

                /* Disable GPIO port clock if no other handle is using this port */
                bool port_used = true;
                for (uint32_t j = 0U; j < NUMBER_GPIOS_PORTS; j++)
                {
                    if (gpio_hal_ports[j].in_use_count > 0U && gpio_hal_ports[j].port_number == p_handle->port_number)
                    {
                        port_used = true;
                        break;
                    }
                }

                if (!port_used)
                {
                    switch (p_handle->port_number)
                    {
                    case GPIOA_PORT:
                        p_device_rcc->ahb2enr &= ~RCC_AHB2ENR_GPIOAEN_BIT;
                        break;
                    case GPIOB_PORT:
                        p_device_rcc->ahb2enr &= ~RCC_AHB2ENR_GPIOBEN_BIT;
                        break;
                    case GPIOC_PORT:
                        p_device_rcc->ahb2enr &= ~RCC_AHB2ENR_GPIOCEN_BIT;
                        break;
                    case GPIOD_PORT:
                        p_device_rcc->ahb2enr &= ~RCC_AHB2ENR_GPIODEN_BIT;
                        break;
                    case GPIOE_PORT:
                        p_device_rcc->ahb2enr &= ~RCC_AHB2ENR_GPIOEEN_BIT;
                        break;
                    case GPIOH_PORT:
                        p_device_rcc->ahb2enr &= ~RCC_AHB2ENR_GPIOHEN_BIT;
                        break;
                    default:
                        break;
                    }
                }

                /* Clear handle bookkeeping */
                p_handle->port_number   = 0U;
                p_handle->p_device_gpio = NULL;
                p_handle->config_handle = NULL;

                /* Ensure callbacks array is cleared */
                for (uint32_t pin = 0U; pin < 16U; pin++)
                {
                    p_handle->callbacks[pin].callback          = NULL;
                    p_handle->callbacks[pin].p_callback_handle = NULL;
                    p_handle->callbacks[pin].callback_context  = NULL;
                }
            }
        }
    }

    void gpio_hal_init(p_gpio_hal_t p_handle)
    {

        if (p_handle != NULL)
        {
            switch (p_handle->port_number)
            {
            case GPIOA_PORT:
                p_device_rcc->ahb2enr |= RCC_AHB2ENR_GPIOAEN_BIT;
                break;
            case GPIOB_PORT:
                p_device_rcc->ahb2enr |= RCC_AHB2ENR_GPIOBEN_BIT;
                break;
            case GPIOC_PORT:
                p_device_rcc->ahb2enr |= RCC_AHB2ENR_GPIOCEN_BIT;
                break;
            case GPIOD_PORT:
                p_device_rcc->ahb2enr |= RCC_AHB2ENR_GPIODEN_BIT;
                break;
            case GPIOE_PORT:
                p_device_rcc->ahb2enr |= RCC_AHB2ENR_GPIOEEN_BIT;
                break;
            case GPIOH_PORT:
                p_device_rcc->ahb2enr |= RCC_AHB2ENR_GPIOHEN_BIT;
                break;
            default:
                assert(false); // invalid port
                break;
            }
        }
    }

    bool gpio_hal_pin_direction(p_gpio_hal_t p_handle, uint8_t pin, pin_direction_t value)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;

        if (p_gpio != NULL)
        {
            uint32_t shift = 2U * pin;
            uint32_t temp  = p_gpio->moder & ~(3U << shift);

            switch (value)
            {
            case PIN_DIRECTION_INPUT:
                p_gpio->moder = temp;
                break;
            case PIN_DIRECTION_OUTPUT:
                p_gpio->moder = temp | (1U << shift);
                break;
            case PIN_DIRECTION_ALT:
                p_gpio->moder = temp | (2U << shift);
                break;
            case PIN_DIRECTION_ANALOG:
                p_gpio->moder = temp | (3U << shift);
                break;
            default:
                return false;
            }
        }
        return true;
    }

    bool gpio_hal_set_alt_function(p_gpio_hal_t p_handle, uint8_t pin, uint8_t alt_func)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;

        if (p_gpio != NULL)
        {
            uint32_t reg_index = pin / 8U;        /* afr[0] for pins 0-7, afr[1] for pins 8-15 */
            uint32_t shift     = 4U * (pin % 8U); /* 4 bits per pin within the register        */
            uint32_t temp      = p_gpio->afr[reg_index] & ~(0x0FU << shift);

            p_gpio->afr[reg_index] = temp | ((alt_func & 0x0FU) << shift);
        }
        return true;
    }

    /*
    GPIO port pull-up/pull-down register (GPIOx_PUPDR)
    Bits 2y:2y+1 PUPDRy[1:0]: Port x configuration bits (y = 0..15)
    These bits are written by software to configure the I/O pull-up or pull-down
    00: No pull-up, pull-down
    01: Pull-up
    10: Pull-down
    11: Reserved

    GPIO port output type register (GPIOx_OTYPER)
    Bits 15:0 OTy: Port x configuration bits (y = 0..15)
    These bits are written by software to configure the output type of the I/O port.
    0: Output push-pull (reset state)
    1: Output open-drain
    */

    bool gpio_hal_pin_mode(p_gpio_hal_t p_handle, uint8_t pin, pin_mode_t value)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;

        if (p_gpio != NULL)
        {
            uint32_t shift = 2U * pin;
            uint32_t temp;

            switch (value)
            {
            case FLOAT:
                p_gpio->pupdr &= ~(3U << shift);
                break;
            case PULLUP:
                temp          = p_gpio->pupdr & ~(3U << shift);
                p_gpio->pupdr = temp | (1U << shift);
                break;
            case PULLDOWN:
                temp          = p_gpio->pupdr & ~(3U << shift);
                p_gpio->pupdr = temp | (2U << shift);
                break;
            case OPENDRAIN:
                p_gpio->otype |= (1U << pin);
                break;
            case PUSHPULL:
                p_gpio->otype &= ~(1U << pin);
                break;
            default:
                return false;
            }
        }
        return true;
    }

    /*
    GPIO port output speed register (GPIOx_OSPEEDR)
    Bits 2y:2y+1 OSPEEDRy[1:0]: Port x configuration bits (y = 0..15)
    These bits are written by software to configure the I/O output speed.
        00: Low speed
        01: Medium speed
        10: Fast speed
        11: High speed
    Note: Refer to the product datasheets for the values of OSPEEDRy bits versus VDD
    range and external load.
    */
    bool gpio_hal_pin_speed(p_gpio_hal_t p_handle, uint8_t pin, pin_speed_t value)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;

        if (p_gpio != NULL)
        {
            uint32_t shift = 2U * pin;
            uint32_t temp  = p_gpio->ospeedr & ~(3U << shift);

            switch (value)
            {
            case LOWSPEED:
                p_gpio->ospeedr = temp;
                break;
            case MEDIUMSPEED:
                p_gpio->ospeedr = temp | (1U << shift);
                break;
            case FASTSPEED:
                p_gpio->ospeedr = temp | (2U << shift);
                break;
            case HIGHSPEED:
                p_gpio->ospeedr = temp | (3U << shift);
                break;
            default:
                return false;
            }
        }
        return true;
    }

    void gpio_hal_set_state(p_gpio_hal_t p_handle, uint8_t pin, bool value)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        /* Use BSRR for atomic set/reset - no read-modify-write needed */
        if (value)
        {
            p_gpio->bsrr = (1U << pin); /* Set bit */
        }
        else
        {
            p_gpio->bsrr = (1U << (pin + 16U)); /* Reset bit */
        }
    }

    bool gpio_hal_get_state(p_gpio_hal_t p_handle, uint8_t pin)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        uint32_t    mode   = (p_gpio->moder >> (2U * pin)) & 3U;

        if (mode == PIN_DIRECTION_INPUT)
        {
            return (p_gpio->idr & (1U << pin)) != 0u;
        }
        else if (mode == PIN_DIRECTION_OUTPUT)
        {
            return (p_gpio->odr & (1U << pin)) != 0u;
        }

        return false;
    } /*lint !e818*/

    void gpio_hal_toggle_state(p_gpio_hal_t p_handle, uint8_t pin)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        p_gpio->odr ^= (1U << pin);

    } /*lint !e818*/

    void gpio_hal_write_port(p_gpio_hal_t p_handle, uint8_t value)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        p_gpio->odr        = (uint32_t)value;
    }

    uint8_t gpio_hal_read_port(p_gpio_hal_t p_handle)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        return (p_gpio->odr & 0xFFu);
    }

    bool gpio_hal_register_callback(p_gpio_hal_t p_handle, const void *p_callback_handle, uint8_t pin,
                                    gpio_hal_interrupt_callback_t callback, void *p_callback_context, irq_edge_t edge)
    {

        if (p_handle == NULL || callback == NULL)
        {
            return false;
        }
        p_handle->callbacks[pin].callback          = callback;
        p_handle->callbacks[pin].p_callback_handle = (void *)p_callback_handle;
        p_handle->callbacks[pin].callback_context  = p_callback_context;

        // p_handle->irq_edge          = edge;

        p_device_rcc->apb2enr |= RCC_APB2ENR_SYSCFGEN_BIT;
        p_device_syscfg->exticr[pin / 4U] &= ~(0x0FU << (4U * (pin % 4U))); // Clear EXTI configuration bits for the pin
        p_device_syscfg->exticr[pin / 4U] |=
            ((p_handle->port_number & 0x0FU) << (4U * (pin % 4U))); // Set EXTI configuration bits for the pin
        p_device_exti->imr1 |= (1U << pin);                         // Unmask interrupt for the pin
        switch (edge)
        {
        case IRQ_POSITIVE:
            p_device_exti->rtsr1 |= (1U << pin);  // Enable rising edge trigger
            p_device_exti->ftsr1 &= ~(1U << pin); // Disable falling edge trigger
            break;
        case IRQ_NEGATIVE:
            p_device_exti->ftsr1 |= (1U << pin);  // Enable falling edge trigger
            p_device_exti->rtsr1 &= ~(1U << pin); // Disable rising edge trigger
            break;
        case IRQ_BOTH:
            p_device_exti->rtsr1 |= (1U << pin); // Enable rising edge trigger
            p_device_exti->ftsr1 |= (1U << pin); // Enable falling edge trigger
            break;
        default:
            return false;
            break;
            ;
        }
        if (pin == 0U)
        {
            NVIC_EnableIRQ(EXTI0_IRQN);
            NVIC_SetPriority(EXTI0_IRQN, BUTTON_IRQ_PRIORITY);
        }
        else if (pin == 1U)
        {
            NVIC_EnableIRQ(EXTI1_IRQN);
            NVIC_SetPriority(EXTI1_IRQN, BUTTON_IRQ_PRIORITY);
        }
        else if (pin == 2U)
        {
            NVIC_EnableIRQ(EXTI2_IRQN);
            NVIC_SetPriority(EXTI2_IRQN, BUTTON_IRQ_PRIORITY);
        }
        else if (pin == 3U)
        {
            NVIC_EnableIRQ(EXTI3_IRQN);
            NVIC_SetPriority(EXTI3_IRQN, BUTTON_IRQ_PRIORITY);
        }
        else if (pin == 4U)
        {
            NVIC_EnableIRQ(EXTI4_IRQN);
            NVIC_SetPriority(EXTI4_IRQN, BUTTON_IRQ_PRIORITY);
        }
        else if (pin <= 9U)
        {
            NVIC_EnableIRQ(EXTI9_5_IRQN);
            NVIC_SetPriority(EXTI9_5_IRQN, BUTTON_IRQ_PRIORITY);
        }
        else if (pin <= 15U)
        {
            NVIC_EnableIRQ(EXTI15_10_IRQN);
            NVIC_SetPriority(EXTI15_10_IRQN, BUTTON_IRQ_PRIORITY);
        }
        else
        {
            return false; // invalid pin
        }

        return true;
    }
#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
