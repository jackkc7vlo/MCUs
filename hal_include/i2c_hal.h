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
 * @file i2c_hal.h
 * @brief Generic I2C interface definition.
 * @details Outlines the portable I2C abstraction used to configure and
 * interact with I2C peripherals on any supported platform.
 *
 * @author  Jack Wilson
 * @date    February 12, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef I2C_HAL_H
#define I2C_HAL_H

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h> /*lint -e129*/

#if HW_CONFIG_I2C == 1u

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    // typedef struct i2c_hal_device_def_t *p_i2c_hal_device_def_t;
    typedef void (*p_i2c_callback_t)(void);

    /* Opaque i2c_hal type: definition is private and resides in the .c
     * implementation file. Consumers should only use the pointer type. */
    typedef struct i2c_hal  i2c_hal_t;
    typedef struct i2c_hal *p_i2c_hal_t;

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /********************************************************************************
     * @brief          Initialization function for the I2C driver
     * @retval         None
     ********************************************************************************/
    void i2c_hal_initialize(void);

    /********************************************************************************
     * @brief          Check if the I2C driver is initialized
     * @retval         true if initialized, false otherwise
     ********************************************************************************/
    bool i2c_hal_is_initialized(void);

    /********************************************************************************
     * @brief          Create an I2C device
     * @param[in]      i2c_board_id - The id of the I2C on the board (i.e 0, 1, 2..)
     * @param[in]      sda_port - the port used for the SDA line
     * @param[in]      sda_pin - the pin on the sda_port to use for the SDA line
     * @param[in]      scl_port - the port used for the SCL line
     * @param[in]      scl_pin - the pin on the scl_port to use for the SCL line
     * @param[in]      p_callback - Pointer to the callback function
     * @retval         Pointer to the I2C device
     ********************************************************************************/
    p_i2c_hal_t i2c_hal_create_device(const uint32_t i2c_board_id, const uint32_t sda_port, const uint32_t sda_pin,
                                      const uint32_t scl_port, const uint32_t scl_pin,
                                      const p_i2c_callback_t p_callback);

    /********************************************************************************
     * @brief          Remove an I2C device
     * @param[in]      p_handle - pointer to the i2c device
     * @retval         None
     ********************************************************************************/
    void i2c_hal_remove_device(p_i2c_hal_t p_handle);

    /********************************************************************************
     * @brief          Write function for the I2C driver
     * @param[in]      p_handle - pointer to the i2c device
     * @param[in]      cmd_or_register - The register or command to write data to
     *(i.e. memory address)
     * @param[in]      cmd_or_register_len - Length of the register or command in
     *bytes
     * @param[in]      data -  pointer to data to send (may be words depending on
     *MCU)
     * @param[in]      data_len - Length of data to write in bytes
     * @retval         Number of bytes written
     ********************************************************************************/
    uint32_t i2c_hal_write(const p_i2c_hal_t p_handle, const uint32_t slave_address, const uint32_t cmd_or_register,
                           const uint32_t cmd_or_register_len, const uint8_t *const data, const uint_fast16_t data_len);

    /********************************************************************************
     * @brief          Read function for the I2C driver
     * @param[in]      p_handle - pointer to the i2c device
     * @param[in]      cmd_or_register - The register or command to write data to
     *(i.e. memory address)
     * @param[in]      cmd_or_register_len - Length of the register or command in
     *bytes
     * @param[in]      data -  pointer to data (in bytes!) to read
     * @param[in]      data_len - Length of data to read in bytes
     * @retval         Number of bytes read
     ********************************************************************************/
    uint32_t i2c_hal_read(const p_i2c_hal_t p_handle, const uint32_t slave_address, const uint32_t cmd_or_register,
                          const uint32_t cmd_or_register_len, uint8_t *const data, const uint_fast16_t data_len);

    /********************************************************************************
     * @brief  Enable (power on) or disable (power off) to the i2c devices
     * @param[in]      p_handle - pointer to the i2c device
     * @param[in]      turn_on - enable or disable the i2c devices (POWER_ON or
     *POWER_OFF)
     * @param[in]      settle_time_ms - number of milliseconds to wait for the i2c
     *                                  devices power on
     * @retval
     ********************************************************************************/
    void i2c_hal_enable(const p_i2c_hal_t p_handle, const bool turn_on, const uint32_t settle_time_ms);

    /**
     * @brief Register a callback to be invoked on GPIO interrupt events.
     * @param[in] handle Pointer to platform-specific context associated with the
     * callback.
     * @param callback Function to invoke when an interrupt fires. May be NULL to
     * clear the registration.
     *
     * @return 0 on success, negative value when registration fails.
     */
    int i2c_hal_register_callback(const p_i2c_hal_t p_handle, p_i2c_callback_t callback, void *p_callback_context);

#if USE_I2C_INTERRUPTS == 1
    /**
     * @brief  Handle an I2C interrupt
     * @retval None
     */
    void i2c_hal_irqhandler(void);
#endif

    /********************************************************************************
     * @brief          Scan for i2c devices on the bus
     * @param[in]      i2c_board_id - The id of the I2C on the board (i.e 0, 1, 2..)
     * @param[in]      sda_port - the port used for the SDA line
     * @param[in]      sda_pin - the pin on the sda_port to use for the SDA line
     * @param[in]      scl_port - the port used for the SCL line
     * @param[in]      scl_pin - the pin on the scl_port to use for the SCL line
     * @param[in]      last_found_address - the last address that was found (use 0 for initial scan)
     * @retval         address of the first detected device on the bus, or 0 if no devices are found
     ********************************************************************************/
    uint32_t i2c_hal_scan(const uint32_t i2c_board_id, const uint32_t sda_port, const uint32_t sda_pin,
                          const uint32_t scl_port, const uint32_t scl_pin, uint32_t last_found_address);

#endif /* USE_I2C */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* I2C_HAL_H */
