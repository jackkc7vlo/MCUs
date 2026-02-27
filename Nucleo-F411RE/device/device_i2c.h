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
 * @file device_i2c.h
 * @brief  Defines registers used for I2C peripherals on STM32F401xE.
 *
 * @author  Jack Wilson
 * @date    February 26, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

#ifndef DEVICE_I2C_H
#define DEVICE_I2C_H

#include "device_regs.h"
#include <stdint.h>

// lint -esym(528,*) There are definitions they may not be used
// lint -esym(750,*) There are definitions they may not be used
// lint -esym(751,*) There are definitions they may not be used
// lint -esym(754,*) There are definitions they may not be used
// lint -esym(749,*) There are definitions they may not be used

// NOTE:  This does not define every possible register or bit
// More can be added from the datasheet

/********************************************************************************
 * I2C peripheral base addresses (STM32F401xE)
 ********************************************************************************/
#define REG_I2C1_BASE (REG_APB1PERIPH_BASE + 0x5400U)
#define REG_I2C2_BASE (REG_APB1PERIPH_BASE + 0x5800U)
#define REG_I2C3_BASE (REG_APB1PERIPH_BASE + 0x5C00U)

/********************************************************************************
 * I2C register map (RM0368 §18.6)
 ********************************************************************************/
typedef struct
{
    volatile uint32_t cr1;   // offset 0x00  Control register 1
    volatile uint32_t cr2;   // offset 0x04  Control register 2
    volatile uint32_t oar1;  // offset 0x08  Own address register 1
    volatile uint32_t oar2;  // offset 0x0C  Own address register 2
    volatile uint32_t dr;    // offset 0x10  Data register
    volatile uint32_t sr1;   // offset 0x14  Status register 1
    volatile uint32_t sr2;   // offset 0x18  Status register 2
    volatile uint32_t ccr;   // offset 0x1C  Clock control register
    volatile uint32_t trise; // offset 0x20  TRISE register
    volatile uint32_t fltr;  // offset 0x24  FLTR register
} reg_i2c_t;

/********************************************************************************
 * Peripheral pointer instances
 ********************************************************************************/
// lint -e844
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_i2c_t *p_device_i2c1 = ((reg_i2c_t *const)REG_I2C1_BASE);
static reg_i2c_t *p_device_i2c2 = ((reg_i2c_t *const)REG_I2C2_BASE);
static reg_i2c_t *p_device_i2c3 = ((reg_i2c_t *const)REG_I2C3_BASE);
#pragma GCC diagnostic pop

/********************************************************************************
 * CR1 bit definitions
 ********************************************************************************/
#define I2C_CR1_PE (1U << 0U)        /*!< Peripheral enable                */
#define I2C_CR1_SMBUS (1U << 1U)     /*!< SMBus mode                       */
#define I2C_CR1_SMBTYPE (1U << 3U)   /*!< SMBus type                       */
#define I2C_CR1_ENARP (1U << 4U)     /*!< ARP enable                       */
#define I2C_CR1_ENPEC (1U << 5U)     /*!< PEC enable                       */
#define I2C_CR1_ENGC (1U << 6U)      /*!< General call enable              */
#define I2C_CR1_NOSTRETCH (1U << 7U) /*!< Clock stretching disable         */
#define I2C_CR1_START (1U << 8U)     /*!< Start generation                 */
#define I2C_CR1_STOP (1U << 9U)      /*!< Stop generation                  */
#define I2C_CR1_ACK (1U << 10U)      /*!< Acknowledge enable               */
#define I2C_CR1_POS (1U << 11U)      /*!< Acknowledge/PEC position         */
#define I2C_CR1_PEC (1U << 12U)      /*!< Packet error checking            */
#define I2C_CR1_ALERT (1U << 13U)    /*!< SMBus alert                      */
#define I2C_CR1_SWRST (1U << 15U)    /*!< Software reset                   */

/********************************************************************************
 * CR2 bit definitions
 ********************************************************************************/
