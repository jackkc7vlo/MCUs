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
 * @file device_regs.h
 * @brief Generic device register definition.
 * @details Outlines the portable device register abstraction used to configure and interact with hardware peripherals
 * on any supported platform.
 *
 * @author  Jack Wilson
 * @date    September 26, 2025
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Jack Wilson
 * @license MIT License
 */

#ifndef DEVICE_REGS_H
#define DEVICE_REGS_H
#include <stdint.h> //lint !e537

// lint -esym(528,*) There are definitions they may not be used
// lint -esym(750,*) There are definitions they may not be used
// lint -esym(751,*) There are definitions they may not be used
// lint -esym(754,*) There are definitions they may not be used
// lint -esym(749,*) There are definitions they may not be used

// NOTE:  This does not define every possible register or bit
// More can be added from the datasheet
#define VAR_UNUSED(X) (void)(X) // lint !e9026 To avoid gcc/g++ warnings */

// Registry Values come from the data sheet
// Peripheral Base  0x40000000
#define REG_PERIPH_BASE 0x40000000U
// AHB1 Offset from Peripheral Base 0x00020000
#define REG_APB1PERIPH_BASE REG_PERIPH_BASE
#define REG_APB2PERIPH_BASE (REG_PERIPH_BASE + 0x00010000UL)
#define REG_AHB1PERIPH_BASE (REG_PERIPH_BASE + 0x00020000UL)
#define REG_AHB2PERIPH_BASE (REG_PERIPH_BASE + 0x08000000UL)

#endif /* DEVICE_REGS_H */
