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
#include <device_sysctl.h>
#include <gpio_hal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /*lint -e129*/

#include <assert.h>
#include <device_irq.h>
#include <tm4c123gh6pm.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
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
        bool     in_use;        /**< Indicates if this GPIO port instance is in use */
        uint32_t port_number;   /**< The GPIO port number */
        void    *config_handle; /**< Pointer to platform-specific context passed to the
                                   HAL implementation. */
        void *p_device_gpio;    /**< Pointer to the GPIO port device registers. */

        callback_context_t callbacks[GPIOS_INTERRUPTS]; /**< Registered interrupt callbacks */

        // gpio_hal_interrupt_callback_t callback;          /**< Registered interrupt callback */
        // void                         *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
        // void                         *callback_context;  /**< User callback context */
    };

    static gpio_hal_t gpio_hal_ports[NUMBER_GPIOS_PORTS] = {0};

    /********************************************************************************
     * Functions
     ********************************************************************************/
    static inline void handle_irq(uint8_t pin)
    {

        p_gpio_hal_t p_handle = &gpio_hal_ports[0];
        reg_gpio_t  *p_gpio   = (reg_gpio_t *)p_handle->p_device_gpio;
        // uint32_t     pending  = p_gpio->is;

        // Call the registered callback
        if (p_handle->callbacks[pin].callback != NULL)
        {
            p_handle->callbacks[pin].callback(p_handle->callbacks[pin].p_callback_handle,
                                              p_handle->callbacks[pin].callback_context);
        }
        // NVIC_ClearPendingIRQ(PIO0_0_IRQn + pin);
        if (p_gpio->mis & (1 << pin))
        {
            p_gpio->icr |= (1U << pin);
        }
    }

    void GpioFIntHandler(void)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if (((reg_gpio_t *)gpio_hal_ports[0].p_device_gpio)->mis & (1 << i))
            {
                handle_irq(i);
            }
        }
    }

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
        case GPIOF_PORT:
            return p_device_gpio_f;
        default:
            // assert(false); // invalid port
            return NULL;
        }
    }

    p_gpio_hal_t gpio_hal_create(uint32_t port) // setup GPIO HAL instance
    {
        p_gpio_hal_t p_free_slot = NULL;

        /* Single pass: check for existing instance and track first free slot */
        for (uint32_t i = 0; i < NUMBER_GPIOS_PORTS; i++)
        {
            if (gpio_hal_ports[i].in_use == true)
            {
                if (gpio_hal_ports[i].port_number == port)
                {
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
            p_free_slot->in_use        = true;
            p_free_slot->port_number   = port;
            p_free_slot->p_device_gpio = get_gpio_registers(port);
        }

        return p_free_slot;
    } /*lint !e818*/

    void gpio_hal_init(p_gpio_hal_t p_handle)
    {

        if (p_handle != NULL)
        {
            switch (p_handle->port_number)
            {
            case GPIOA_PORT:
                p_device_sysctl->rcgcgpio |= SYSCON_RCGCGPIO_PORTA_BIT;
                break;
            case GPIOB_PORT:
                p_device_sysctl->rcgcgpio |= SYSCON_RCGCGPIO_PORTB_BIT;
                break;
            case GPIOC_PORT:
                p_device_sysctl->rcgcgpio |= SYSCON_RCGCGPIO_PORTC_BIT;
                break;
            case GPIOD_PORT:
                p_device_sysctl->rcgcgpio |= SYSCON_RCGCGPIO_PORTD_BIT;
                break;
            case GPIOE_PORT:
                p_device_sysctl->rcgcgpio |= SYSCON_RCGCGPIO_PORTE_BIT;
                break;
            case GPIOF_PORT:
                p_device_sysctl->rcgcgpio |= SYSCON_RCGCGPIO_PORTF_BIT;
                break;
            default:
                // assert(false); // invalid port
                break;
            }
        }
        /*
        if (p_gpio_hal->config_handle != NULL)
        {
            p_gpio_hal_config_handle_t p_config = (p_gpio_hal_config_handle_t)p_gpio_hal->config_handle;
            reg_gpio_t                *p_gpio   = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
            // Enable pins
            p_gpio->den &= ~p_config->pin_mask;  // Clear bits that are not to be used
            p_gpio->den |= (p_config->pin_mask); // Set bits to enable digital function

            // Configure pin directions
            p_gpio->dir &= ~p_config->pin_mask;                             // Clear bits to set as input
            p_gpio->dir |= (p_config->direction_mask & p_config->pin_mask); // Set bits to set as output
        }
            */
    }

    /*

    GPIO Direction (GPIODIR)
    The GPIODIR register is the data direction register. Setting a bit in the GPIODIR register configures
    the corresponding pin to be an output, while clearing a bit configures the corresponding pin to be
    an input. All bits are cleared by a reset, meaning all GPIO pins are inputs by default.
    Bit/Field   Name    Type     Reset        Description
    31:8        reserved RO      0x0000.00    Software should not rely on the value of a reserved bit. To provide
                                              compatibility with future products, the value of a reserved bit should
    be preserved across a read-modify-write operation.

    7:0         DIR      RW      0x00         GPIO Data Direction
                                              Value       Description
                                              0           Corresponding pin is an input.
                                              1           Corresponding pins is an output.

    */
    bool gpio_hal_pin_direction(p_gpio_hal_t p_handle, uint8_t pin, pin_direction_t value)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        p_gpio->den |= (1U << pin); // Set bits to enable digital function
        if (p_gpio != NULL)
        {
            if (value == PIN_DIRECTION_OUTPUT)
            {
                p_gpio->dir |= (1U << pin);
            }
            else
            {
                p_gpio->dir &= ~(1U << pin);
            }
        }
        return true;
    }

    bool gpio_hal_pin_mode(p_gpio_hal_t p_handle, uint8_t pin, pin_mode_t value)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        switch (value)
        {
        case PULLUP:
            p_gpio->pur |= (1U << pin);
            p_gpio->pdr &= ~(1U << pin);
            break;
        case PULLDOWN:
            p_gpio->pdr |= (1U << pin);
            p_gpio->pur &= ~(1U << pin);
            break;
        case OPENDRAIN:
            p_gpio->odr |= (1U << pin);
            break;
        case FLOAT:
            p_gpio->odr &= ~(1U << pin);
            p_gpio->pur &= ~(1U << pin);
            p_gpio->pdr &= ~(1U << pin);
            break;
        default:
            return false;
        }

        return true;
    }

    bool gpio_hal_pin_speed(p_gpio_hal_t p_handle, uint8_t pin, pin_speed_t value)
    {
        (void)p_handle;
        (void)pin;
        (void)value;
        return true;
    }

    void gpio_hal_set_state(p_gpio_hal_t p_handle, uint8_t pin, bool value)
    {
        reg_gpio_t *p_gpio = p_handle->p_device_gpio;
        if (value)
        {
            p_gpio->data |= (1U << pin);
        }
        else
        {
            p_gpio->data &= ~(1U << pin);
        }
    }

    bool gpio_hal_get_state(p_gpio_hal_t p_handle, uint8_t pin)
    {
        reg_gpio_t *p_gpio = p_handle->p_device_gpio;
        return (p_gpio->data & (1U << pin)) != 0u;

    } /*lint !e818*/

    void gpio_hal_toggle_state(p_gpio_hal_t p_handle, uint8_t pin)
    {
        reg_gpio_t *p_gpio = p_handle->p_device_gpio;
        p_gpio->data ^= (1U << pin);

    } /*lint !e818*/

    void gpio_hal_write_port(p_gpio_hal_t p_handle, uint8_t value)
    {
        reg_gpio_t *p_gpio = p_handle->p_device_gpio;
        p_gpio->data       = (uint32_t)value;
    }

    uint8_t gpio_hal_read_port(p_gpio_hal_t p_handle)
    {
        reg_gpio_t *p_gpio = p_handle->p_device_gpio;
        return (p_gpio->data & 0xFFu);
    }

    bool gpio_hal_register_callback(p_gpio_hal_t p_handle, const void *p_callback_handle, uint8_t pin,
                                    gpio_hal_interrupt_callback_t callback, void *p_callback_context, irq_edge_t edge)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        if (p_handle == NULL || callback == NULL)
        {
            return false;
        }
        p_handle->callbacks[pin].callback          = callback;
        p_handle->callbacks[pin].p_callback_handle = (void *)p_callback_handle;
        p_handle->callbacks[pin].callback_context  = p_callback_context;

        GPIO_PORTF_LOCK_R = 0x4C4F434B; // Unlock GPIO Port F
        GPIO_PORTF_CR_R |= (1 << 4);

        if (edge == IRQ_NONE)
        {
            // Disable interrupt
            p_gpio->im &= ~(1U << pin);
            return true;
        }

        p_gpio->is &= ~(1U << pin);  // Edge sensitive
        p_gpio->ibe &= ~(1U << pin); // Single edge

        if (edge == IRQ_POSITIVE)
        {
            p_gpio->iev |= (1U << pin); // Rising edge
        }
        else if (edge == IRQ_NEGATIVE)
        {
            p_gpio->iev &= ~(1U << pin); // Falling edge
        }
        else if (edge == IRQ_BOTH)
        {
            p_gpio->ibe |= (1U << pin); // Both edges
        }

        // p_handle->irq_edge          = edge;
        p_gpio->mis |= (1U << pin); // Enable interrupt
        p_gpio->icr |= (1U << pin); // Enable interrupt
        p_gpio->im |= (1U << pin);  // Enable interrupt
        // NVIC_ClearPendingIRQ(PIO0_IRQn);
        // NVIC_EnableIRQ(PIO0_IRQn);
        NVIC_EN0_R |= (1 << 30); // Enable IRQ30 (GPIOF)
        __enable_irq();

        return true;
    }

#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */