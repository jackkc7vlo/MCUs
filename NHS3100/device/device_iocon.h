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
 * @file      device_iocon.h
 * @defgroup  device_iocon I/O Configuration (IOCON)
 * @author    Jack Wilson
 * @brief     Defines and Macros for the I/O Configuration (IOCON)
 *
 * @details   The I/O configuration registers control the electrical characteristics of the pads. The
 * following features are programmable:
 * • Pin function
 * • Internal pull-up/pull-down resistor or bus keeper function
 * • Low-pass filter
 * • I2C-bus mode for pads hosting the I2C-bus function
 * The IOCON registers control the function (GPIO or peripheral function), the input mode,
 * and the hysteresis of all PIO_m pins. In addition, the I2C-bus pins can be configured for
 * different I2C-bus modes.
 *
 * @mod
 * @moditem{V1.0.x, Nov-12-2025, Jack Wilson, Initial Release, jackkc7vlo@gmail.com }
 * @modend
 *
*/
 
/**
 * @file iocon.h
 * @author Jack Wilson (jackkc7vlo@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdint.h>
#ifndef DEVICE_IOCON_H
#define DEVICE_IOCON_H
//lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
//lint -restore
//lint -esym(751,iocon_*) There are definitions they may not be used
//lint -esym(754,iocon_*) There are definitions they may not be used 

// IOCON Base Address
#define REG_IOCON_BASE (0x40044000U)   //!< IOCON Base Address

/**
 * @brief IOCON registers structure definition
 */
typedef struct
{
    volatile uint32_t pio0_0;       //!< PIO0_0 / WAKEUP                         offset 0x00
    volatile uint32_t pio0_1;       //!< PIO0_1 / CLKOUT                         offset 0x04
    volatile uint32_t pio0_2;       //!< PIO0_2 / SPI SSEL                       offset 0x08
    volatile uint32_t pio0_3;       //!< PIO0_3 / CT16B_M0                       offset 0x0C
    volatile uint32_t pio0_4;       //!< PIO0_4 / IICSDA                         offset 0x10
    volatile uint32_t pio0_5;       //!< PIO0_5 / Reserved                       offset 0x14
    volatile uint32_t pio0_6;       //!< PIO0_6 / SPI SCLK                       offset 0x18
    volatile uint32_t pio0_7;       //!< PIO0_7 / CT16B_M1                       offset 0x1C
    volatile uint32_t pio0_8;       //!< PIO0_8 / SPI MISO                       offset 0x20
    volatile uint32_t pio0_9;       //!< PIO0_9 / SPI MOSI                       offset 0x24
    volatile uint32_t pio0_10;      //!< PIO0_10 / CT32B_M0 / SWCLK              offset 0x28
    volatile uint32_t pio0_11;      //!< PIO0_11 / CT32B_M1 / SWDIO              offset 0x2C
    volatile uint32_t ana0_0;       //!< ANA0_0 / ana_bus0                       offset 0x30
    volatile uint32_t ana0_1;       //!< ANA0_1 / ana_bus1                       offset 0x34
    volatile uint32_t ana0_2;       //!< ANA0_2 / ana_bus2                       offset 0x38
    volatile uint32_t ana0_3;       //!< ANA0_3 / ana_bus3                       offset 0x3C
    volatile uint32_t ana0_4;       //!< ANA0_4 / ana_bus4                       offset 0x40
    volatile uint32_t ana0_5;       //!< ANA0_5 / ana_bus5                       offset 0x44
    volatile uint32_t reserved[14]; //!< Reserved                                offset 0x48 - 0x7C
    volatile uint32_t anabusground; //!< Analog bus grounding control            offset 0x80
} reg_iocon_t;

/** @brief iocon registers
 */
static reg_iocon_t * p_iocon = (reg_iocon_t * const) REG_IOCON_BASE;  //!< The IOCON registers

#define IOCON_FUNC_BITMASK      (0x7U)          //!< Bits 0..2 Function bit mask
#define IOCON_FUNC_GPIO         (0U)              //!< Function GPIO
#define IOCON_FUNC_ALT1         (1U)              //!< Function ALT1
#define IOCON_FUNC_ALT2         (2U)              //!< Function ALT2

#define IOCON_FUNC_PIO0_0_ALT1    (1U)        //!< PIO0_0 function ALT1 WAKEUP
#define IOCON_FUNC_PIO0_1_ALT1    (1U)        //!< PIO0_1 function ALT1 CLKOUT
#define IOCON_FUNC_PIO0_2_ALT1    (1U)        //!< PIO0_2 function ALT1 SPI SSEL
#define IOCON_FUNC_PIO0_3_ALT1    (1U)        //!< PIO0_3 function ALT1 CT16B_M0
#define IOCON_FUNC_PIO0_4_ALT1    (1U)        //!< PIO0_4 function ALT1 IICSCL
#define IOCON_FUNC_PIO0_5_ALT1    (1U)        //!< PIO0_5 function ALT1 IICSDA
#define IOCON_FUNC_PIO0_6_ALT1    (1U)        //!< PIO0_6 function ALT1 SPI SCLK
#define IOCON_FUNC_PIO0_7_ALT1    (1U)        //!< PIO0_7 function ALT1 CT16B_M1
#define IOCON_FUNC_PIO0_8_ALT1    (1U)        //!< PIO0_8 function ALT1 SPI MISO
#define IOCON_FUNC_PIO0_9_ALT1    (1U)        //!< PIO0_9 function ALT1 SPI MOSI
#define IOCON_FUNC_PIO0_10_ALT1   (1U)        //!< PIO0_10 function ALT1 CT32B_M0
#define IOCON_FUNC_PIO0_11_ALT1   (1U)        //!< PIO0_11 function ALT1 CT32B_M1

#define IOCON_FUNC_PIO0_10_ALT2   (2U)        //!< PIO0_10 function ALT2 SWCLK
#define IOCON_FUNC_PIO0_11_ALT2   (2U)        //!< PIO0_11 function ALT2 SWDIO

#define IOCON_MODE_BITMASK        (3U<<3U)    //!< Bits 3..4 Mode bit mask
#define IOCON_MODE_INACTIVE       (0U<<3U)    //!< Inactive (no pull-down/pull-up resistor enabled)
#define IOCON_MODE_PULLDOWN       (1U<<3U)    //!< Pull-down resistor enabled
#define IOCON_MODE_PULLUP         (2U<<3U)    //!< Pull-up resistor enabled
#define IOCON_MODE_REPEATER       (3U<<3U)    //!< Repeater mode enabled

#define IOCON_LPF_BIT             (1U<<5U)    //!< Input filter enabled

#define IOCON_CDRIVE_BIT          (1U<<6U)    //!< High drive strength enabled
#define IOCON_DDRIVE_BIT          (1U<<7U)    //!< digital drive strength enabled

#define IOCON_I2C_MODE_BITMASK     (3U<<8U)    //!< Bits 8..9 I2C mode bit mask
#define IOCON_I2C_MODE_STANDARD    (0U<<8U)    //!< Standard/Fast mode I2C
#define IOCON_I2C_MODE_OPEN_DRAIN  (1U<<8U)    //!< open-drain GPIO



#define IOCON_HYS_BIT             (1U<<5U)       //!< Hysteresis


#endif // DEVICE_IOCON_H