/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       i2c_hal.h
 * @author     Jack Wilson
 * @brief
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Dec-11-2023     Jack Wilson                     jackkc7vlo@gmail.com
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
#ifndef i2c_hal_h_
#define i2c_hal_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif              /* __cplusplus */
#include <stdint.h> /*lint -e129*/
#include <hw_config.h>

#if USE_I2C == 1u

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    typedef struct i2c_hal_def_s *p_i2c_hal_def_t;
    typedef void (*p_i2c_callback_t)(void);

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /********************************************************************************
     * @brief          Initialization function for the I2C driver
     * @retval         None
     ********************************************************************************/
    void i2c_hal_initialize(const uint32_t speed);

    /********************************************************************************
     * @brief          Check if the I2C driver is initialized
     * @retval         true if initialized, false otherwise
     ********************************************************************************/
    bool i2c_hal_is_initialized(void);

    /********************************************************************************
     * @brief          Create an I2C device
     * @param[in]      i2c_device_id - I2C device ID
     * @param[in]      device_id_len - Length of the ID in bytes
     * @param[in]      p_callback - Pointer to the callback function
     * @retval         Pointer to the I2C device
     ********************************************************************************/
    p_i2c_hal_def_t i2c_hal_create_device(const uint32_t i2c_device_id,
                                          const uint32_t device_id_len,
                                          const p_i2c_callback_t p_callback);

    /********************************************************************************
     * @brief          Free an I2C device
     * @param[in]      p_i2c_device - pointer to the i2c device
     * @retval         None
     * @note           This function will free the memory allocated for the device
     *                and remove it from the list of devices
     ********************************************************************************/
    void i2c_hal_free(const p_i2c_hal_def_t p_i2c_device);

    /********************************************************************************
     * @brief          Write function for the I2C driver
     * @param[in]      p_i2c_device - pointer to the i2c device
     * @param[in]      cmd_or_register - The register or command to write data to (i.e. memory address)
     * @param[in]      cmd_or_register_len - Length of the register or command in bytes
     * @param[in]      data -  pointer to data to send (may be words depending on MCU)
     * @param[in]      data_len - Length of data to write in bytes
     * @retval         Number of bytes written
     ********************************************************************************/
    uint_fast16_t i2c_hal_write(const p_i2c_hal_def_t p_i2c_device, const uint32_t cmd_or_register,
                                const uint32_t cmd_or_register_len,
                                const uint8_t *const data, const uint_fast16_t data_len);

    /********************************************************************************
     * @brief          Read function for the I2C driver
     * @param[in]      i2c_device_id - I2C device ID
     * @param[in]      cmd_or_register - The register or command to write data to (i.e. memory address)
     * @param[in]      cmd_or_register_len - Length of the register or command in bytes
     * @param[in]      data -  pointer to data (in bytes!) to read
     * @param[in]      data_len - Length of data to read in bytes
     * @retval         Number of bytes read
     ********************************************************************************/
    uint_fast16_t i2c_hal_read(const p_i2c_hal_def_t p_i2c_device, const uint32_t cmd_or_register,
                               const uint32_t cmd_or_register_len,
                               uint8_t *const data, const uint_fast16_t data_len);

    /********************************************************************************
     * @brief  Enable (power on) or disable (power off) to the i2c devices
     * @param[in]      turn_on - enable or disable the i2c devices (POWER_ON or POWER_OFF)
     * @param[in]      settle_time_ms - number of milliseconds to wait for the i2c
     *                                  devices power on
     * @retval
     ********************************************************************************/
    void i2c_hal_enable(const bool turn_on, const uint32_t settle_time_ms);

#if USE_I2C_INTERRUPTS == 1
    /**
     * @brief  Handle an I2C interrupt
     * @retval None
     */
    void i2c_hal_irqhandler(void);
#endif

#if UNIT_TESTS == 1u
    void i2c_hal_unit_test(void);
#endif

#endif /* USE_I2C */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* i2c_hal.h_ */