#define I2C_CR2_FREQ_MASK (0x3FU)   /*!< Peripheral clock frequency mask  */
#define I2C_CR2_ITERREN (1U << 8U)  /*!< Error interrupt enable           */
#define I2C_CR2_ITEVTEN (1U << 9U)  /*!< Event interrupt enable           */
#define I2C_CR2_ITBUFEN (1U << 10U) /*!< Buffer interrupt enable          */
#define I2C_CR2_DMAEN (1U << 11U)   /*!< DMA requests enable              */
#define I2C_CR2_LAST (1U << 12U)    /*!< DMA last transfer                */

/********************************************************************************
 * SR1 bit definitions
 ********************************************************************************/
#define I2C_SR1_SB (1U << 0U)        /*!< Start bit (Master mode)          */
#define I2C_SR1_ADDR (1U << 1U)      /*!< Address sent (master) / matched  */
#define I2C_SR1_BTF (1U << 2U)       /*!< Byte transfer finished           */
#define I2C_SR1_ADD10 (1U << 3U)     /*!< 10-bit header sent               */
#define I2C_SR1_STOPF (1U << 4U)     /*!< Stop detection (Slave mode)      */
#define I2C_SR1_RXNE (1U << 6U)      /*!< Data register not empty (rx)     */
#define I2C_SR1_TXE (1U << 7U)       /*!< Data register empty (tx)         */
#define I2C_SR1_BERR (1U << 8U)      /*!< Bus error                        */
#define I2C_SR1_ARLO (1U << 9U)      /*!< Arbitration lost                 */
#define I2C_SR1_AF (1U << 10U)       /*!< Acknowledge failure              */
#define I2C_SR1_OVR (1U << 11U)      /*!< Overrun / Underrun               */
#define I2C_SR1_PECERR (1U << 12U)   /*!< PEC Error in reception           */
#define I2C_SR1_TIMEOUT (1U << 14U)  /*!< Timeout or Tlow detection flag   */
#define I2C_SR1_SMBALERT (1U << 15U) /*!< SMBus alert                      */

/********************************************************************************
 * SR2 bit definitions
 ********************************************************************************/
#define I2C_SR2_MSL (1U << 0U)     /*!< Master/Slave                     */
#define I2C_SR2_BUSY (1U << 1U)    /*!< Bus busy                         */
#define I2C_SR2_TRA (1U << 2U)     /*!< Transmitter/Receiver             */
#define I2C_SR2_GENCALL (1U << 4U) /*!< General call address (Slave)     */
#define I2C_SR2_DUALF (1U << 7U)   /*!< Dual flag (Slave mode)           */

/********************************************************************************
 * CCR bit definitions
 ********************************************************************************/
#define I2C_CCR_CCR (0xFFFU)     /*!< Clock control register mask      */
#define I2C_CCR_DUTY (1U << 14U) /*!< Fm mode duty cycle               */
#define I2C_CCR_FS (1U << 15U)   /*!< I2C master mode selection (Fast) */

/********************************************************************************
 * Convenience constants
 ********************************************************************************/
#define I2C_DUTYCYCLE_2 (0U)            /*!< Fm t_low/t_high = 2  */
#define I2C_DUTYCYCLE_16_9 I2C_CCR_DUTY /*!< Fm t_low/t_high = 16/9 */

#define I2C_SPEED_STANDARD (100000U) /*!< 100 kHz standard mode            */
#define I2C_SPEED_FAST (400000U)     /*!< 400 kHz fast mode                */

#define I2C_TIMEOUT_FLAG (35U) /*!< Timeout in ms for flag polling   */

/********************************************************************************
 * I2C board-id to peripheral mapping helpers
 ********************************************************************************/
typedef enum
{
    I2C_BOARD_ID_1 = 0U,
    I2C_BOARD_ID_2 = 1U,
    I2C_BOARD_ID_3 = 2U,
} i2c_board_id_t;

#endif /* DEVICE_I2C_H */
