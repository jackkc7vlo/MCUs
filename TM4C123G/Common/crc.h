/**
 * @file crc.h
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

#ifndef CRC_H_
#define CRC_H_
#include <stdint.h>
#define CRC_INIT (0xffffu)

/**
 * @brief Initialize the CRC function
 *
 * @returns The initial CRC value
 *
 */

static inline uint_fast16_t crc_init() { return CRC_INIT; }

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

uint_fast16_t crc_update(const uint8_t *pD, uint_fast16_t n, uint_fast16_t crc);

/**
 * @brief Calculates the 16-bit crc on a section of memory
 *
 * @param *pD  Pointer to bytes to add
 * @param n  Number of bytes to use in the calculation
 *
 * @returns The updated CRC value
 *
 */
uint_fast16_t crc_compute(const uint8_t *const pD, const uint_fast16_t n);

#endif /* CRC_H_ */