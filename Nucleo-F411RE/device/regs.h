/** @file regs.h
 * 
 * @brief defines registers used in the ARM mcu
 *
 *
 * @par
 * COPYWRITE NOTICE: (c) 2022 Jack Wilson.  All rights reserved
 *
 */

#ifndef REGS_H
#define REGS_H
#include <stdint.h> //lint !e537 

//lint -esym(528,*) There are definitions they may not be used
//lint -esym(750,*) There are definitions they may not be used
//lint -esym(751,*) There are definitions they may not be used
//lint -esym(754,*) There are definitions they may not be used 
//lint -esym(749,*) There are definitions they may not be used 

// NOTE:  This does not define every possible register or bit 
// More can be added from the datasheet
#define VAR_UNUSED(X) (void)(X)  //lint !e9026 To avoid gcc/g++ warnings */
	
// Registry Values come from the data sheet
// Peripheral Base  0x40000000
#define REG_PERIPH_BASE 0x40000000U
// AHB1 Offset from Peripheral Base 0x00020000
#define REG_APB1PERIPH_BASE       REG_PERIPH_BASE
#define REG_APB2PERIPH_BASE       (REG_PERIPH_BASE + 0x00010000UL)
#define REG_AHB1PERIPH_BASE 			(REG_PERIPH_BASE + 0x00020000U)
#define REG_AHB2PERIPH_BASE       (REG_PERIPH_BASE + 0x10000000UL)


static uint32_t reg_modify_bit(const uint32_t n, const uint32_t p, const uint32_t b)
{
	const uint32_t mask = 1U << p;
  return ((n & ~mask) | (b << p));
}

#endif /* REGS_H */
