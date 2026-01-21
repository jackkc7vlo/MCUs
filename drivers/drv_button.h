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
 * @file drv_button.h
 * @brief Driver for a simple GPIO button
 * @details Outlines the driver functions for a simple GPIO button
 *
 * @author  Jack Wilson
 * @date    January 08, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef drv_button_h_
#define drv_button_h_

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

#if USE_BUTTON_GPIO == 1u
    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    /**
     * @brief Button state enumeration
     */
    typedef enum
    {
        BUTTON_RELEASED = 0, /**< Button is released */
        BUTTON_PRESSED       /**< Button is pressed */
    } button_state_t;

    typedef void (*p_button_callback_t)(button_state_t button_state);

    typedef void *p_button_handle_t;
    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /**
     * @brief Create a GPIO button instance
     * @param port The GPIO port
     * @param pin The GPIO pin
     * @param p_callback Pointer to the callback function - can be NULL
     * @return Pointer to the button instance or NULL if error
     */
    void *drv_button_create(uint32_t port, uint8_t pin, p_button_callback_t p_callback);
    /**
     * @brief Initialize the GPIO button
     * @param p_handle Pointer to the button instance
     * @return true if successful, false if not
     */
    bool drv_button_init(const void *p_handle);
    /**
     * @brief Check if the button is pressed
     * @param p_handle Pointer to the button instance
     * @return true if pressed, false if not
     *
     */
    bool drv_button_is_pressed(void *p_handle);

#endif // USE_BUTTON_GPIO
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* drv_button_h_ */
