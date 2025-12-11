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

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    //<! Pointer to an incomplete type (hides implementation)
    typedef struct gpio_hal_config_handle
    {
        uint32_t pin_mask;        /**< Pin mask for GPIO pins (1 = valid pin, 0 = invalid pin) */
        uint32_t direction_mask;  /**< Direction mask for GPIO pins (1 = output, 0 = input) */
        uint32_t pull_up_mask;    /**< Pull-up resistor mask for GPIO pins (1 = enabled, 0 = disabled) */
        uint32_t pull_down_mask;  /**< Pull-down resistor mask for GPIO pins (1 = enabled, 0 =
                                     disabled) */
        uint32_t open_drain_mask; /**< Open-drain configuration mask for GPIO pins (1 = enabled, 0 =
                                     disabled) */

    } gpio_hal_config_handle_t, *p_gpio_hal_config_handle_t;

    // forward declarations
    void    gpio_hal_set_state(const void *p_handle, uint8_t pin, bool value);
    bool    gpio_hal_get_state(const void *p_handle, uint8_t pin);
    void    gpio_hal_toggle_state(const void *p_handle, uint8_t pin);
    void    gpio_hal_write_port(const void *p_handle, uint8_t value);
    uint8_t gpio_hal_read_port(const void *p_handle);
    void    gpio_hal_init(const void *p_handle);

#if USE_PORTA_GPIO == 1

    static gpio_hal_config_handle_t gpio_hal_config_port_a = {
        .pin_mask = 0u,        // | GPIO_PIN6_BIT | GPIO_PIN5_BIT | GPIO_PIN4_BIT | GPIO_PIN3_BIT |
                               // GPIO_PIN2_BIT | GPIO_PIN1_BIT | GPIO_PIN0_BIT,
        .direction_mask  = 0u, // Set PIO0_7 as output
        .pull_up_mask    = 0u,
        .pull_down_mask  = 0x00000000u,
        .open_drain_mask = 0x00000000u};
    static gpio_hal_t gpio_hal_port_a = {.config_handle     = (void *)&gpio_hal_config_port_a,
                                         .p_device_gpio     = NULL,
                                         .init              = &gpio_hal_init,
                                         .set               = &gpio_hal_set_state,
                                         .get               = &gpio_hal_get_state,
                                         .toggle            = &gpio_hal_toggle_state,
                                         .register_callback = NULL

    };
#endif // USE_PORTA_GPIO
#if USE_PORTB_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_b = {
        .pin_mask = 0u,        // | GPIO_PIN6_BIT | GPIO_PIN5_BIT | GPIO_PIN4_BIT | GPIO_PIN3_BIT |
                               // GPIO_PIN2_BIT | GPIO_PIN1_BIT | GPIO_PIN0_BIT,
        .direction_mask  = 0u, // Set PIO0_7 as output
        .pull_up_mask    = 0u,
        .pull_down_mask  = 0x00000000u,
        .open_drain_mask = 0x00000000u};
    static gpio_hal_t gpio_hal_port_b = {.config_handle     = (void *)&gpio_hal_config_port_b,
                                         .p_device_gpio     = NULL,
                                         .init              = &gpio_hal_init,
                                         .set               = &gpio_hal_set_state,
                                         .get               = &gpio_hal_get_state,
                                         .toggle            = &gpio_hal_toggle_state,
                                         .register_callback = NULL

    };
#endif // USE_PORTB_GPIO
#if USE_PORTC_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_c = {
        .pin_mask = 0u,        // | GPIO_PIN6_BIT | GPIO_PIN5_BIT | GPIO_PIN4_BIT | GPIO_PIN3_BIT |
                               // GPIO_PIN2_BIT | GPIO_PIN1_BIT | GPIO_PIN0_BIT,
        .direction_mask  = 0u, // Set PIO0_7 as output
        .pull_up_mask    = 0u,
        .pull_down_mask  = 0x00000000u,
        .open_drain_mask = 0x00000000u};
    static gpio_hal_t gpio_hal_port_c = {.config_handle     = (void *)&gpio_hal_config_port_c,
                                         .p_device_gpio     = NULL,
                                         .init              = &gpio_hal_init,
                                         .set               = &gpio_hal_set_state,
                                         .get               = &gpio_hal_get_state,
                                         .toggle            = &gpio_hal_toggle_state,
                                         .register_callback = NULL

    };
#endif // USE_PORTC_GPIO
#if USE_PORTD_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_d = {
        .pin_mask = 0u,        // | GPIO_PIN6_BIT | GPIO_PIN5_BIT | GPIO_PIN4_BIT | GPIO_PIN3_BIT |
                               // GPIO_PIN2_BIT | GPIO_PIN1_BIT | GPIO_PIN0_BIT,
        .direction_mask  = 0u, // Set PIO0_7 as output
        .pull_up_mask    = 0u,
        .pull_down_mask  = 0x00000000u,
        .open_drain_mask = 0x00000000u};
    static gpio_hal_t gpio_hal_port_d = {.config_handle     = (void *)&gpio_hal_config_port_d,
                                         .p_device_gpio     = NULL,
                                         .init              = &gpio_hal_init,
                                         .set               = &gpio_hal_set_state,
                                         .get               = &gpio_hal_get_state,
                                         .toggle            = &gpio_hal_toggle_state,
                                         .register_callback = NULL

    };
