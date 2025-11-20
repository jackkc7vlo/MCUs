/**
 *
 *
 * @file crc.c
 * @brief calculate CRC
 *
 * @details Much of the communication of data to and from the RFID or NFC interfaces are checked with a
 *          16 bit crc value calculated in these routines
 *
 * @author Jack Wilson
 * @version 0.1
 * @date 11/01/2018
 * @copyright Copyright 2004 - 2019 Infratab Incorporated. All Rights Reserved.
 * * NOTICE:  All information contained herein is, and remains
 * the property of Infratab Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Infratab Incorporated
 * and its suppliers and may be covered by U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Infratab Incorporated.
 *
 */

#include <stdint.h>
#include <crc.h>

#define CRC_POLY (0x1021u)

/**
 * @brief Add bytes to the crc calculation
 *
 * @param *pD  Pointer to bytes to add
 * @param n  Number of bytes to use in the calculation
 * @param crc - CRC value to update
 *
 * @returns The updated CRC value
 *
 */

uint_fast16_t crc_update(const uint8_t *pD, uint_fast16_t n, uint_fast16_t crc)
{
    while (n--)
    {
        // OE: Matches C# version
        crc = crc ^ (*pD++ << 8);
        int32_t bits = 8;
        do
        {
            const uint_fast16_t prev = crc;
            crc = (crc << 1);
            if (prev & 0x8000u)
            {
                crc = (crc ^ CRC_POLY);
            }
        } while (--bits > 0);
    }
    // uint_fast16_t may be larger than 16 bits;
    // make sure we only return 16 bits.
    return crc & 0xFFFFu;
}

/**
 * @brief Calculates the 16-bit crc on a section of memory
 *
 * @param *pD  Pointer to bytes to add
 * @param n  Number of bytes to use in the calculation
 *
 * @returns The updated CRC value
 *
 */
uint_fast16_t crc_compute(const uint8_t *const pD, const uint_fast16_t n)
{
    return crc_update(pD, n, crc_init());
}
