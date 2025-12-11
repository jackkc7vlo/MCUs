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
 * @file drv_pcf8574a.h
 * @brief Driver for the PCF8574A I2C GPIO expander
 * @details Outlines the driver functions for the PCF8574A I2C GPIO expander
 *
 * @author  Jack Wilson
 * @date    November 24, 2025
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef drv_pcf8574a_h_
#define drv_pcf8574a_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <hw_config.h>
#include <stdbool.h>
#include <stdint.h>

#if HAS_PCF8574A == 1u
/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs & Enums
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/**
 * @brief Initialize the PCF8574A I2C GPIO expander
 * @param p_config Pointer to the configuration structure
 * @return true if successful, false if not
 */
void *drv_pcf8574a_init(void);

/**
 * @brief Write data to the PCF8574A I2C GPIO expander

 * @param data The data to write
 * @note Only the lower 8 bits are used
 *
 */
void drv_pcf8574a_write(void *p_device, uint32_t data);

/**
 * @brief Read data from the PCF8574A I2C GPIO expander
 * @param input_mask The bits to read from
 * @return The data read from the PCF8574A
 * @note Only the lower 8 bits are used
 */
uint32_t drv_pcf8574a_read(void *p_device);

#endif
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* drv_pcf8574a.h_ */