#endif // USE_PORTD_GPIO
#if USE_PORTE_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_e = {
        .pin_mask = 0u,        // | GPIO_PIN6_BIT | GPIO_PIN5_BIT | GPIO_PIN4_BIT | GPIO_PIN3_BIT |
                               // GPIO_PIN2_BIT | GPIO_PIN1_BIT | GPIO_PIN0_BIT,
        .direction_mask  = 0u, // Set PIO0_7 as output
        .pull_up_mask    = 0u,
        .pull_down_mask  = 0x00000000u,
        .open_drain_mask = 0x00000000u};
    static gpio_hal_t gpio_hal_port_e = {.config_handle     = (void *)&gpio_hal_config_port_e,
                                         .p_device_gpio     = NULL,
                                         .init              = &gpio_hal_init,
                                         .set               = &gpio_hal_set_state,
                                         .get               = &gpio_hal_get_state,
                                         .toggle            = &gpio_hal_toggle_state,
                                         .register_callback = NULL

    };
#endif // USE_PORTE_GPIO
#if USE_PORTF_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_f = {
        .pin_mask        = (GPIO_PIN1_BIT | GPIO_PIN2_BIT | GPIO_PIN3_BIT), // Set up PF1, PF2, PF3
        .direction_mask  = (GPIO_PIN1_BIT | GPIO_PIN2_BIT | GPIO_PIN3_BIT),
        .pull_up_mask    = (GPIO_PIN1_BIT | GPIO_PIN2_BIT | GPIO_PIN3_BIT),
        .pull_down_mask  = 0x00000000u,
        .open_drain_mask = 0x00000000u};
    static gpio_hal_t gpio_hal_port_f = {.config_handle     = (void *)&gpio_hal_config_port_f,
                                         .p_device_gpio     = NULL,
                                         .init              = &gpio_hal_init,
                                         .set               = &gpio_hal_set_state,
                                         .get               = &gpio_hal_get_state,
                                         .write             = &gpio_hal_write_port,
                                         .read              = &gpio_hal_read_port,
                                         .toggle            = &gpio_hal_toggle_state,
                                         .register_callback = NULL

    };
#endif // USE_PORTF_GPIO

    /********************************************************************************
     * Functions
     ********************************************************************************/
    p_gpio_hal_t gpio_hal_create(uint32_t port) // setup GPIO HAL instance
    {
        switch (port)
        {
#if USE_PORTA_GPIO == 1
        case GPIOA_PORT:
            gpio_hal_port_a.p_device_gpio = p_device_gpio_a;
            return (p_gpio_hal_t)&gpio_hal_port_a;
            break;
#endif // USE_PORTA_GPIO
#if USE_PORTB_GPIO == 1
        case GPIOB_PORT:
            gpio_hal_port_b.p_device_gpio = p_device_gpio_b;
            return (p_gpio_hal_t)&gpio_hal_port_b;
            break;
#endif // USE_PORTB_GPIO
#if USE_PORTC_GPIO == 1
        case GPIOC_PORT:
            gpio_hal_port_c.p_device_gpio = p_device_gpio_c;
            return (p_gpio_hal_t)&gpio_hal_port_c;
            break;
#endif // USE_PORTC_GPIO
#if USE_PORTD_GPIO == 1
        case GPIOD_PORT:
            gpio_hal_port_d.p_device_gpio = p_device_gpio_d;
            return (p_gpio_hal_t)&gpio_hal_port_d;
            break;
#endif // USE_PORTD_GPIO
#if USE_PORTE_GPIO == 1
        case GPIOE_PORT:
            gpio_hal_port_e.p_device_gpio = p_device_gpio_e;
            return (p_gpio_hal_t)&gpio_hal_port_e;
            break;
#endif // USE_PORTE_GPIO
#if USE_PORTF_GPIO == 1
        case GPIOF_PORT:
            gpio_hal_port_f.p_device_gpio = p_device_gpio_f;
            return (p_gpio_hal_t)&gpio_hal_port_f;
            break;
#endif // USE_PORTF_GPIO
        default:
            return NULL;
            break;
        }

    } /*lint !e818*/

    void gpio_hal_init(const void *p_handle)
    {
        // assert(p_handle != NULL);
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        if (p_gpio_hal != NULL)
        {
            p_device_sysctl->rcgcgpio |=
#if USE_PORTA_GPIO == 1
                SYSCON_RCGCGPIO_PORTA_BIT |
#endif // USE_PORTA_GPIO
#if USE_PORTB_GPIO == 1
                SYSCON_RCGCGPIO_PORTB_BIT |
#endif // USE_PORTB_GPIO
#if USE_PORTC_GPIO == 1
                SYSCON_RCGCGPIO_PORTC_BIT |
#endif // USE_PORTC_GPIO
#if USE_PORTD_GPIO == 1
                SYSCON_RCGCGPIO_PORTD_BIT |
#endif // USE_PORTD_GPIO
#if USE_PORTE_GPIO == 1
                SYSCON_RCGCGPIO_PORTE_BIT |
#endif // USE_PORTE_GPIO
#if USE_PORTF_GPIO == 1
                SYSCON_RCGCGPIO_PORTF_BIT |
#endif // USE_PORTF_GPIO
                0u;
        }
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
    }

    void gpio_hal_set_state(const void *p_handle, uint8_t pin, bool value)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        if (value)
        {
            p_gpio->data |= pin;
        }
        else
        {
            p_gpio->data &= ~pin;
        }
    }

    bool gpio_hal_get_state(const void *p_handle, uint8_t pin)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        return (p_gpio->data) != 0u;

    } /*lint !e818*/

    void gpio_hal_toggle_state(const void *p_handle, uint8_t pin)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        p_gpio->data ^= (pin);

    } /*lint !e818*/

    void gpio_hal_write_port(const void *p_handle, uint8_t value)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        p_gpio->data            = (uint32_t)value;
    }

    uint8_t gpio_hal_read_port(const void *p_handle)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        return (p_gpio->data & 0xFFu);
    }

#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */