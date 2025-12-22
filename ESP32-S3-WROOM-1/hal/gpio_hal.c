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
#if AUTO_CS == 1u
        .bank[0] = {.pin_mask       = (GPIO_PIN3_BIT | GPIO_PIN7_BIT | GPIO_PIN9_BIT | GPIO_PIN18_BIT),
                    .direction_mask = (GPIO_PIN3_BIT | GPIO_PIN7_BIT | GPIO_PIN9_BIT | GPIO_PIN18_BIT),
                    //.pull_up_mask    = (GPIO_PIN3_BIT | GPIO_PIN7_BIT | GPIO_PIN9_BIT | GPIO_PIN18_BIT),
                    .pull_up_mask    = 0U,
                    .pull_down_mask  = 0U,
                    .open_drain_mask = 0U},
#else
        .bank[0] = {.pin_mask        = (GPIO_PIN3_BIT | GPIO_PIN7_BIT | GPIO_PIN15_BIT | GPIO_PIN18_BIT),
                    .direction_mask  = (GPIO_PIN3_BIT | GPIO_PIN7_BIT | GPIO_PIN15_BIT | GPIO_PIN18_BIT),
                    .pull_up_mask    = (GPIO_PIN3_BIT | GPIO_PIN7_BIT | GPIO_PIN15_BIT | GPIO_PIN18_BIT),
                    .pull_down_mask  = 0U,
                    .open_drain_mask = 0U},
#endif
        .bank[1] = {.pin_mask        = 0U, // GPIO_PIN48_BIT,
                    .direction_mask  = 0U, // GPIO_PIN48_BIT,
                    .pull_up_mask    = 0u, // GPIO_PIN48_BIT,
                    .pull_down_mask  = 0U,
                    .open_drain_mask = 0U}};

    // forward declarations
    void    gpio_hal_set_state(const void *p_handle, uint8_t pin, bool value);
    bool    gpio_hal_get_state(const void *p_handle, uint8_t pin);
    void    gpio_hal_toggle_state(const void *p_handle, uint8_t pin);
    void    gpio_hal_write_port(const void *p_handle, uint8_t value);
    uint8_t gpio_hal_read_port(const void *p_handle);

    void gpio_hal_init(const void *p_handle);

    static gpio_hal_t gpio_hal_port_a = {.config_handle     = (void *)&gpio_hal_config_port_a,
                                         .p_device_gpio     = NULL,
                                         .init              = &gpio_hal_init,
                                         .set               = &gpio_hal_set_state,
                                         .get               = &gpio_hal_get_state,
                                         .toggle            = &gpio_hal_toggle_state,
                                         .write             = &gpio_hal_write_port,
                                         .read              = &gpio_hal_read_port,
                                         .register_callback = NULL

    };

    /********************************************************************************
     * Functions
     ********************************************************************************/
    p_gpio_hal_t gpio_hal_create(uint32_t port)
    {
        (void)port;
        return (p_gpio_hal_t)&gpio_hal_port_a;
    } /*lint !e818*/

    void gpio_hal_init(const void *p_handle)
    {
        // assert(p_handle != NULL);
        p_gpio_hal_t               p_gpio_hal = (p_gpio_hal_t)p_handle;
        p_gpio_hal_config_handle_t p_config   = (p_gpio_hal_config_handle_t)p_gpio_hal->config_handle;

        for (uint32_t bank = 0; bank < 2u; bank++)
        {
            gpio_hal_config_bank_t *p_bank_config = &p_config->bank[bank];

            for (uint32_t i = 0; i < 32u; i++)
            {
                if ((p_bank_config->pin_mask & (1u << i)) != 0u)
                {
                    uint8_t pin_number = (uint8_t)(i + (bank * 32u));
                    gpio_reset_pin(pin_number);
                    gpio_set_direction(pin_number, (p_bank_config->direction_mask & (1u << i)) != 0u ? GPIO_MODE_OUTPUT
                                                                                                     : GPIO_MODE_INPUT);
                    if ((p_bank_config->pull_up_mask & (1u << i)) != 0u)
                    {
                        gpio_set_pull_mode(pin_number, GPIO_PULLUP_ONLY);
                    }
                    else if ((p_bank_config->pull_down_mask & (1u << i)) != 0u)
                    {
                        gpio_set_pull_mode(pin_number, GPIO_PULLDOWN_ONLY);
                    }
                    else
                    {
                        gpio_set_pull_mode(pin_number, GPIO_FLOATING);
                    }
                    if ((p_bank_config->open_drain_mask & (1u << i)) != 0u)
                    {
                        // Open-drain not directly supported in ESP32 GPIO API
                        // Custom handling may be required here
                    }
                }
            }
        }
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
#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
