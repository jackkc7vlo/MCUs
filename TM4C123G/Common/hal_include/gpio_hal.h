/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       gpio_hal.h
 * @author     Jack Wilson
 * @brief
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-19-2023     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 *
 * ==============================================================================
 * @endverbatim
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************/

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef gpio_hal_h_
#define gpio_hal_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /********************************************************************************
     * Defines
     ********************************************************************************/
#define PORTA 0u
#define PORTB 1u
#define PORTC 2u
#define PORTD 3u
#define PORTE 4u
#define PORTF 5u
#define PORTG 6u
#define PORTH 7u
#define PORTI 8u

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    //<! Pointer to an incomplete type (hides implementation)
    typedef struct gpio_hal_def_s *p_gpio_hal_def_t;

    typedef void (*p_gpio_callback_t)(uint8_t pin);

    typedef enum
    {
        GPIO_HAL_MODE_INPUT = 0u,             /**< Input */
        GPIO_HAL_MODE_OUTPUT = 1u,            /**< General Purpose Output */
        GPIO_HAL_MODE_ALTERNATE = 2u,         /**< Alternate function mode */
        GPIO_HAL_MODE_ANALOG = 3u,            /**< Analog mode */
        GPIO_HAL_MODE_WIRED_AND_PULLEDUP = 4u /**< Wired and pulled up */
    } gpio_hal_mode_t;

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    /// *****************************************************************************
    /// @brief create a gpio device
    /// @param gpio_port The port
    /// @param gpio_pin The pin
    /// @param mode The mode (Input, Output, Alternate, Analog)
    /// @param pull The pullup/pulldown configuration
    /// @param p_callback The optional callback function (can be NULL)
    /// @return Pointer to the device
    /// @note Typically this is done once per device, but may need to be done on every
    ///       wakeup depending on the sleep mode
    /// @warning  It is important to free the device when done with it @see gpio_hal_free
    /// *****************************************************************************
    p_gpio_hal_def_t gpio_hal_create_device(const uint32_t gpio_port, const uint32_t gpio_pin,
                                            const gpio_hal_mode_t mode, const uint32_t pull,
                                            const p_gpio_callback_t p_callback);

    /// *****************************************************************************
    /// @brief Free the gpio device
    /// @param p_gpio
    /// @return none
    /// *****************************************************************************
    void gpio_hal_free(const p_gpio_hal_def_t p_gpio);

    /// *****************************************************************************
    /// @brief Get the gpio device by index
    /// @param gpio_num
    /// @return Pointer to the device
    /// *****************************************************************************
    p_gpio_hal_def_t gpio_hal_get(const uint32_t gpio_num);

    /// *****************************************************************************
    /// @brief Initialize the gpio device.
    /// @param p_gpio Pointer to the gpio device @see gpio_hal_create_device
    /// @return none
    /// @note: assumes the CMU GPIO clock is already enabled
    /// *****************************************************************************
    void gpio_hal_init(const p_gpio_hal_def_t p_gpio);

    /// *****************************************************************************
    /// @brief Set the gpio state
    /// @param p_gpio Pointer to the gpio device @see gpio_hal_create_device
    /// @param state The state to set (on or off)
    /// @return none
    /// *****************************************************************************
    void gpio_hal_set_state(const p_gpio_hal_def_t p_gpio, const bool state);

    /// *****************************************************************************
    /// @brief Get the gpio state
    /// @param p_gpio Pointer to the gpio device @see gpio_hal_create_device
    /// @return The state of the gpio (on or off)
    /// *****************************************************************************
    bool gpio_hal_get_state(const p_gpio_hal_def_t p_gpio);

    /// *****************************************************************************
    /// @brief Toggle the gpio state
    /// @param p_gpio Pointer to the gpio device @see gpio_hal_create_device
    /// @return none
    /// *****************************************************************************
    void gpio_hal_toggle_state(const p_gpio_hal_def_t p_gpio);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* gpio_hal_h_ */
