/*
 * Copyright (c), NXP Semiconductors
 * (C)NXP B.V. 2014-2018
 * All rights are reserved. Reproduction in whole or in part is prohibited without
 * the written consent of the copyright owner. NXP reserves the right to make
 * changes without notice at any time. NXP makes no warranty, expressed, implied or
 * statutory, including but not limited to any implied warranty of merchantability
 * or fitness for any particular purpose, or that the use will not infringe any
 * third party patent, copyright or trademark. NXP must not be liable for any loss
 * or damage arising from its use.
 */

// #include "chip.h"
#include <reg_hal.h>
#include "iocon_nss.h"

void Chip_IOCON_Init(NSS_IOCON_T *pIOCON)
{
    (void)pIOCON; /* suppress [-Wunused-parameter]: argument is only present for consistency. */
    NSS_SYSCON->SYSAHBCLKCTRL |= CLOCK_PERIPHERAL_IOCON & 0x1DBFFC;
    // Chip_Clock_Peripheral_EnableClock(CLOCK_PERIPHERAL_IOCON);
}

void Chip_IOCON_DeInit(NSS_IOCON_T *pIOCON)
{
    (void)pIOCON; /* suppress [-Wunused-parameter]: argument is only present for consistency. */
    NSS_SYSCON->SYSAHBCLKCTRL &= ~(CLOCK_PERIPHERAL_IOCON & 0x1DBFFC);
    // Chip_Clock_Peripheral_DisableClock(CLOCK_PERIPHERAL_IOCON);
}

void Chip_IOCON_SetPinConfig(NSS_IOCON_T *pIOCON, IOCON_PIN_T pin, int config)
{
    if (pin >= IOCON_ANA0_0) /* Analog Pins */
    {
        if (config == IOCON_FUNC_1) /* Indicates connection to analog bus */
        {
            Chip_IOCON_UngroundAnabus(pIOCON, (1 << (pin - IOCON_ANA0_0)));
        }
    }
    pIOCON->REG[pin] = config & 0xFFFFFF;
}

int Chip_IOCON_GetPinConfig(NSS_IOCON_T *pIOCON, IOCON_PIN_T pin)
{
    return pIOCON->REG[pin] & 0xFFFFFF;
}

void Chip_IOCON_SetAnabusGrounded(NSS_IOCON_T *pIOCON, IOCON_ANABUS_T bitvector)
{
    pIOCON->ANABUSGROUND = bitvector & 0x0FFFFFFF;
}

IOCON_ANABUS_T Chip_IOCON_GetAnabusGrounded(NSS_IOCON_T *pIOCON)
{
    return (IOCON_ANABUS_T)(pIOCON->ANABUSGROUND & 0x0FFFFFFF);
}

void Chip_IOCON_GroundAnabus(NSS_IOCON_T *pIOCON, IOCON_ANABUS_T bitvector)
{
    pIOCON->ANABUSGROUND |= bitvector & 0x0FFFFFFF;
}

void Chip_IOCON_UngroundAnabus(NSS_IOCON_T *pIOCON, IOCON_ANABUS_T bitvector)
{
    pIOCON->ANABUSGROUND &= ~(bitvector & 0x0FFFFFFF);
}
