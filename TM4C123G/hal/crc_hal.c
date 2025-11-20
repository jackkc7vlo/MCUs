
#include <crc_hal.h>
/***************************************************************************/
/***************************************************************************/
/*******************************************************************************
 ***************************   GLOBAL FUNCTIONS   ******************************
 ******************************************************************************/

/*lint -e750 -e9059 -e835*/
#if UNIT_TESTS == 1u
#include <unittest.h>
#endif

#define CRC_POLY (0x1021u)

uint_fast16_t soft_crc_update(const uint8_t *pD, uint_fast16_t n, uint_fast16_t crc)
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
uint_fast16_t soft_crc_compute(const uint8_t *const pD, const uint_fast16_t n)
{
    return soft_crc_update(pD, n, soft_crc_init());
}

#if UNIT_TESTS == 1u
void crc_hal_unit_test(void)
{
    unittest_start_test("crc_hal_unit_test");

    uint_fast16_t soft_checksum;
    const uint8_t data = (uint8_t)'A';
    uint8_t dataArray[100] = {0u};

    for (uint32_t i = 0u; i < 100u; i++)
    {
        dataArray[i] = (uint8_t)i;
    }

    unittest_start_testcase("crc16 software 1 byte        ");
    soft_checksum = soft_crc_update(&data, 1u, soft_crc_init());
    (void)unittest_check_equal((int32_t)soft_checksum, (int32_t)0xB915u);

    unittest_start_testcase("crc16 software 100 byte");
    soft_checksum = soft_crc_update(dataArray, 100u, soft_crc_init());
    (void)unittest_check_equal((int32_t)soft_checksum, (int32_t)0x44aa);

    /* The checksum is now 0xBF41 */
    // CMU_ClockEnable(cmuClock_HFPER, true);
    // CMU_ClockEnable(cmuClock_GPCRC, true);
}

#endif
