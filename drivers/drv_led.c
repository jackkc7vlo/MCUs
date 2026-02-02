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
 ************************(C) COPYRIGHT 2026 Jack Wilson **********************/
/**
 * @file drv_led.c
 * @brief Driver for a simple GPIO led (implementation)
 * @details Implements the driver functions for a simple GPIO led using the
 * GPIO HAL abstraction.
 *
 * @author  Jack Wilson
 * @date    February 02, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/

/********************************************************************************
 * Includes
 ********************************************************************************/

/*lint -esym(793,__*)*/
#include <hw_config.h>
#if HW_CONFIG_GPIO == 1 && USE_LED_GPIO == 1
#include "drv_led.h"
#include <gpio_hal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs & Enums
 ********************************************************************************/
/* Pointer to an incomplete type (hides implementation) */
typedef struct led_handle
{
    bool          in_use;     /**< Is this led instance in use */
    p_gpio_hal_t  p_gpio_hal; /**< Pointer to the GPIO HAL instance */
    uint_fast32_t pin;        /**< The GPIO pin number */
    uint_fast32_t port;       /**< The GPIO port */

} led_handle_t;

static led_handle_t leds[NUMBER_OF_LEDS] = {0}; // initialize all to zero / not in use

#if (NUMBER_OF_BUTTONS > 2U)
#error "NUMBER_OF_BUTTONS greater than 2 not supported"
#endif /* NUMBER_OF_BUTTONS */
/********************************************************************************
 * Functions
 ********************************************************************************/

void *drv_led_create(uint32_t port, uint8_t pin)
{
    led_handle_t *p_led = NULL;
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
        if (leds[i].in_use == false)
        {
            leds[i].in_use = true;
            p_led          = &leds[i];
            break;
        }
    }
    if (p_led == NULL)
    {
        return NULL; // No available led instances
    }

    p_led->port = port;
    p_led->pin  = pin;

    return (void *)p_led;
}

bool drv_led_init(const void *p_handle)
{
    led_handle_t *p_led = (led_handle_t *)p_handle;
    if (p_led == NULL)
    {
        return false;
    }
    p_gpio_hal_t gpio_handle = gpio_hal_create(p_led->port);
    if (gpio_handle == NULL)
    {
        return false;
    }
    p_led->p_gpio_hal = gpio_handle;
    gpio_hal_init(gpio_handle);
    gpio_hal_pin_direction(gpio_handle, p_led->pin, PIN_DIRECTION_OUTPUT);
#ifndef LED_MODE
#pragma message "LED_MODE not defined in hw_config.h"
#endif
    gpio_hal_pin_mode(gpio_handle, p_led->pin, LED_MODE);

    return true;
}

bool drv_led_on_off(void *p_handle, bool on)
{
    led_handle_t *p_led = (led_handle_t *)p_handle;
    if (p_led == NULL || p_led->p_gpio_hal == NULL)
    {
        return false;
    }
    gpio_hal_set_state(p_led->p_gpio_hal, p_led->pin, on);
    return true;
}

bool drv_led_toggle(void *p_handle)
{
    led_handle_t *p_led = (led_handle_t *)p_handle;
    if (p_led == NULL || p_led->p_gpio_hal == NULL)
    {
        return false;
    }
    gpio_hal_toggle_state(p_led->p_gpio_hal, p_led->pin);
    return true;
}

#endif /* HW_CONFIG_GPIO */