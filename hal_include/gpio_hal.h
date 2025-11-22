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

#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#include <device_gpio.h>
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>

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
typedef enum {
  PORT_STATE_LOW = 0, /**< Pin state is logic low */
  PORT_STATE_HIGH     /**< Pin state is logic high */
} port_state_t;

typedef struct gpio_hal_handle {

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
typedef void (*gpio_hal_interrupt_callback_t)(void *handle,
                                              void *callback_context);

/**
 * @struct gpio_hal_t
 * @brief Hardware Abstraction Layer interface for General-Purpose IO.
 *
 * The structure exposes platform hooks for initializing GPIO hardware and
 * performing basic pin manipulation in a portable manner.
 */
typedef struct gpio_hal {
  void *config_handle; /**< Pointer to platform-specific context passed to the
                          HAL implementation. */
  reg_gpio_t *p_device_gpio; /**< Pointer to the GPIO port device registers. */
  /**
   * @brief Initialize the GPIO subsystem.
   *
   * @param handle Pointer to platform-specific context passed to the HAL
   * implementation.
   *
   * @return 0 on success, negative value when initialization fails.
   */
  void (*init)(const void *p_handle);

  /**
   * @brief Set the logical state of a GPIO pin.
   *
   * @param handle Pointer to platform-specific context passed to the HAL
   * implementation.
   * @param pin Identifier of the pin to configure.
   * @param value Logical value to drive on the pin.
   *
   */
  void (*set)(const void *p_handle, uint8_t pin, bool value);

  /**
   * @brief Read the logical state of a GPIO pin.
   *
   * @param handle Pointer to platform-specific context passed to the HAL
   * implementation.
   * @param pin Identifier of the pin to sample.
   *
   * @return Logical value of the pin on success, negative value when the read
   * fails.
   */
  bool (*get)(const void *p_handle, uint8_t pin);

  /**
   * @brief Toggle the logical state of a GPIO pin.
   *
   * @param handle Pointer to platform-specific context passed to the HAL
   * implementation.
   * @param pin Identifier of the pin to sample.
   *
   */
  void (*toggle)(const void *p_handle, uint8_t pin);

  /**
   * @brief Write to the GPIO port.
   *
   * @param handle Pointer to platform-specific context passed to the HAL
   * implementation.
   * @param value Logical value to drive on the pin.
   *
   * @return 0 on success, negative value when the operation fails.
   */
  void (*write)(const void *p_handle, uint8_t value);

  /**
   * @brief Read from the GPIO port.
   *
   * @param handle Pointer to platform-specific context passed to the HAL
   * implementation.
   * @param value Logical value to drive on the pin.
   *
   * @return 0 on success, negative value when the operation fails.
   */
  uint8_t (*read)(const void *p_handle);

  /**
   * @brief Register a callback to be invoked on GPIO interrupt events.
   *
   * @param handle Pointer to platform-specific context associated with the
   * callback.
   * @param callback Function to invoke when an interrupt fires. May be NULL to
   * clear the registration.
   *
   * @return 0 on success, negative value when registration fails.
   */
  int (*register_callback)(const void *p_handle,
                           gpio_hal_interrupt_callback_t callback,
                           void *p_callback_context);
} gpio_hal_t, *p_gpio_hal_t;

p_gpio_hal_t gpio_hal_create(uint32_t port);

#endif /* GPIO_HAL_H */
