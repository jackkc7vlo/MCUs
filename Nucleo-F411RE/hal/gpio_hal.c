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
#include <device_rcc.h>
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
        uint32_t pin_mask; /**< Pin mask for GPIO pins (1 = valid pin, 0 = invalid pin) */
        uint32_t
            moder_mask; /**< Direction mask for GPIO pins (0 = input, 1 = output, 2 = alternate function, 3 = analog) */
        uint32_t pull_up_mask;    /**< Pull-up resistor mask for GPIO pins (1 = enabled, 0 = disabled) */
        uint32_t pull_down_mask;  /**< Pull-down resistor mask for GPIO pins (1 = enabled, 0 =
                                     disabled) */
        uint32_t open_drain_mask; /**< Open-drain configuration mask for GPIO pins (1 = enabled, 0 =
                                     disabled) */

    } gpio_hal_config_handle_t, *p_gpio_hal_config_handle_t;

#if USE_PORTA_GPIO == 1

    static gpio_hal_config_handle_t gpio_hal_config_port_a = {.pin_mask        = 0u,
                                                              .moder_mask      = 0u,
                                                              .pull_up_mask    = 0u,
                                                              .pull_down_mask  = 0x00000000u,
                                                              .open_drain_mask = 0x00000000u};
    static gpio_hal_t               gpio_hal_port_a        = {
                             .config_handle = (void *)&gpio_hal_config_port_a, .p_device_gpio = NULL, .register_callback = NULL

    };
#endif // USE_PORTA_GPIO
#if USE_PORTB_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_b = {.pin_mask        = 0u,
                                                              .moder_mask      = 0u,
                                                              .pull_up_mask    = 0u,
                                                              .pull_down_mask  = 0x00000000u,
                                                              .open_drain_mask = 0x00000000u};
    static gpio_hal_t               gpio_hal_port_b        = {
                             .config_handle = (void *)&gpio_hal_config_port_b, .p_device_gpio = NULL, .register_callback = NULL

    };
#endif // USE_PORTB_GPIO
#if USE_PORTC_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_c = {.pin_mask        = 0u,
                                                              .moder_mask      = 0u,
                                                              .pull_up_mask    = 0u,
                                                              .pull_down_mask  = 0x00000000u,
                                                              .open_drain_mask = 0x00000000u};
    static gpio_hal_t               gpio_hal_port_c        = {
                             .config_handle = (void *)&gpio_hal_config_port_c, .p_device_gpio = NULL, .register_callback = NULL

    };
#endif // USE_PORTC_GPIO
#if USE_PORTD_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_d = {.pin_mask        = 0u,
                                                              .moder_mask      = 0u,
                                                              .pull_up_mask    = 0u,
                                                              .pull_down_mask  = 0x00000000u,
                                                              .open_drain_mask = 0x00000000u};
    static gpio_hal_t               gpio_hal_port_d        = {
                             .config_handle = (void *)&gpio_hal_config_port_d, .p_device_gpio = NULL, .register_callback = NULL

    };
#endif // USE_PORTD_GPIO
#if USE_PORTE_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_e = {.pin_mask        = 0u,
                                                              .moder_mask      = 0x80u,
                                                              .pull_up_mask    = 0u,
                                                              .pull_down_mask  = 0x00000000u,
                                                              .open_drain_mask = 0x00000000u};
    static gpio_hal_t               gpio_hal_port_e        = {
                             .config_handle = (void *)&gpio_hal_config_port_e, .p_device_gpio = NULL, .register_callback = NULL

    };
#endif // USE_PORTE_GPIO
#if USE_PORTF_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_f = {.pin_mask        = 0U,
                                                              .moder_mask      = 0U,
                                                              .pull_up_mask    = 0U,
                                                              .pull_down_mask  = 0x00000000u,
                                                              .open_drain_mask = 0x00000000u};
    static gpio_hal_t               gpio_hal_port_f        = {
                             .config_handle = (void *)&gpio_hal_config_port_f, .p_device_gpio = NULL, .register_callback = NULL

    };
#endif // USE_PORTF_GPIO
#if USE_PORTG_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_g = {.pin_mask        = 0U, // Set up PG1, PG2, PG3
                                                              .direction_mask  = 0U,
                                                              .pull_up_mask    = 0U,
                                                              .pull_down_mask  = 0x00000000u,
                                                              .open_drain_mask = 0x00000000u};
    static gpio_hal_t               gpio_hal_port_g        = {
                             .config_handle = (void *)&gpio_hal_config_port_g, .p_device_gpio = NULL, .register_callback = NULL

    };
#endif // USE_PORTG_GPIO
#if USE_PORTH_GPIO == 1
    static gpio_hal_config_handle_t gpio_hal_config_port_h = {.pin_mask        = 0U,
                                                              .direction_mask  = 0U,
                                                              .pull_up_mask    = 0U,
                                                              .pull_down_mask  = 0x00000000u,
                                                              .open_drain_mask = 0x00000000u};
    static gpio_hal_t               gpio_hal_port_h        = {
                             .config_handle = (void *)&gpio_hal_config_port_h, .p_device_gpio = NULL, .register_callback = NULL

    };
#endif // USE_PORTH_GPIO

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
#if USE_PORTG_GPIO == 1
        case GPIOG_PORT:
            gpio_hal_port_g.p_device_gpio = p_device_gpio_G;
            return (p_gpio_hal_t)&gpio_hal_port_g;
            break;
