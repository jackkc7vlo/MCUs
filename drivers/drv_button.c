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
#if HW_CONFIG_GPIO == 1 && USE_BUTTON_GPIO == 1
#include "drv_button.h"
#include <device_gpio.h>
// #include <device_rcc.h>
#include <gpio_hal.h>
#include <stdbool.h>
#include <stddef.h>
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
    typedef struct button_handle
    {
        bool                in_use;     /**< Is this button instance in use */
        p_gpio_hal_t        p_gpio_hal; /**< Pointer to the GPIO HAL instance */
        uint_fast32_t       pin;        /**< The GPIO pin number */
        uint_fast32_t       port;       /**< The GPIO port */
        p_button_callback_t p_callback; /**< Pointer to the callback function */

    } button_handle_t;

    button_handle_t buttons[NUMBER_OF_BUTTONS] = {0}; // initialize all to zero / not in use

#if (NUMBER_OF_BUTTONS > 2U)
#message("Error: NUMBER_OF_BUTTONS greater than 2 not supported")
#endif // NUMBER_OF_BUTTONS
    /********************************************************************************
     * Functions
     ********************************************************************************/
    void button_isr_callback(void *p_handle, void *p_callback_context)
    {
        button_handle_t *p_button = (button_handle_t *)p_handle;
        if (p_button != NULL && p_button->p_callback != NULL)
        {
            p_button->p_callback(drv_button_is_pressed(p_button) ? BUTTON_PRESSED : BUTTON_RELEASED);
        }
    }

    void *drv_button_create(uint32_t port, uint8_t pin, p_button_callback_t p_callback)
    {
        button_handle_t *p_button = NULL;

        for (uint8_t i = 0; i < NUMBER_OF_BUTTONS; i++)
        {
            if (buttons[i].in_use == false)
            {
                buttons[i].in_use = true;
                p_button          = &buttons[i];
                break;
            }
        }
        if (p_button == NULL)
        {
            return NULL; // No available button instances
        }

        p_button->port       = port;
        p_button->pin        = pin;
        p_button->p_callback = p_callback;

        return (void *)p_button;
    }

    bool drv_button_init(const void *p_handle)
    {
        button_handle_t *p_button = (button_handle_t *)p_handle;
        if (p_button == NULL)
        {
            return false;
        }
        p_gpio_hal_t gpio_handle = gpio_hal_create(p_button->port);
        if (gpio_handle == NULL)
        {
            return false;
        }
        p_button->p_gpio_hal = gpio_handle;
        gpio_hal_init(gpio_handle);
        gpio_hal_pin_direction(gpio_handle, p_button->pin, INPUT);
        if (p_button->p_callback != NULL)
        {
            gpio_hal_register_callback(p_button->p_gpio_hal, p_button, p_button->pin, button_isr_callback, p_button,
                                       IRQ_BOTH);
        }
        return true;
    }

    bool drv_button_is_pressed(void *p_handle)
    {
        button_handle_t *p_button = (button_handle_t *)p_handle;
        if (p_button == NULL || p_button->p_gpio_hal == NULL)
        {
            return false;
        }
        return gpio_hal_get_state(p_button->p_gpio_hal, p_button->pin) ^
               (BUTTON_ACTIVE_HIGH ? true : false); // Adjust for active high/low
    }

#endif // HW_CONFIG_GPIO

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */