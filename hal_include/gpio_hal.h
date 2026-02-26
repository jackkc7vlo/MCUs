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

#ifndef MCUS_HAL_INCLUDE_GPIO_HAL_H
#define MCUS_HAL_INCLUDE_GPIO_HAL_H

#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup Port_Pin_Macros Pin Manipulation Macros
 * @brief Macros for pin and port manipulation
 * @{
 */
#define PORT_PIN_MASK(pin) (1U << (pin))
#define PORT_PIN_SET(reg, pin) ((reg) |= PORT_PIN_MASK(pin))
#define PORT_PIN_CLEAR(reg, pin) ((reg) &= ~PORT_PIN_MASK(pin))
#define PORT_PIN_TOGGLE(reg, pin) ((reg) ^= PORT_PIN_MASK(pin))
#define PORT_PIN_IS_SET(reg, pin) (((reg) & PORT_PIN_MASK(pin)) != 0U)
    /** @} */

#define GPIOA_PORT 0
#define GPIOB_PORT 1
#define GPIOC_PORT 2
#define GPIOD_PORT 3
#define GPIOE_PORT 4
#define GPIOF_PORT 5
#define GPIOG_PORT 6
#define GPIOH_PORT 7

    /**
     * @brief Port pin state enumeration
     */
    typedef enum
    {
        PORT_STATE_LOW = 0, /**< Pin state is logic low */
        PORT_STATE_HIGH     /**< Pin state is logic high */
    } port_state_t;

    /**
     * @brief Port pin direction
     */
    typedef enum
    {
        PIN_DIRECTION_INPUT = 0, /**< Input Pin */
        PIN_DIRECTION_OUTPUT,    /**< Output Pin */
        PIN_DIRECTION_ALT,       /**< Alternate Function */
        PIN_DIRECTION_ANALOG,    /**< Analog */
        PIN_DIRECTION_BIDIRECTIONAL
    } pin_direction_t;

    typedef enum
    {
        IRQ_POSITIVE = 0, /**< Input Pin */
        IRQ_NEGATIVE,     /**< Output Pin */
        IRQ_BOTH,         /**< Alternate Function */
        IRQ_NONE,         /**< Analog */
        IRQ_RESERVED
    } irq_edge_t;

    /**
     * @brief Port pin mode
     */
    typedef enum
    {
        FLOAT,     /**< Floating (High Impedance) Input */
        PULLUP,    /**< PULLUP Resistor */
        PULLDOWN,  /**< PULLDOWN Resistor */
        OPENDRAIN, /**< Open Drain Output */
        PUSHPULL,  /**< Push Pull Output */
    } pin_mode_t;

    /**
     * @brief Port pin speed
     */
    typedef enum
    {
        LOWSPEED,    /**< Floating (High Impedance) Input */
        MEDIUMSPEED, /**< PULLUP Resistor */
        FASTSPEED,   /**< PULLDOWN Resistor */
        HIGHSPEED,   /**< Open Drain Output */
    } pin_speed_t;

    typedef struct gpio_hal_handle
    {

        uint32_t open_drain_mask; /**< Open-drain configuration mask for GPIO pins (1
                                     = enabled, 0 = disabled) */
    } gpio_hal_handle_t, *p_gpio_hal_handle_t;

    /**
     * @typedef gpio_hal_interrupt_callback_t
     * @brief Prototype for GPIO interrupt callbacks.
     *
     * Implementations invoke the registered callback when a platform-specific
     * GPIO interrupt occurs. The @p handle argument points to the same context
     * passed during registration, allowing the callback to access hardware state.
     */
    typedef void (*gpio_hal_interrupt_callback_t)(void *handle, void *callback_context);

    /**
     * @struct gpio_hal_t
     * @brief Hardware Abstraction Layer interface for General-Purpose IO.
     *
     * The structure exposes platform hooks for initializing GPIO hardware and
     * performing basic pin manipulation in a portable manner.
     */

    /* Opaque gpio_hal type: definition is private and resides in the .c
     * implementation file. Consumers should only use the pointer type. */
    typedef struct gpio_hal  gpio_hal_t;
    typedef struct gpio_hal *p_gpio_hal_t;

    /**
     * @brief Create a GPIO HAL instance for a specific port.
     *
     * Allocates or returns a handle representing the GPIO port @p port.
     * The returned handle is used with all other HAL functions.
     *
     * @param port Port identifier (use GPIOA_PORT..GPIOH_PORT).
     * @return Pointer to a `gpio_hal_t` instance on success, or NULL on failure.
     */
    // p_gpio_hal_t gpio_hal_create(uint32_t port);

    /**
     * @brief Create a GPIO HAL instance for a specific port.
     *
     * Allocates or returns a handle representing the GPIO port @p port.
     * The returned handle is used with all other HAL functions.
     *
     * @param port Port identifier (use GPIOA_PORT..GPIOH_PORT).
     * @return Pointer to a `gpio_hal_t` instance on success, or NULL on failure.
     */
    p_gpio_hal_t gpio_hal_create(uint32_t port);

    /**
     * @brief Remove the GPIO HAL instance.
     *
     * Performs any platform specific deinitialization required before using the
     * GPIO HAL (e.g. disable clocks, configure register access). The @p p_handle
     * should point to the `gpio_hal_t` instance returned from `gpio_hal_create`.
     *
     * @param p_handle Pointer to the HAL instance (input).
     */
    void gpio_hal_remove(p_gpio_hal_t p_handle);

    /**
     * @brief Initialize the GPIO HAL instance.
     *
     * Performs any platform specific initialization required before using the
     * GPIO HAL (e.g. enable clocks, configure register access). The @p p_handle
     * should point to the `gpio_hal_t` instance returned from `gpio_hal_create`.
     *
     * @param p_handle Pointer to the HAL instance (input).
     */
    void gpio_hal_init(p_gpio_hal_t p_handle);

    /**
     * @brief Set an individual GPIO pin state.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @param pin Pin index within the port (0..31).
     * @param value true to drive the pin high, false to drive low.
     */
    void gpio_hal_set_state(p_gpio_hal_t p_handle, uint8_t pin, bool value);

    /**
     * @brief Read the current state of a GPIO pin.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @param pin Pin index within the port (0..31).
     * @return true if the pin is high, false if low.
     */
    bool gpio_hal_get_state(p_gpio_hal_t p_handle, uint8_t pin);

    /**
     * @brief Toggle the state of a GPIO pin.
     *
     * Inverts the current output state of the specified pin.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @param pin Pin index within the port (0..31).
     */
    void gpio_hal_toggle_state(p_gpio_hal_t p_handle, uint8_t pin);

    /**
     * @brief Write an 8-bit value to the port data register.
     *
     * The meaning of the written bits depends on the platform; typically the
     * lower N bits correspond to the first pins of the port.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @param value 8-bit value to write to the port.
     */
    void gpio_hal_write_port(p_gpio_hal_t p_handle, uint8_t value);

    /**
     * @brief Read the current 8-bit port value.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @return 8-bit value representing the current port input states.
     */
    uint8_t gpio_hal_read_port(p_gpio_hal_t p_handle);

    /**
     * @brief Configure the direction of a pin.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @param pin Pin index within the port (0..31).
     * @param value Direction to configure (`INPUT`, `OUTPUT`, `ALT`, ...).
     * @retval true on success
     * @retval false on failure or invalid parameter
     */
    bool gpio_hal_pin_direction(p_gpio_hal_t p_handle, uint8_t pin, pin_direction_t value);

    /**
     * @brief Configure the mode (pull/up/down/open-drain) for a pin.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @param pin Pin index within the port (0..31).
     * @param value Mode to set (FLOAT, PULLUP, PULLDOWN, ...).
     * @retval true on success
     * @retval false on failure or invalid parameter
     */
    bool gpio_hal_pin_mode(p_gpio_hal_t p_handle, uint8_t pin, pin_mode_t value);

    /**
     * @brief Configure the speed characteristics for a pin.
     *
     * Some platforms support multiple drive/slew rate settings for GPIO outputs.
     * This function configures that characteristic if available.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @param pin Pin index within the port (0..31).
     * @param value Speed setting to apply (LOWSPEED..HIGHSPEED).
     * @retval true on success
     * @retval false on failure or unsupported on platform
     */
    bool gpio_hal_pin_speed(p_gpio_hal_t p_handle, uint8_t pin, pin_speed_t value);

    /**
     * @brief Set the alternate function number (PCTL) for a pin.
     *
     * Should be called after setting the pin direction to PIN_DIRECTION_ALT.
     * The alt_func value selects which peripheral function is routed to the pin.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @param pin Pin index within the port (0..7).
     * @param alt_func Alternate function number (0..15, platform-specific).
     * @retval true on success
     * @retval false on failure or invalid parameter
     */
    bool gpio_hal_set_alt_function(p_gpio_hal_t p_handle, uint8_t pin, uint8_t alt_func);

    /**
     * @brief Register an interrupt callback for a specific GPIO pin.
     *
     * When the configured edge(s) occur on @p pin, the provided @p callback
     * will be invoked with @p p_callback_context and @p p_callback_handle.
     *
     * @param p_handle Pointer to the HAL instance (input).
     * @param p_callback_handle Opaque pointer passed back as the first callback argument.
     * @param pin Pin index to monitor for interrupts.
     * @param callback Function to call on interrupt (may be NULL to unregister).
     * @param p_callback_context User-provided context pointer passed to callback.
     * @param edge Edge type to trigger on (`IRQ_POSITIVE`, `IRQ_NEGATIVE`, `IRQ_BOTH`).
     * @retval true on success
     * @retval false on failure or if interrupts are unsupported
     */
    bool gpio_hal_register_callback(p_gpio_hal_t p_handle, const void *p_callback_handle, uint8_t pin,
                                    gpio_hal_interrupt_callback_t callback, void *p_callback_context, irq_edge_t edge);

#ifdef __cplusplus
}
#endif

#endif /* MCUS_HAL_INCLUDE_GPIO_HAL_H */
