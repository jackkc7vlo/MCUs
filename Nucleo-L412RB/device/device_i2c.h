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
 * @brief  Defines registers used for I2C peripherals on STM32L412RB.
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
 * I2C peripheral base addresses (STM32L412RB)
 ********************************************************************************/
#define REG_I2C1_BASE (REG_APB1PERIPH_BASE + 0x5400U)
#define REG_I2C2_BASE (REG_APB1PERIPH_BASE + 0x5800U)
#define REG_I2C3_BASE (REG_APB1PERIPH_BASE + 0x5C00U)

/********************************************************************************
 * I2C register map (RM0394 §37.7 - STM32L4 I2C registers)
 ********************************************************************************/
typedef struct
{
    volatile uint32_t cr1;     // offset 0x00  Control register 1
    volatile uint32_t cr2;     // offset 0x04  Control register 2
    volatile uint32_t oar1;    // offset 0x08  Own address register 1
    volatile uint32_t oar2;    // offset 0x0C  Own address register 2
    volatile uint32_t timingr; // offset 0x10  Timing register
    volatile uint32_t timoutr; // offset 0x14  Timeout register
    volatile uint32_t isr;     // offset 0x18  Interrupt and status register
    volatile uint32_t icr;     // offset 0x1C  Interrupt clear register
    volatile uint32_t pecr;    // offset 0x20  PEC register
    volatile uint32_t rxdr;    // offset 0x24  Receive data register
    volatile uint32_t txdr;    // offset 0x28  Transmit data register
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
 * CR1 bit definitions (STM32L4)
 ********************************************************************************/
#define I2C_CR1_PE (1U << 0U)         /*!< Peripheral enable                */
#define I2C_CR1_TXIE (1U << 1U)       /*!< TX interrupt enable              */
#define I2C_CR1_RXIE (1U << 2U)       /*!< RX interrupt enable              */
#define I2C_CR1_ADDRIE (1U << 3U)     /*!< Address match interrupt enable   */
#define I2C_CR1_NACKIE (1U << 4U)     /*!< NACK interrupt enable            */
#define I2C_CR1_STOPIE (1U << 5U)     /*!< STOP detection interrupt enable  */
#define I2C_CR1_TCIE (1U << 6U)       /*!< Transfer complete interrupt en   */
#define I2C_CR1_ERRIE (1U << 7U)      /*!< Error interrupt enable           */
#define I2C_CR1_DNF (0xFU << 8U)      /*!< Digital noise filter             */
#define I2C_CR1_ANFOFF (1U << 12U)    /*!< Analog noise filter OFF          */
#define I2C_CR1_TXDMAEN (1U << 14U)   /*!< DMA transmission enable          */
#define I2C_CR1_RXDMAEN (1U << 15U)   /*!< DMA reception enable             */
#define I2C_CR1_SBC (1U << 16U)       /*!< Slave byte control               */
#define I2C_CR1_NOSTRETCH (1U << 17U) /*!< Clock stretching disable         */
#define I2C_CR1_GCEN (1U << 19U)      /*!< General call enable              */
#define I2C_CR1_SMBHEN (1U << 20U)    /*!< SMBus host address enable        */
#define I2C_CR1_SMBDEN (1U << 21U)    /*!< SMBus device default addr enable */
#define I2C_CR1_ALERTEN (1U << 22U)   /*!< SMBus alert enable               */
#define I2C_CR1_PECEN (1U << 23U)     /*!< PEC enable                       */

/********************************************************************************
 * CR2 bit definitions (STM32L4)
 ********************************************************************************/
#define I2C_CR2_SADD (0x3FFU << 0U)   /*!< Slave address (master mode)      */
#define I2C_CR2_RD_WRN (1U << 10U)    /*!< Transfer direction (1=read)      */
#define I2C_CR2_ADD10 (1U << 11U)     /*!< 10-bit addressing mode           */
#define I2C_CR2_HEAD10R (1U << 12U)   /*!< 10-bit address header only       */
#define I2C_CR2_START (1U << 13U)     /*!< Start generation                 */
#define I2C_CR2_STOP (1U << 14U)      /*!< Stop generation                  */
#define I2C_CR2_NACK (1U << 15U)      /*!< NACK generation                  */
#define I2C_CR2_NBYTES (0xFFU << 16U) /*!< Number of bytes                  */
#define I2C_CR2_RELOAD (1U << 24U)    /*!< NBYTES reload mode               */
#define I2C_CR2_AUTOEND (1U << 25U)   /*!< Automatic end mode               */
#define I2C_CR2_PECBYTE (1U << 26U)   /*!< Packet error checking byte       */

/********************************************************************************
 * ISR bit definitions (STM32L4 Interrupt and Status Register)
 ********************************************************************************/
#define I2C_ISR_TXE (1U << 0U)         /*!< Transmit data register empty     */
#define I2C_ISR_TXIS (1U << 1U)        /*!< Transmit interrupt status        */
#define I2C_ISR_RXNE (1U << 2U)        /*!< Receive data register not empty  */
#define I2C_ISR_ADDR (1U << 3U)        /*!< Address matched (slave)          */
#define I2C_ISR_NACKF (1U << 4U)       /*!< NACK received flag               */
#define I2C_ISR_STOPF (1U << 5U)       /*!< STOP detection flag              */
#define I2C_ISR_TC (1U << 6U)          /*!< Transfer complete                */
#define I2C_ISR_TCR (1U << 7U)         /*!< Transfer complete reload         */
#define I2C_ISR_BERR (1U << 8U)        /*!< Bus error                        */
#define I2C_ISR_ARLO (1U << 9U)        /*!< Arbitration lost                 */
#define I2C_ISR_OVR (1U << 10U)        /*!< Overrun/Underrun                 */
#define I2C_ISR_PECERR (1U << 11U)     /*!< PEC error in reception           */
#define I2C_ISR_TIMEOUT (1U << 12U)    /*!< Timeout/tLOW detection flag      */
#define I2C_ISR_ALERT (1U << 13U)      /*!< SMBus alert                      */
#define I2C_ISR_BUSY (1U << 15U)       /*!< Bus busy                         */
#define I2C_ISR_DIR (1U << 16U)        /*!< Transfer direction (slave)       */
#define I2C_ISR_ADDCODE (0x7FU << 17U) /*!< Address match code (slave)    */

/********************************************************************************
 * ICR bit definitions (STM32L4 Interrupt Clear Register)
 ********************************************************************************/
#define I2C_ICR_ADDRCF (1U << 3U)    /*!< Address matched flag clear       */
#define I2C_ICR_NACKCF (1U << 4U)    /*!< NACK flag clear                  */
#define I2C_ICR_STOPCF (1U << 5U)    /*!< STOP detection flag clear        */
#define I2C_ICR_BERRCF (1U << 8U)    /*!< Bus error flag clear             */
#define I2C_ICR_ARLOCF (1U << 9U)    /*!< Arbitration lost flag clear      */
#define I2C_ICR_OVRCF (1U << 10U)    /*!< Overrun/Underrun flag clear      */
#define I2C_ICR_PECCF (1U << 11U)    /*!< PEC error flag clear             */
#define I2C_ICR_TIMOUTCF (1U << 12U) /*!< Timeout flag clear               */
#define I2C_ICR_ALERTCF (1U << 13U)  /*!< Alert flag clear                 */
                                     /********************************************************************************  \
                                      * Convenience constants                                                           \
                                      ********************************************************************************/ \
#define I2C_SPEED_STANDARD                                                                                              \
    (100000U)                    /*!< 100 kHz standard mode            */
#define I2C_SPEED_FAST (400000U) /*!< 400 kHz fast mode                */

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
