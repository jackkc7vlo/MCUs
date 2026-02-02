/******************************************************************************
 * Copyright (C) 2026 by Jack Wilson
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
 * @file drv_led.h
 * @brief Driver for a simple GPIO led
 * @details Outlines the driver functions for a simple GPIO led
 *
 * @author  Jack Wilson
 * @date    February 08, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef drv_led_h_
#define drv_led_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>

#if USE_LED_GPIO == 1u
    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    typedef void *p_led_handle_t;
    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /**
     * @brief Create a GPIO led instance
     * @param port The GPIO port
     * @param pin The GPIO pin
     * @return Pointer to the button instance or NULL if error
     */
    void *drv_led_create(uint32_t port, uint8_t pin);
    /**
     * @brief Initialize the GPIO button
     * @param p_handle Pointer to the button instance
     * @return true if successful, false if not
     */
    bool drv_led_init(const void *p_handle);
    /**
     * @brief Turn the LED on
     * @param p_handle Pointer to the led instance
     * @param on true to turn on, false to turn off
     * @return true if successful, false if not
     */
    bool drv_led_on_off(void *p_handle, bool on);

    /**
     * @brief Turn the LED
     * @param p_handle Pointer to the led instance
     * @return true if successful, false if not
     */
    bool drv_led_toggle(void *p_handle);

#endif // USE_LED_GPIO
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* drv_led_h_ */
