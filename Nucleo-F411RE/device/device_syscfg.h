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
 * @file device_syscfg.h
 * @brief  Defines registers used for System Configuration Controller (SYSCFG)
 *          on Arm devices
 * @details The system configuration controller is mainly used to remap the
 *          memory accessible in the code area and manage the external
 *          interrupt line connection to the GPIOs.
 *
 * @author  Jack Wilson
 * @date    January 23, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Jack Wilson
 * @license MIT License
 */

#ifndef DEVICE_SYSCFG_H
#define DEVICE_SYSCFG_H

#include "device_regs.h"
#include <stdint.h>

// lint -esym(528,*) There are definitions they may not be used
// lint -esym(750,*) There are definitions they may not be used
// lint -esym(751,*) There are definitions they may not be used
// lint -esym(754,*) There are definitions they may not be used
// lint -esym(749,*) There are definitions they may not be used

// NOTE:  This does not define every possible register or bit
// More can be added from the datasheet

// Reset Clock Control
#define REG_SYSCFG_BASE (REG_APB2PERIPH_BASE + 0x3800U)
/**
 * @brief  Sysconfig memory definition
 */
typedef struct
{
    volatile uint32_t memrmp;    /*!< SYSCFG memory remap register,               Address offset: 0x00 */
    volatile uint32_t pmc;       /*!< SYSCFG peripheral mode configuration register, Address offset: 0x04 */
    volatile uint32_t exticr[4]; /*!< SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 */
    uint32_t          reserved;  /*!< Reserved,                                  Address offset: 0x18 */
    uint32_t          reserved1; /*!< Reserved,                                  Address offset: 0x1C */
    volatile uint32_t cmpcr;     /*!< SYSCFG compensation cell control register, Address offset: 0x20 */

} reg_syscfg_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_syscfg_t *p_device_syscfg = (reg_syscfg_t *const)(REG_SYSCFG_BASE);
// create RCC_AHB1ENR register
// #define REG_RCC_AHB1ENR (*(volatile unsigned int *)(REG_RCC_BASE + 0x30U))
#pragma GCC diagnostic pop

#endif /* DEVICE_SYSCFG_H */