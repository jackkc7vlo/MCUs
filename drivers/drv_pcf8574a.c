/**
 * ****************************(C) COPYRIGHT 2024 Old Man Software
 * *****************
 * @file       drv_pcf8574a.c
 * @author     Jack Wilson
 * @brief      Driver for the PCF8574A I2C GPIO expander
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Feb-19-2024     Jack Wilson jackkc7vlo@gmail.com
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
 * ****************************(C) COPYRIGHT 2024 Old Man Software
 * *****************/

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "i2c_hal.h"
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> /*lint -e129*/
#if UNIT_TESTS == 1u
#include <clock_hal.h>
#include <unittest.h>
#endif

#if HAS_PCF8574A == 1u
    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    // static void*p_pcf8574a_i2c_device = NULL;

    static uint32_t pcf8574a_mask;
    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /**
     * @brief Initialize the PCF8574A I2C GPIO expander
     * @return true if successful, false if not
     */
    void *drv_pcf8574a_init(void)
    {
        void *p_pcf8574a_i2c_device = i2c_hal_create_device(1u, NULL);
        pcf8574a_mask               = 0xffu;
        return p_pcf8574a_i2c_device;
    }

    /**
     * @brief Write data to the PCF8574A I2C GPIO expander
     * @param data The data to write
     * @note Only the lower 8 bits are used
     *
     */
    void drv_pcf8574a_write(void *p_device, uint32_t data)
    {
        data &= pcf8574a_mask;
        i2c_device_hal_t *p_i2c_dev = (i2c_device_hal_t *)p_device;

        p_i2c_dev->write(p_i2c_dev, 0u, 0u, (uint8_t *)&data, 1u);
        //     i2c_hal_write(p_i2c_dev->i2c_device_handle, 0u, 0u, (uint8_t *)&data,
        //     1u);
        //  uint8_t read_data[2];
        //(void)i2c_hal_read(p_pcf8574a_i2c_device, 0x1u, 1u, read_data, 0u);
    }

    /**
     * @brief Read data from the PCF8574A I2C GPIO expander
     * @return The data read from the PCF8574A
     * @note Only the lower 8 bits are used
     */
    uint32_t drv_pcf8574a_read(void *p_device)
    {
        i2c_device_hal_t *p_i2c_dev = (i2c_device_hal_t *)p_device;
        uint8_t           read_data[2];
        (void)p_i2c_dev->read(p_i2c_dev, 0xffu, 1u, read_data,
                              //(void)i2c_hal_read(p_i2c_dev->i2c_device_handle,
                              // pcf8574a_mask, 1u, read_data,
                              1u);
        return (uint32_t)read_data[0]; // Only the lower 8 bits are used
    }

#if UNIT_TESTS == 1u
    void drv_pcf8574a_unit_test(void)
    {
        unittest_start_test("drv_pcf8574a_unit_test");
        unittest_start_testcase("drv_pcf8574a_init");
        i2c_hal_enable(true, 10u);

        (void)unittest_check_true(drv_pcf8574a_init(PCF8574A_I2C_ADDRESS, 0xffu));
        for (uint32_t i = 0; i < 0x100u; i++)
        {
            drv_pcf8574a_write(i);
            clock_hal_delay(10u);
        }
        drv_pcf8574a_write(0xffu);
        uint32_t data = drv_pcf8574a_read();
        (void)unittest_check_true(data == 0x55);
        i2c_hal_enable(false, 10u);
        unittest_end_test();
    }
#endif

#endif
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
