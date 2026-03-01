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
 * @file gpio_hal.c
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
#include <stdbool.h>
#include <stdint.h>
#if HW_CONFIG_GPIO == 1
#include "device_gpio.h"
#include "driver/gpio.h"
#include <assert.h>
#include <esp_attr.h>
#include <esp_log.h>
#include <gpio_hal.h>
#include <hal/gpio_types.h>
#include <stdlib.h> /*lint -e129*/

    /********************************************************************************
     * Defines
     ********************************************************************************/
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

        gpio_hal_interrupt_callback_t callback;          /**< Registered interrupt callback */
        void                         *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
        void                         *callback_context;  /**< User callback context */
    };

    static gpio_hal_t gpio_hal_ports[NUMBER_GPIOS_PORTS] = {0};

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Functions
     ********************************************************************************/
    static void IRAM_ATTR gpio_isr_handler(void *arg)
    {
        // callbacks[(int)arg]->
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)arg;
        gpio_intr_disable(BUTTON_PIN);
        if (p_gpio_hal != NULL && p_gpio_hal->callback != NULL)
        {
            p_gpio_hal->callback(p_gpio_hal->p_callback_handle, p_gpio_hal->callback_context);
        }
        gpio_intr_enable(BUTTON_PIN);
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
            p_free_slot->port_number = port;
            // p_free_slot->p_device_gpio = get_gpio_registers(port);
        }
        return p_free_slot; /* Return new or NULL if no free slot */
    } /*lint !e818*/

    void gpio_hal_remove(p_gpio_hal_t p_handle)
    {
        if (p_handle != NULL && p_handle->in_use_count > 0U)
        {
            p_handle->in_use_count--;
            if (p_handle->in_use_count == 0U)
            {
                // Clean up hardware resources if needed
                // e.g., disable interrupts, reset pin configurations, etc.
                p_handle->config_handle = NULL;
                p_handle->p_device_gpio = NULL;
            }
        }
    }

    void gpio_hal_init(p_gpio_hal_t p_handle)
    {
        (void)p_handle;
    }

    bool gpio_hal_pin_direction(p_gpio_hal_t p_handle, uint8_t pin, pin_direction_t direction)
    {
        (void)p_handle;
        gpio_reset_pin(pin);
        gpio_set_direction(pin, (direction == PIN_DIRECTION_OUTPUT) ? GPIO_MODE_INPUT_OUTPUT : GPIO_MODE_INPUT);
        return true;

    } /*lint !e818*/

    bool gpio_hal_pin_mode(p_gpio_hal_t p_handle, uint8_t pin, pin_mode_t value)
    {
        (void)p_handle;
        switch (value)
        {
        case FLOAT:
            gpio_set_pull_mode(pin, GPIO_FLOATING);
            break;
        case PULLUP:
            gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
            break;
        case PULLDOWN:
            gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
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
        return true;
    }

    bool gpio_hal_pin_speed(p_gpio_hal_t p_handle, uint8_t pin, pin_speed_t value)

    {

        (void)p_handle;
        (void)pin;
        (void)value;
        /* ESP32 GPIO does not have direct speed settings; this is a placeholder */
        return true;
    }

    void gpio_hal_set_state(p_gpio_hal_t p_handle, uint8_t pin, bool value)
    {
        (void)p_handle;

        gpio_set_level(pin, value);

    } /*lint !e818*/

    bool gpio_hal_get_state(p_gpio_hal_t p_handle, uint8_t pin)
    {
        (void)p_handle;
        return (gpio_get_level(pin)) != 0u;

    } /*lint !e818*/

    void gpio_hal_toggle_state(p_gpio_hal_t p_handle, uint8_t pin)
    {
        (void)p_handle;
        bool value = gpio_get_level(pin);
        gpio_set_level(pin, !value);

    } /*lint !e818*/

    void gpio_hal_write_port(p_gpio_hal_t p_handle, uint8_t value)
    {

        (void)p_handle;
        (void)value;
    }

    uint8_t gpio_hal_read_port(p_gpio_hal_t p_handle)
    {

        (void)p_handle;
        return 0;
    }

    bool gpio_hal_register_callback(p_gpio_hal_t p_handle, const void *p_callback_handle, uint8_t pin,
                                    gpio_hal_interrupt_callback_t callback, void *p_callback_context, irq_edge_t edge)
    {

        if (p_handle == NULL || callback == NULL)
        {
            return false;
        }
        p_handle->callback          = callback;
        p_handle->p_callback_handle = (void *)p_callback_handle;
        esp_err_t err               = ESP_OK;
        if (edge == IRQ_POSITIVE)
        {
            err = gpio_set_intr_type(pin, GPIO_INTR_POSEDGE);
        }
        else if (edge == IRQ_NEGATIVE)
        {
            err = gpio_set_intr_type(pin, GPIO_INTR_NEGEDGE);
        }
        else if (edge == IRQ_BOTH)
        {
            err = gpio_set_intr_type(pin, GPIO_INTR_ANYEDGE);
        }
        else if (edge == IRQ_NONE)
        {
            err = gpio_set_intr_type(pin, GPIO_INTR_DISABLE);
        }
        if (err == ESP_OK)
        {
            err = gpio_install_isr_service(0);
            if (err == ESP_OK)
            {
                err = gpio_isr_handler_add(pin, gpio_isr_handler, (void *)p_handle);
            }
        }
        if (err != ESP_OK)
        {
            return false;
        }

        return true;
    }
#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
