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
 * @details Outlines the portable GPIO abstraction used to configure and
 * interact with digital pins on any supported platform.
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
#include "device_gpio.h"
#include "device_iocon.h"
#include "device_irq.h"
#include "device_syscon.h"
#include <assert.h>
#include <gpio_hal.h>
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /*lint -e129*/

    /********************************************************************************
     * Defines
     ********************************************************************************/
    // support for GPIO interrupt callbacks
    typedef struct callback_context_s
    {                                           /**< The GPIO pin number */
        gpio_hal_interrupt_callback_t callback; /**< Registered interrupt callback */
        void *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
        void *callback_context;  /**< User callback context */
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

        callback_context_t callbacks[GPIOS_INTERRUPTS];
    };

    static gpio_hal_t gpio_hal_ports[NUMBER_GPIOS_PORTS] = {0};

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Functions
     ********************************************************************************/
    void irq_handler_common(uint8_t pin)
    {
        p_gpio_hal_t p_handle = &gpio_hal_ports[0];
        reg_gpio_t  *p_gpio   = (reg_gpio_t *)p_handle->p_device_gpio;
        // uint32_t     pending  = p_gpio->is;

        // Clear the interrupt

        // Call the registered callback
        if (p_handle->callbacks[pin].callback != NULL)
        {
            p_handle->callbacks[pin].callback(p_handle->callbacks[pin].p_callback_handle,
                                              p_handle->callbacks[pin].callback_context);
        }
        // NVIC_ClearPendingIRQ(PIO0_0_IRQn + pin);
        p_gpio->ic |= (1U << pin);
    }

    void PIO0_IRQHandler(void)
    {
        p_gpio_hal_t p_handle = &gpio_hal_ports[0];
        reg_gpio_t  *p_gpio   = (reg_gpio_t *)p_handle->p_device_gpio;
        uint32_t     status   = p_gpio->mis;

        for (uint8_t pin = 0; pin < 10; pin++)
        {
            if (status & (1U << pin))
            {
                irq_handler_common(pin);
            }
        }
    }

    p_gpio_hal_t gpio_hal_create(uint32_t port)
    {
        const uint32_t i = 0;
        if (gpio_hal_ports[i].in_use_count == 0)
        {
            p_device_gpio_a->imse = 0; // Disable all interrupts initially
        }

        // There is only one GPIO port on the NHS3100
        gpio_hal_ports[i].in_use_count++;
        gpio_hal_ports[i].port_number   = port;
        gpio_hal_ports[i].p_device_gpio = (void *)p_device_gpio_a;

        return (p_gpio_hal_t)&gpio_hal_ports[i];

    } /*lint !e818*/

    void gpio_hal_remove(p_gpio_hal_t p_handle)
    {
        if (p_handle == NULL)
        {
            return;
        }
        if (p_handle->in_use_count > 0u)
        {
            p_handle->in_use_count--;
        }
        if (p_handle->in_use_count == 0u)
        {
            reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;

            if (p_gpio != NULL)
            {

                /* Clear device pointer */
                p_handle->p_device_gpio = NULL;
                p_handle->port_number   = 0u;
            }
        }
    }

    void gpio_hal_init(p_gpio_hal_t p_handle)
    {
        // assert(p_handle != NULL);
        p_gpio_hal_t p_gpio_hal = p_handle;
        if (p_gpio_hal != NULL)
        {
            p_syscon->ahbclkctrl |= (SYSCON_AHBCLKCTRL_GPIO_BIT | SYSCON_AHBCLKCTRL_IOCON_BIT) &
                                    SYSCON_AHBCLKCTRL_BITS_THAT_CAN_BE_SET; // Enable GPIO and
                                                                            // IOCON clock
        }
    }

    bool gpio_hal_pin_direction(p_gpio_hal_t p_handle, uint8_t pin, pin_direction_t direction)
    {

        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        if (direction == PIN_DIRECTION_OUTPUT)
        {
            p_gpio->dir |= (1U << pin);
        }
        else
        {
            p_gpio->dir &= ~(1U << pin);
        }

        return true;

    } /*lint !e818*/

    bool gpio_hal_pin_mode(p_gpio_hal_t p_handle, uint8_t pin, pin_mode_t value)
    {

        uint32_t mode = 0;
        switch (value)
        {
        case FLOAT:

            // Mode is already 0 for floating
            break;
        case PULLUP:
            mode |= IOCON_MODE_PULLUP;
            // gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
            break;
        case PULLDOWN:
            mode |= IOCON_MODE_PULLDOWN;
            // gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
            break;
        case OPENDRAIN:
            // Open-drain not directly supported in ESP32 GPIO API
            // Custom handling may be required here
            break;
        case PUSHPULL:
            // Push-pull is the default mode for output pins
            break;
        default:
            return false;
        }

        volatile uint32_t *p_iocon_reg = &p_iocon->pio0_0 + pin;
        *p_iocon_reg                   = IOCON_FUNC_GPIO | mode;
        return true;
    }

    bool gpio_hal_pin_speed(p_gpio_hal_t p_handle, uint8_t pin, pin_speed_t value)

    {

        (void)p_handle;
        (void)pin;
        (void)value;
        /* NHS3100 GPIO does not have direct speed settings; this is a placeholder */
        return true;
    }

    void gpio_hal_set_state(p_gpio_hal_t p_handle, uint8_t pin, bool value)
    {

        reg_gpio_t *p_gpio     = (reg_gpio_t *)p_handle->p_device_gpio;
        p_gpio->data[1 << pin] = (uint32_t)value ? 1U << pin : 0U;
        // NSS_GPIO[p_gpio->port].DATA[1u << p_gpio->pin] = (uint32_t)state <<
        // p_gpio->pin;

    } /*lint !e818*/

    bool gpio_hal_get_state(p_gpio_hal_t p_handle, uint8_t pin)
    {

        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        return (p_gpio->data[1 << pin]) != 0u;
        // return (NSS_GPIO[p_gpio->port].DATA[1u << p_gpio->pin]) != 0u;
    } /*lint !e818*/

    void gpio_hal_toggle_state(p_gpio_hal_t p_handle, uint8_t pin)
    {

        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        p_gpio->data[1 << pin] ^= (1u << pin);
        // NSS_GPIO[p_gpio->port].DATA[1u << p_gpio->pin] ^= (1u <<
        // p_gpio->pin);
    } /*lint !e818*/

    void gpio_hal_write_port(p_gpio_hal_t p_handle, uint8_t value)
    {
        reg_gpio_t *p_gpio  = (reg_gpio_t *)p_handle->p_device_gpio;
        p_gpio->data[0xfff] = (uint32_t)value;
        // p_gpio_hal->p_device_gpio->data = (uint32_t)value;
    }

    uint8_t gpio_hal_read_port(p_gpio_hal_t p_handle)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        return (p_gpio->data[0xfff]) & 0xFF;
    }

    bool gpio_hal_register_callback(p_gpio_hal_t p_handle, const void *p_callback_handle,
                                    uint8_t pin, gpio_hal_interrupt_callback_t callback,
                                    void *p_callback_context, irq_edge_t edge)
    {
        reg_gpio_t *p_gpio = (reg_gpio_t *)p_handle->p_device_gpio;
        if (p_handle == NULL || callback == NULL)
        {
            return false;
        }
        p_handle->callbacks[pin].callback          = callback;
        p_handle->callbacks[pin].p_callback_handle = (void *)p_callback_handle;
        p_handle->callbacks[pin].callback_context  = p_callback_context;

        if (edge == IRQ_NONE)
        {
            // Disable interrupt
            p_gpio->imse &= ~(1U << pin);
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
        p_gpio->mis |= (1U << pin);  // Enable interrupt
        p_gpio->ic |= (1U << pin);   // Enable interrupt
        p_gpio->imse |= (1U << pin); // Enable interrupt
        NVIC_ClearPendingIRQ(PIO0_IRQn);
        NVIC_EnableIRQ(PIO0_IRQn);
        __enable_irq();

        return true;
    }
#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
