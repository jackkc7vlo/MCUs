/*****************************************************************************
 *                                                                           *
 *  uCUnit - A unit testing framework for microcontrollers                   *
 *                                                                           *
 *  (C) 2007 - 2008 Sven Stefan Krauss                                       *
 *                  https://www.ucunit.org                                   *
 *                                                                           *
 *  File        : System.c                                                   *
 *  Description : System dependent functions used by uCUnit.                 *
 *                This file runs with arm-elf-run                            *
 *  Author      : Sven Stefan Krauss                                         *
 *  Contact     : www.ucunit.org                                             *
 *                                                                           *
 *****************************************************************************/

/*
 * This file is part of ucUnit.
 *
 * You can redistribute and/or modify it under the terms of the
 * Common Public License as published by IBM Corporation; either
 * version 1.0 of the License, or (at your option) any later version.
 *
 * uCUnit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Common Public License for more details.
 *
 * You should have received a copy of the Common Public License
 * along with uCUnit.
 *
 * It may also be available at the following URL:
 *       http://www.opensource.org/licenses/cpl1.0.txt
 *
 * If you cannot obtain a copy of the License, please contact the
 * author.
 */
#include <hw_config.h>
#include <stdio.h>
#include <stdlib.h> /*lint -e129*/
#include "System.h"
#if USE_SEGGER == 1
#include <SEGGER_RTT.h>
#endif

/* Stub: Initialize your hardware here */
void System_Init(void)
{

// switch to Channel 0 - virtual terminal 1
#if USE_SEGGER == 1
	SEGGER_RTT_SetTerminal(2);
	SEGGER_RTT_printf(0, "Init of hardware finished.\n");
	// back to Channel 0 - virtual terminal 0
	SEGGER_RTT_SetTerminal(0);
#endif
}

/* Stub: Shutdown your hardware here */
void System_Shutdown(void)
{

#if USE_SEGGER == 1
	SEGGER_RTT_SetTerminal(2);
	SEGGER_RTT_printf(0, "System shutdown. - not implemented\n");
	// back to Channel 0 - virtual terminal 0
	SEGGER_RTT_SetTerminal(0);
#endif
	/* asm("\tSTOP"); */
	exit(0);
}

/* Stub: Recover the system */
void System_Recover(void)
{
	/* Stub: Recover the hardware */
	/* asm("\tRESET"); */
#if USE_SEGGER == 1
	SEGGER_RTT_SetTerminal(2);
	SEGGER_RTT_printf(0, "System reset.- not implemented\n");
	// back to Channel 0 - virtual terminal 0
	SEGGER_RTT_SetTerminal(0);
#endif

	exit(0);
}

/* Stub: Put system in a safe state */
void System_Safestate(void)
{
/* Disable all port pins */
/* PORTA = 0x0000; */
/* PORTB = 0x0000; */
/* PORTC = 0x0000; */

/* Disable interrupts */
/* DIE(); */

/* Put processor into idle state */
/* asm("\tIDLE"); */
#if USE_SEGGER == 1
	SEGGER_RTT_SetTerminal(2);
	SEGGER_RTT_printf(0, "System safe state - not implemented.\n");
	// back to Channel 0 - virtual terminal 0
	SEGGER_RTT_SetTerminal(0);
#endif

	exit(0);
}

/* Stub: Transmit a string to the host/debugger/simulator */
void System_WriteString(char *msg)
{
#if USE_SEGGER == 1
	SEGGER_RTT_SetTerminal(2);
	SEGGER_RTT_printf(0, msg);
	// back to Channel 0 - virtual terminal 0
	SEGGER_RTT_SetTerminal(0);
#endif
}

void System_WriteInt(int n)
{
#if USE_SEGGER == 1
	SEGGER_RTT_SetTerminal(2);
	SEGGER_RTT_printf(0, "%ld", n);
	// back to Channel 0 - virtual terminal 0
	SEGGER_RTT_SetTerminal(0);
#endif
}
