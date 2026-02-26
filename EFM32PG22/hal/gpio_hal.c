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
#include <clock_hal.h>
#include <device_reg.h>
#include <gpio_hal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /*lint -e129*/

#include <assert.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
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
        bool  bOutput[8];       /**< Track which pins are outputs */

        callback_context_t callbacks[GPIOS_INTERRUPTS]; /**< Registered interrupt callbacks */

        // gpio_hal_interrupt_callback_t callback;          /**< Registered interrupt callback */
        // void                         *p_callback_handle; /**< Pointer to callback context (i.e.
        // Button handle)*/ void                         *callback_context;  /**< User callback
        // context */
    };

    static gpio_hal_t gpio_hal_ports[NUMBER_GPIOS_PORTS] = {0};

    /********************************************************************************
     * Functions
     ********************************************************************************/
    static inline void handle_irq(uint8_t pin)
    {
        //(void)pin;

        p_gpio_hal_t p_handle = &gpio_hal_ports[0];

        // Call the registered callback
        for (uint8_t i = 0; i < NUMBER_GPIOS_PORTS; i++)
        {
            p_handle = &gpio_hal_ports[i];
            if (p_handle->callbacks[pin].callback != NULL)
            {
                p_handle->callbacks[pin].callback(p_handle->callbacks[pin].p_callback_handle,
                                                  p_handle->callbacks[pin].callback_context);
            }
        }
    }

    /*
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
*/
    void GPIO_EVEN_IRQHandler(void)
    {
        uint32_t flags = GPIO->IF & GPIO->IEN;

        // Clear handled flags
        GPIO->IF_CLR = flags;

        if (flags & (1 << 0))
        {
            // Button pressed
        }
    }

    void GPIO_ODD_IRQHandler(void)
    {
        uint32_t flags = GPIO->IF & GPIO->IEN;

        for (uint8_t i = 0; i <= 8; i++)
        {
            if (flags & (1 << i))
            {
                handle_irq(i);
            }
        }

        // Clear handled flags
        GPIO->IF_CLR = flags;
    }

    p_gpio_hal_t gpio_hal_create(uint32_t port) // setup GPIO HAL instance
    {
        p_gpio_hal_t p_free_slot = NULL;

        /* Single pass: check for existing instance and track first free slot */
        for (uint32_t i = 0; i < NUMBER_GPIOS_PORTS; i++)
        {
            if (gpio_hal_ports[i].in_use_count > 0u)
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
            p_free_slot->in_use_count++;
            p_free_slot->port_number   = port;
            p_free_slot->p_device_gpio = GPIO;
            clock_hal_enable(CLOCK_GPIO, true);
            // p_device_cmu->clken0 |= (1U << (_CMU_CLKEN0_GPIO_SHIFT)); // enable clock for GPIO
            // port
        }

        return p_free_slot;
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
            /* Disable any registered interrupts for all pins on this port */
            for (uint8_t pin = 0u; pin < GPIOS_INTERRUPTS; pin++)
            {
                if (p_handle->callbacks[pin].callback != NULL)
                {
                    /* Disable the interrupt for this pin */
                    GPIO->IEN &= ~(1u << pin);
                    GPIO->IF_CLR = (1u << pin);

                    /* Clear edge detection */
                    GPIO->EXTIRISE &= ~(1u << pin);
                    GPIO->EXTIFALL &= ~(1u << pin);

                    /* Clear the callback */
                    p_handle->callbacks[pin].callback          = NULL;
                    p_handle->callbacks[pin].p_callback_handle = NULL;
                    p_handle->callbacks[pin].callback_context  = NULL;
                }
            }

            /* Reset all pins on this port to disabled (default) mode */
            GPIO->P[p_handle->port_number].MODEL = 0u;
            GPIO->P[p_handle->port_number].MODEH = 0u;
            GPIO->P[p_handle->port_number].DOUT  = 0u;

            /* Clear output tracking */
            for (uint8_t i = 0u; i < 8u; i++)
            {
                p_handle->bOutput[i] = false;
            }

            /* Clear device pointer */
            p_handle->p_device_gpio = NULL;
            p_handle->port_number   = 0u;

            clock_hal_enable(CLOCK_GPIO, false);
        }
    }

    void gpio_hal_init(p_gpio_hal_t p_handle)
    {
        (void)p_handle;
    }

    /*



    */
    bool gpio_hal_pin_direction(p_gpio_hal_t p_handle, uint8_t pin, pin_direction_t value)
    {
        GPIO_TypeDef *p_gpio = (GPIO_TypeDef *)p_handle->p_device_gpio;

        if (p_gpio != NULL)
        {
            uint32_t mode = 0u;

            switch (value)
            {
            case PIN_DIRECTION_INPUT:
                mode = _GPIO_P_MODEL_MODE0_INPUT;
                break;
            case PIN_DIRECTION_OUTPUT:
                mode = _GPIO_P_MODEL_MODE0_PUSHPULL;
                break;
            case PIN_DIRECTION_ALT:
                mode = _GPIO_P_MODEL_MODE0_PUSHPULLALT;
                break;
                // not sure for now, so drop through
            default:
                assert(false); /*lint !e506 */
                break;
            }

            if (pin < 8u)
            {
                // Cast parameter [mode] to 32 bits to fix C99 Undefined Behavior (see SEI CERT C
                // INT34-C) Compiler assigned 8 bits for enum. Same thing for other branch.
                device_reg_masked_write(&(GPIO->P[p_handle->port_number].MODEL), 0xFu << (pin * 4u),
                                        (uint32_t)mode << (pin * 4u));
            }
            else
            {
                device_reg_masked_write(&(GPIO->P[p_handle->port_number].MODEH),
                                        0xFu << ((pin - 8u) * 4u),
                                        (uint32_t)mode << ((pin - 8u) * 4u));
            }
        }
        if (value == PIN_DIRECTION_OUTPUT)
        {
            p_handle->bOutput[pin] = true;
        }
        else
        {
            p_handle->bOutput[pin] = false;
        }
        return true;
    }

    bool gpio_hal_pin_mode(p_gpio_hal_t p_handle, uint8_t pin, pin_mode_t value)
    {
        GPIO_TypeDef *p_gpio = (GPIO_TypeDef *)p_handle->p_device_gpio;
        uint32_t      mode   = 0u;

        if (p_gpio != NULL)
        {
            switch (value)
            {
            case FLOAT:
                return true;
                break;
            case PULLUP:
                if (p_handle->bOutput[pin])
                {
                    mode = _GPIO_P_MODEL_MODE0_WIREDOR; // output pull-up (WIREDOR)
                }
                else
                {
                    mode = _GPIO_P_MODEL_MODE0_INPUTPULL; // input pull-up
                    GPIO->P[p_handle->port_number].DOUT = (1 << pin);
                }

                break;
            case PULLDOWN:
                if (p_handle->bOutput[pin])
                {
                    mode = _GPIO_P_MODEL_MODE0_WIREDORPULLDOWN; // output pull-down
                }
                else
                {
                    mode = _GPIO_P_MODEL_MODE0_INPUTPULL; // pull-down
                }
                break;
            case OPENDRAIN:
                if (p_handle->bOutput[pin])
                {
                    mode = _GPIO_P_MODEL_MODE0_WIREDAND; // open drain
                }

                break;
            default:
                assert(false); /*lint !e506 */
                break;
            }

            if (pin < 8u)
            {
                // Cast parameter [mode] to 32 bits to fix C99 Undefined Behavior (see SEI CERT C
                // INT34-C) Compiler assigned 8 bits for enum. Same thing for other branch.
                // device_reg_masked_write(&(GPIO->P[p_handle->port_number].MODEL), 0xFu << (pin *
                // 4u),
                //                        (uint32_t)mode << (pin * 4u));
                GPIO->P[p_handle->port_number].MODEL &= ~(0xFu << (pin * 4u));
                GPIO->P[p_handle->port_number].MODEL |= (uint32_t)mode << (pin * 4u);
            }
            else
            {
                GPIO->P[p_handle->port_number].MODEH &= ~(0xFu << ((pin - 8u) * 4u));
                GPIO->P[p_handle->port_number].MODEH |= (uint32_t)mode << ((pin - 8u) * 4u);

                // device_reg_masked_write(&(GPIO->P[p_handle->port_number].MODEH),
                //                         0xFu << ((pin - 8u) * 4u),
                //                         (uint32_t)mode << ((pin - 8u) * 4u));
            }

            return true;
        }
        return false;
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
        GPIO_TypeDef *p_gpio = (GPIO_TypeDef *)p_handle->p_device_gpio;

        if (p_gpio != NULL)
        {
            if (value)
            {
                GPIO->P_SET[p_handle->port_number].DOUT = 1u << pin;
            }
            else
            {
                GPIO->P_CLR[p_handle->port_number].DOUT = 1u << pin;
            }
        }
    }

    bool gpio_hal_get_state(p_gpio_hal_t p_handle, uint8_t pin)
    {
        if (!p_handle->bOutput[pin])
        {

            return GPIO->P[p_handle->port_number].DIN >> pin & 1u;
            // device_reg_bit_read(&GPIO->P[p_handle->port_number].DIN, pin);
        }
        else
        {
            return (GPIO->P[p_handle->port_number].DOUT) >> pin & 1u;
        }

    } /*lint !e818*/

    void gpio_hal_toggle_state(p_gpio_hal_t p_handle, uint8_t pin)
    {
        GPIO->P_TGL[p_handle->port_number].DOUT = 1UL << pin;

    } /*lint !e818*/

    void gpio_hal_write_port(p_gpio_hal_t p_handle, uint8_t value)
    {
        GPIO->P[p_handle->port_number].DOUT = value;
    }

    uint8_t gpio_hal_read_port(p_gpio_hal_t p_handle)
    {
        return (GPIO->P_CLR[p_handle->port_number].DIN & 0xFFu);
    }

    bool gpio_hal_register_callback(p_gpio_hal_t p_handle, const void *p_callback_handle,
                                    uint8_t pin, gpio_hal_interrupt_callback_t callback,
                                    void *p_callback_context, irq_edge_t edge)
    {

        GPIO->LOCK = GPIO_LOCK_LOCKKEY_UNLOCK;

#if 0
        // PB03 input pull-up
        // GPIO->P[1].MODEL &= ~_GPIO_P_MODEL_MODE3_MASK;
        // GPIO->P[1].MODEL |= GPIO_P_MODEL_MODE3_INPUT;
        // GPIO->P[1].DOUT |= (1 << 3);

        // Route PB03 → EXTI3
        GPIO->EXTIPSELL   = 0x1000; // Port B
        GPIO->EXTIPINSELL = (3 << _GPIO_EXTIPINSELL_EXTIPINSEL3_SHIFT);

        // Falling edge
        GPIO->EXTIRISE &= ~(1 << 3);
        GPIO->EXTIFALL |= (1 << 3);

        // Enable interrupt
        GPIO->IF_CLR = (1 << 3);
        GPIO->IEN |= (1 << 3);

        // NVIC (odd pin)
        NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
        NVIC_EnableIRQ(GPIO_ODD_IRQn);

        __enable_irq();
#endif
#if 1

        // GPIO_TypeDef *p_gpio = (GPIO_TypeDef *)p_handle->p_device_gpio;

        if (p_handle == NULL || callback == NULL)
        {
            return false;
        }
        // set up the callback
        p_handle->callbacks[pin].callback          = callback;
        p_handle->callbacks[pin].p_callback_handle = (void *)p_callback_handle;
        p_handle->callbacks[pin].callback_context  = p_callback_context;

#if defined(_GPIO_EXTIPSELH_MASK)
        uint32_t tmp = 0u;
#endif
        // GPIO->EXTIPSELL |= GPIO_EXTIPSELL_EXTIPSEL3_PORTB;

#if 1

        /* The EXTIPSELL register controls pins 0-7 and EXTIPSELH controls
         * pins 8-15 of the interrupt configuration. */
        if (pin < 8u)
        {
            device_reg_masked_write(
                &GPIO->EXTIPSELL,
                _GPIO_EXTIPSELL_EXTIPSEL0_MASK
                    << (_GPIO_EXTIPSELL_EXTIPSEL1_SHIFT * pin),                    /*lint !e9117 */
                p_handle->port_number << (_GPIO_EXTIPSELL_EXTIPSEL1_SHIFT * pin)); /*lint !e9117 */

            // not sure I understand this line, but it somehow enables the pin for the irq
            GPIO->EXTIPINSELL = ((pin % 4) << _GPIO_EXTIPINSELL_EXTIPINSEL1_SHIFT * pin);
        }
        else
        {

            tmp = pin - 8u;

            device_reg_masked_write(
                &GPIO->EXTIPSELH,
                _GPIO_EXTIPSELH_EXTIPSEL0_MASK
                    << (_GPIO_EXTIPSELH_EXTIPSEL1_SHIFT * tmp),                    /*lint !e9117 */
                p_handle->port_number << (_GPIO_EXTIPSELH_EXTIPSEL1_SHIFT * tmp)); /*lint !e9117 */
            GPIO->EXTIPINSELL = ((tmp % 4) << _GPIO_EXTIPINSELL_EXTIPINSEL1_SHIFT * tmp);
        }

        // Falling edge
        GPIO->EXTIRISE &= ~(1 << pin);
        GPIO->EXTIFALL |= (1 << pin);

        // Enable interrupt
        GPIO->IF_CLR = (1 << pin);
        GPIO->IEN |= (1 << pin);

        // NVIC (odd pin)
        if (pin % 2u)
        {
            NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
            NVIC_SetPriority(GPIO_ODD_IRQn, 3); // Add this
            NVIC_EnableIRQ(GPIO_ODD_IRQn);
        }
        else
        {
            NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
            NVIC_SetPriority(GPIO_EVEN_IRQn, 3); // Add this
            NVIC_EnableIRQ(GPIO_EVEN_IRQn);
        }
#endif

/* The EXTIPINSELL register controls interrupt 0-7 and EXTIPINSELH controls
 * interrupt 8-15 of the interrupt/pin number mapping. */
#if 0
        if (pin < 8u)
        {
            device_reg_masked_write(
                &GPIO->EXTIPINSELL,
                _GPIO_EXTIPINSELL_EXTIPINSEL0_MASK
                    << (_GPIO_EXTIPINSELL_EXTIPINSEL1_SHIFT * pin),  /*lint !e9117 */
                ((pin % 4) & _GPIO_EXTIPINSELL_EXTIPINSEL0_MASK)     /*lint !e9117 */
                    << (_GPIO_EXTIPINSELL_EXTIPINSEL1_SHIFT * pin)); /*lint !e9117 */
        }
#endif
        /* Enable/disable the rising edge interrupt. */
        if ((edge == IRQ_POSITIVE) || (edge == IRQ_BOTH))
        {
            // device_reg_masked_write(&(GPIO->EXTIRISE), pin, 1u);
            GPIO->EXTIRISE |= (1 << pin);
        }
        /* Enable/disable the falling edge interrupt. */
        if ((edge == IRQ_NEGATIVE) || (edge == IRQ_BOTH))
        {
            // device_reg_masked_write(&(GPIO->EXTIFALL), pin, 1u);
            GPIO->EXTIFALL |= (1 << pin);
        }

        /* Clear any pending interrupt. */
        GPIO->IF_CLR = (1U << pin);
        GPIO->IEN |= (1U << pin);
        /* Finally enable/disable interrupt. */
        // device_reg_masked_write(&(GPIO->IEN), pin, true);

#endif
        return true;
    }
#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */