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
#if HW_CONFIG_GPIO == 1
#include "device_gpio.h"
#include "driver/gpio.h"
#include <assert.h>
#include <esp_attr.h>
#include <esp_log.h>
#include <gpio_hal.h>
#include <hal/gpio_types.h>
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /*lint -e129*/

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    //<! Pointer to an incomplete type (hides implementation)
    typedef struct gpio_hal_config_pins
    {
        uint32_t pin_mask;        /**< Pin mask for GPIO pins (1 = valid pin, 0 =
                                     invalid pin) */
        uint32_t direction_mask;  /**< Direction mask for GPIO pins (1 = output,
                                     0 = input) */
        uint32_t pull_up_mask;    /**< Pull-up resistor mask for GPIO pins (1 =
                                     enabled,   0 = disabled) */
        uint32_t pull_down_mask;  /**< Pull-down resistor mask for GPIO pins (1 =
                                     enabled, 0 = disabled) */
        uint32_t open_drain_mask; /**< Open-drain configuration mask for GPIO
                                     pins (1 = enabled, 0 = disabled) */

    } gpio_hal_config_bank_t, *p_gpio_hal_config_bank_t;

    typedef struct gpio_hal_config_handle
    {
        gpio_hal_config_bank_t bank[2]; /**< Pin mask for GPIO pins (1 = valid pin, 0 =
                              invalid pin) */

    } gpio_hal_config_handle_t, *p_gpio_hal_config_handle_t;

    static gpio_hal_config_handle_t gpio_hal_config_port_a = {

        .bank[0] =
            {.pin_mask = 0u, .direction_mask = 0u, .pull_up_mask = 0U, .pull_down_mask = 0U, .open_drain_mask = 0U},
        .bank[1] = {
            .pin_mask = 0U, .direction_mask = 0U, .pull_up_mask = 0u, .pull_down_mask = 0U, .open_drain_mask = 0U}};

    // forward declarations
    void    gpio_hal_set_state(const void *p_handle, uint8_t pin, bool value);
    bool    gpio_hal_get_state(const void *p_handle, uint8_t pin);
    void    gpio_hal_toggle_state(const void *p_handle, uint8_t pin);
    void    gpio_hal_write_port(const void *p_handle, uint8_t value);
    uint8_t gpio_hal_read_port(const void *p_handle);

    void gpio_hal_init(const void *p_handle);

    static gpio_hal_t gpio_hal_port_a = {.config_handle    = (void *)&gpio_hal_config_port_a,
                                         .p_device_gpio    = NULL,
                                         .callback         = NULL,
                                         .callback_context = NULL};

    /********************************************************************************
     * Functions
     ********************************************************************************/
    static void IRAM_ATTR gpio_isr_handler(void *arg)
    {
        // callbacks[(int)arg]->
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)arg;
        if (p_gpio_hal->callback != NULL)
        {
            p_gpio_hal->callback(p_gpio_hal->p_callback_handle, p_gpio_hal->callback_context);
        }
    }

    p_gpio_hal_t gpio_hal_create(uint32_t port)
    {
        (void)port;
        return (p_gpio_hal_t)&gpio_hal_port_a;
    } /*lint !e818*/

    void gpio_hal_init(const void *p_handle)
    {
        (void)p_handle;
    }

    bool gpio_hal_pin_direction(const void *p_handle, uint8_t pin, pin_direction_t direction)
    {
        (void)p_handle;
        gpio_set_direction(pin, (direction == OUTPUT) ? GPIO_MODE_INPUT_OUTPUT : GPIO_MODE_INPUT);
        return true;

    } /*lint !e818*/

    bool gpio_hal_pin_mode(const void *p_handle, uint8_t pin, pin_mode_t value)
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

    bool gpio_hal_pin_speed(const void *p_handle, uint8_t pin, pin_speed_t value)
    {

        (void)p_handle;
        (void)pin;
        (void)value;
        // ESP32 GPIO does not have direct speed settings; this is a placeholder
        return true;
    }

    void gpio_hal_set_state(const void *p_handle, uint8_t pin, bool value)
    {
        (void)p_handle;

        gpio_set_level(pin, value);

    } /*lint !e818*/

    bool gpio_hal_get_state(const void *p_handle, uint8_t pin)
    {
        (void)p_handle;
        return (gpio_get_level(pin)) != 0u;

    } /*lint !e818*/

    void gpio_hal_toggle_state(const void *p_handle, uint8_t pin)
    {
        (void)p_handle;
        bool value = gpio_get_level(pin);
        gpio_set_level(pin, !value);

    } /*lint !e818*/

    void gpio_hal_write_port(const void *p_handle, uint8_t value)
    {
        // p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        (void)p_handle;
        (void)value;
    }

    uint8_t gpio_hal_read_port(const void *p_handle)
    {
        // p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        return 0;
    }

    bool gpio_hal_register_callback(const void *p_handle, const void *p_callback_handle, uint8_t pin,
                                    gpio_hal_interrupt_callback_t callback, void *p_callback_context, irq_edge_t edge)
    {

        if (p_handle == NULL || callback == NULL)
        {
            return false;
        }
        gpio_hal_t *p_gpio_hal        = (gpio_hal_t *)p_handle;
        p_gpio_hal->callback          = callback;
        p_gpio_hal->p_callback_handle = (void *)p_callback_handle;
        esp_err_t err                 = ESP_OK;
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
        ESP_LOGI("TAG", "err is %s", err == ESP_OK ? "ESP_OK" : "ESP_FAIL");
        err = gpio_install_isr_service(0);
        ESP_LOGI("TAG", "err is %s", err == ESP_OK ? "ESP_OK" : "ESP_FAIL");
        err = gpio_isr_handler_add(pin, gpio_isr_handler, (void *)p_gpio_hal);
        ESP_LOGI("TAG", "err is %s", err == ESP_OK ? "ESP_OK" : "ESP_FAIL");

        return true;
    }
#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
