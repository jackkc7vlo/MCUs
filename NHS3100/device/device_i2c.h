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
 * @file      device_i2c.h
 * @defgroup  i2c Configuration (I2C)
 * @author    Jack Wilson
 * @brief     Defines and Macros for the I2C Configuration (I2C)
 *
 * @details   The I2C configuration registers control the electrical characteristics of the
 *  I2C pads. The I2C-bus interface is byte oriented and has four operating modes: master
 * transmitter mode, master receiver mode, slave transmitter mode, and slave receiver mode. The I2C
 * interface is completely I2C-bus compliant, supporting the ability to power off the NHS31xx
 * independent of other devices on the same I2C-bus.
 *
 *  The I2C-bus interface receives an 8 MHz clock signal from the system oscillator.
 *
 * @mod
 * @moditem{V1.0.x, Nov-12-2025, Jack Wilson, Initial Release, jackkc7vlo@gmail.com }
 * @modend
 *
 */

/**
 * @file device_i2c.h
 * @author Jack Wilson (jackkc7vlo@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <stdint.h>
#ifndef DEVICE_I2C_H
#define DEVICE_I2C_H
// lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
// lint -restore
// lint -esym(751,i2c_*) There are definitions they may not be used
// lint -esym(754,i2c_*) There are definitions they may not be used

// I2C Base Address
#define REG_I2C_BASE (0x40000000U) //!< I2C Base Address

/**
 * @brief I2C registers structure definition
 */
typedef struct
{
    volatile uint32_t conset;      //!< I2C Control Set Register              offset 0x00
    volatile uint32_t stat;        //!< I2C Status Register                   offset 0x04
    volatile uint32_t dat;         //!< I2C Data Register                     offset 0x08
    volatile uint32_t adr0;        //!< I2C Address Register 0                offset 0x0C
    volatile uint32_t sclh;        //!< I2C SCL High Register                 offset 0x10
    volatile uint32_t scll;        //!< I2C SCL Low Register                  offset 0x14
    volatile uint32_t conclr;      //!< I2C Control Register                  offset 0x18
    volatile uint32_t mmctrl;      //!< I2C Status Register                   offset 0x1C
    volatile uint32_t adr1;        //!< I2C Data Register                     offset 0x20
    volatile uint32_t adr2;        //!< I2C Address Register                  offset 0x24
    volatile uint32_t adr3;        //!< I2C Address Register                  offset 0x28
    volatile uint32_t data_buffer; //!< I2C Data Buffer Register              offset 0x2C
    volatile uint32_t mask[4];     //!< I2C Mask Register 0                   offset 0x30
} reg_i2c_t;

/** @brief i2c registers
 */
static reg_i2c_t *p_i2c = (reg_i2c_t *const)REG_I2C_BASE; //!< The I2C registers

#define NSS_I2Cx(id) (NSS_I2C)
/** I2C Common Control Assert acknowledge bit */
#define I2C_CON_AA (1UL << 2)
/** I2C Common Control I2C interrupt bit */
#define I2C_CON_SI (1UL << 3)
/** I2C Common Control I2C STOP bit */
#define I2C_CON_STO (1UL << 4)
/** I2C Common Control START flag bit */
#define I2C_CON_STA (1UL << 5)
/** I2C Common Control I2C interface bit */
#define I2C_CON_I2EN (1UL << 6)

/** Return Code mask in I2C status register */
#define I2C_STAT_CODE_BITMASK ((0xF8u))
/** Return Code error mask in I2C status register */
#define I2C_STAT_CODE_ERROR ((0xFF))

#define SLAVE_ACTIVE(iic) (((iic)->flags & 0xFF00) != 0)

#define I2C_CON_FLAGS (I2C_CON_AA | I2C_CON_SI | I2C_CON_STO | I2C_CON_STA)

#endif // DEVICE_I2C_H