#endif // USE_PORTG_GPIO
#if USE_PORTF_GPIO == 1
        case GPIOH_PORT:
            gpio_hal_port_h.p_device_gpio = p_device_gpio_h;
            return (p_gpio_hal_t)&gpio_hal_port_h;
            break;
#endif // USE_PORTH_GPIO
        default:
            assert(false); // invalid port
            return NULL;
            break;
        }

    } /*lint !e818*/

    void gpio_hal_init(const void *p_handle)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        if (p_gpio_hal != NULL)
        {

            p_device_rcc->ahb1enr |=
#if USE_PORTA_GPIO == 1
                RCC_AHB1ENR_GPIOAEN_BIT |
#endif // USE_PORTA_GPIO
#if USE_PORTB_GPIO == 1
                RCC_AHB1ENR_GPIOBEN_BIT |
#endif // USE_PORTB_GPIO
#if USE_PORTC_GPIO == 1
                RCC_AHB1ENR_GPIOCEN_BIT |
#endif // USE_PORTC_GPIO
#if USE_PORTD_GPIO == 1
                RCC_AHB1ENR_GPIODEN_BIT |
#endif // USE_PORTD_GPIO
#if USE_PORTE_GPIO == 1
                RCC_AHB1ENR_GPIOEEN_BIT |
#endif // USE_PORTE_GPIO
#if USE_PORTF_GPIO == 1
                RCC_AHB1ENR_GPIOFEN_BIT |
#endif // USE_PORTF_GPIO
#if USE_PORTG_GPIO == 1
                RCC_AHB1ENR_GPIOGEN_BIT |
#endif // USE_PORTG_GPIO
#if USE_PORTH_GPIO == 1
                RCC_AHB1ENR_GPIOHEN_BIT |
#endif // USE_PORTH_GPIO
                0u;
        }
    }

    bool gpio_hal_pin_direction(const void *p_handle, uint8_t pin, pin_direction_t direction)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;

        if (p_gpio != NULL)
        {
            p_gpio->moder &= ~(3U << (2U * pin)); // Clear moder bits
            switch (direction)
            {
            case INPUT:
                break; // bits already cleared
            case OUTPUT:
                p_gpio->moder |= (1U << (2U * pin)); // Set least significant bit
                break;
            case ALT:
                p_gpio->moder |= (2U << (2U * pin)); // Set most significant bit
                break;
            case ANALOG:
                p_gpio->moder |= (3U << (2U * pin)); // Set most significant bit
                break;

            default:
                return false;
                break;
            }
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

    bool gpio_hal_pin_mode(const void *p_handle, uint8_t pin, pin_mode_t value)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;

        if (p_gpio != NULL)
        {

            switch (value)
            {
            case FLOAT:
                p_gpio->pupdr &= ~(3U << (2U * pin)); // Clear pull-up/pull-down bits
                break;
            case PULLUP:
                p_gpio->pupdr &= ~(3U << (2U * pin)); // Clear pull-up/pull-down bits
                p_gpio->pupdr |= (1U << (2U * pin));  // Set least significant bit
                break;
            case PULLDOWN:
                p_gpio->pupdr &= ~(3U << (2U * pin)); // Clear pull-up/pull-down bits
                p_gpio->pupdr |= (2U << (2U * pin));  // Set most significant bit
                break;
            case OPENDRAIN:
                p_gpio->otype |= (1U << pin);
                break;
            case PUSHPULL:
                p_gpio->otype &= ~(1U << pin);
                break;

            default:
                return false;
                break;
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
    bool gpio_hal_pin_speed(const void *p_handle, uint8_t pin, pin_speed_t value)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;

        if (p_gpio != NULL)
        {

            p_gpio->ospeedr &= ~(3U << (2U * pin)); // Clear ospeedr bits
            switch (value)
            {
            case LOWSPEED:
                break; // bits already cleared
            case MEDIUMSPEED:
                p_gpio->ospeedr |= (1U << (2U * pin)); // Set least significant bit
                break;
            case FASTSPEED:
                p_gpio->ospeedr |= (2U << (2U * pin)); // Set most significant bit
                break;
            case HIGHSPEED:
                p_gpio->ospeedr |= (3U << (2U * pin)); // Set both bits
                break;

            default:
                return false;
                break;
            }
        }
        return true;
    }

    void gpio_hal_set_state(const void *p_handle, uint8_t pin, bool value)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        if (value)
        {
            p_gpio->odr |= (1U << pin);
        }
        else
        {
            p_gpio->odr &= ~(1U << pin);
        }
    }

    bool gpio_hal_get_state(const void *p_handle, uint8_t pin)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        uint32_t     mode       = p_gpio->moder & (3U << (2U * pin)) >> (3U << (2U * pin));
        switch (mode)
        {
        case INPUT:
            return (p_gpio->idr & (1U << pin)) != 0u;
            break;
        case OUTPUT:
            return (p_gpio->odr & (1U << pin)) != 0u;
            break;
        }

        return false;

    } /*lint !e818*/

    void gpio_hal_toggle_state(const void *p_handle, uint8_t pin)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        p_gpio->odr ^= (1U << pin);

    } /*lint !e818*/

    void gpio_hal_write_port(const void *p_handle, uint8_t value)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        p_gpio->odr             = (uint32_t)value;
    }

    uint8_t gpio_hal_read_port(const void *p_handle)
    {
        p_gpio_hal_t p_gpio_hal = (p_gpio_hal_t)p_handle;
        reg_gpio_t  *p_gpio     = (reg_gpio_t *)p_gpio_hal->p_device_gpio;
        return (p_gpio->odr & 0xFFu);
    }

#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */