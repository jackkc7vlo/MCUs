#ifndef EM_GPCRC_H
#define EM_GPCRC_H

#include <stdint.h>
#include <stdbool.h>
#include <reg_hal.h>
#include <hw_config.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SOFT_CRC_INIT (0xffffu)
  /***************************************************************************/
  /***************************************************************************/
  /*******************************************************************************
   *******************************   STRUCTS   ***********************************
   ******************************************************************************/

#if USE_HW_CRC == 1u

  /*******************************************************************************
   ******************************   PROTOTYPES   *********************************
   ******************************************************************************/

  void crc_hal_clk_enable(void);
  void crc_hal_hw_init();
  void crc_hal_hw_reset(void);

  /***************************************************************************/
  void crc_hal_hw_enable(const bool enable);

  /***************************************************************************/

  void crc_hal_hw_start(void);

  /*
  __STATIC_INLINE void GPCRC_Start(GPCRC_TypeDef *gpcrc)
  {
    gpcrc->CMD = GPCRC_CMD_INIT;
  }
*/

  void crc_hal_hw_initial_value(const uint32_t initValue);

  /***************************************************************************/
  /*
   __STATIC_INLINE void GPCRC_InitValueSet(GPCRC_TypeDef *gpcrc, uint32_t initValue)
   {
     gpcrc->INIT = initValue;
   }
 */

  void crc_hal_hw_add_uint32(const uint32_t data);
  /***************************************************************************/

  /*
  __STATIC_INLINE void GPCRC_InputU32(GPCRC_TypeDef *gpcrc, uint32_t data)
  {
    gpcrc->INPUTDATA = data;
  }
  */

  void crc_hal_hw_add_uint16(const uint16_t data);
  /***************************************************************************/

  /*
  __STATIC_INLINE void GPCRC_InputU16(GPCRC_TypeDef *gpcrc, uint16_t data)
  {
    gpcrc->INPUTDATAHWORD = data;
  }
  */

  void crc_hal_hw_add_uint8(const uint8_t data);

  /***************************************************************************/
  /*
  __STATIC_INLINE void GPCRC_InputU8(GPCRC_TypeDef *gpcrc, uint8_t data)
  {
    gpcrc->INPUTDATABYTE = data;
  }
*/

  uint32_t crc_hal_hw_get_result(void);

  uint32_t crc_hal_hw_get_rev_result(void);

  /***************************************************************************/
  /*
  __STATIC_INLINE uint32_t GPCRC_DataRead(GPCRC_TypeDef *gpcrc)
  {
    return gpcrc->DATA;
  }
  */
#endif

  /***************************************************************************/
  static inline uint_fast16_t soft_crc_init()
  {
    return SOFT_CRC_INIT;
  }

  uint_fast16_t soft_crc_update(const uint8_t *pD, uint_fast16_t n, uint_fast16_t crc);

  /**
   * @brief Calculates the 16-bit crc on a section of memory
   *
   * @param *pD  Pointer to bytes to add
   * @param n  Number of bytes to use in the calculation
   *
   * @returns The updated CRC value
   *
   */
  uint_fast16_t soft_crc_compute(const uint8_t *const pD, const uint_fast16_t n);
#if UNIT_TESTS == 1u
  void crc_hal_unit_test(void);
#endif
#ifdef __cplusplus
}
#endif

#endif /* EM_GPCRC_H */