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

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    //<! Pointer to an incomplete type (hides implementation)
    typedef struct gpio_hal_config_handle
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

    } gpio_hal_config_handle_t, *p_gpio_hal_config_handle_t;

    static gpio_hal_config_handle_t gpio_hal_config_port_a = {
        .pin_mask = GPIO_PIN7_BIT,        // | GPIO_PIN6_BIT | GPIO_PIN5_BIT |
                                          // GPIO_PIN4_BIT | GPIO_PIN3_BIT | GPIO_PIN2_BIT
                                          // | GPIO_PIN1_BIT | GPIO_PIN0_BIT,
        .direction_mask  = GPIO_PIN7_BIT, // Set PIO0_7 as output
        .pull_up_mask    = GPIO_PIN7_BIT,
        .pull_down_mask  = 0x00000000u,
        .open_drain_mask = 0x00000000u};

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
        // On the NHS3100 there is only one GPIO port, so we ignore port_count
        gpio_hal_port_a.p_device_gpio = p_device_gpio_a;

        return (p_gpio_hal_t)&gpio_hal_port_a;
    } /*lint !e818*/

    void gpio_hal_init(const void *p_handle)
    {
        // assert(p_handle != NULL);
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        if (p_gpio_hal != NULL)
        {
            p_syscon->ahbclkctrl |= (SYSCON_AHBCLKCTRL_GPIO_BIT | SYSCON_AHBCLKCTRL_IOCON_BIT) &
                                    SYSCON_AHBCLKCTRL_BITS_THAT_CAN_BE_SET; // Enable GPIO and IOCON
                                                                            // clock
        }
        if (p_gpio_hal->config_handle != NULL)
        {
            p_gpio_hal_config_handle_t p_config =
                (p_gpio_hal_config_handle_t)p_gpio_hal->config_handle;

            // Configure pin directions
            p_gpio_hal->p_device_gpio->dir &= ~p_config->pin_mask; // Clear bits to set as input
            p_gpio_hal->p_device_gpio->dir |=
                (p_config->direction_mask & p_config->pin_mask); // Set bits to set as output

            // Configure pull-up/pull-down resistors and open-drain settings as
            // needed (Assuming IOCON registers are used for this configuration)
            for (uint8_t pin = 0; pin <= 11; pin++)
            {
                uint32_t pin_bit = (1U << pin);
                if (p_config->pin_mask & pin_bit)
                {
                    uint32_t mode = 0;
                    if (p_config->pull_up_mask & pin_bit)
                    {
                        mode |= IOCON_MODE_PULLUP;
                    }
                    else if (p_config->pull_down_mask & pin_bit)
                    {
                        mode |= IOCON_MODE_PULLDOWN;
                    }
                    //                    if (p_config->open_drain_mask &
                    //                    pin_bit)
                    //                   {
                    //                       mode |= IOCON_OPEN_DRAIN_EN;
                    //                   }
                    // Assuming all pins are on port 0 for simplicity
                    volatile uint32_t *p_iocon_reg = &p_iocon->pio0_0 + pin;
                    *p_iocon_reg                   = IOCON_FUNC_GPIO | mode;
                }
            }
        }
    }

    void gpio_hal_set_state(const void *p_handle, uint8_t pin, bool value)
    {
        p_gpio_hal_t p_gpio_hal                   = (p_gpio_hal_t)p_handle;
        p_gpio_hal->p_device_gpio->data[1 << pin] = (uint32_t)value ? 1U << pin : 0U;
        // NSS_GPIO[p_gpio->port].DATA[1u << p_gpio->pin] = (uint32_t)state <<
        // p_gpio->pin;

    } /*lint !e818*/

    bool gpio_hal_get_state(const void *p_handle, uint8_t pin)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        return (p_gpio_hal->p_device_gpio->data[1 << pin]) != 0u;
        // return (NSS_GPIO[p_gpio->port].DATA[1u << p_gpio->pin]) != 0u;
    } /*lint !e818*/

    void gpio_hal_toggle_state(const void *p_handle, uint8_t pin)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        p_gpio_hal->p_device_gpio->data[1 << pin] ^= (1u << pin);
        // NSS_GPIO[p_gpio->port].DATA[1u << p_gpio->pin] ^= (1u <<
        // p_gpio->pin);
    } /*lint !e818*/

    void gpio_hal_write_port(const void *p_handle, uint8_t value)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        p_gpio_hal->p_device_gpio->data[0xfff] = (uint32_t)value;
        // p_gpio_hal->p_device_gpio->data = (uint32_t)value;
    }

    uint8_t gpio_hal_read_port(const void *p_handle)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        return (p_gpio_hal->p_device_gpio->data[0xfff]) & 0xFF;
       
    }
#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
