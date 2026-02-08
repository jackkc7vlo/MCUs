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
 * @file      device_syscon.h
 * @defgroup  device_syscon System Configuration Controller (SYSCON)
 * @author    Jack Wilson
 * @brief     Defines and Macros for the System Configuration Controller (SYSCON)
 *
 * @details   System Configuration Controller (SYSCON)
 * This describes the SYSCON module, which is identical on all members of the
 * NHS31xx family.
 * The system configuration block is at APB 0x4004 8000. It controls oscillators, start logic,
 * and clock generation of the NHS31xx. It contains registers controlling power on/off of the
 * peripherals and a register to remap the ARM vector table to a selectable location in Flash
 * or SRAM.
 *
 * @mod
 * @moditem{V1.0.x, Nov-11-2025, Jack Wilson, Initial Release, jackkc7vlo@gmail.com }
 * @modend
 *
 */

#include <stdint.h>

#ifndef DEVICE_SYSCON_H
#define DEVICE_SYSCON_H
// lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
// lint -restore
// lint -esym(751,syscon_*) There are definitions they may not be used
// lint -esym(754,syscon_*) There are definitions they may not be used

// System Control Base Address
#define SYSCON_BASE_ADDR 0x40048000U

/**
 * @brief System Control registers
 */
typedef struct
{
    volatile uint32_t sysmemremap; //!< System memory remap register             offset 0x000
    volatile uint32_t presetctrl;  //!< Peripheral reset control register        offset 0x004
    volatile uint32_t unused1[6]; //!< Reserved                                 offset 0x008 - 0x01C
    volatile uint32_t sysclkctrl; //!< System clock control register            offset 0x020
    volatile uint32_t sysclkuen;  //!< System clock update enable register      offset 0x024
    volatile uint32_t
        reserved1[2];             //!< Reserved                                 offset 0x028 - 0x02C
    volatile uint32_t sysrststat; //!< System reset status register             offset 0x030
    volatile uint32_t
        reserved2[19];            //!< Reserved                                 offset 0x034 - 0x07C
    volatile uint32_t ahbclkctrl; //!< AHB clock control register               offset 0x080
    volatile uint32_t
        reserved3[4];            //!< Reserved                                 offset 0x084 - 0x094
    volatile uint32_t sspclkdiv; //!< SSP/SSI clock divider register           offset 0x094
    volatile uint32_t
        reserved4[14];           //!< Reserved                                 offset 0x098 - 0x0D0
    volatile uint32_t wdtclksel; //!< WDT clock select register                offset 0x0D0
    volatile uint32_t wdtclkuen; //!< WDT clock update enable register         offset 0x0D4
    volatile uint32_t wdtclkdiv; //!< WDT clock divider register               offset 0x0D8
    volatile uint32_t
        reserved5[3];           //!< Reserved                                 offset 0x0DC - 0x0F4
    volatile uint32_t clkouten; //!< CLKOUT enable register                   offset 0x0E8
    volatile uint32_t
        reserved6[26];            //!< Reserved                                 offset 0x0EC - 0x154
    volatile uint32_t systickcal; //!< SysTick calibration register             offset 0x154
    volatile uint32_t
        reserved7[42];            //!< Reserved                                 offset 0x158 - 0x200
    volatile uint32_t startaprp0; //!< Start logic edge control register 0      offset 0x200
    volatile uint32_t starterp0;  //!< start logic signal enable register 0     offset 0x204
    volatile uint32_t startsrpoclr; //!< start logic reset register 0             offset 0x208
    volatile uint32_t startsrp0;    //!< start logic status register 0            offset 0x20C
    volatile uint32_t
        reserved8[9];             //!< Reserved                                 offset 0x210 - 0X230
    volatile uint32_t pdawakecfg; //!< Power-down wake-up configuration register offset 0x234
    volatile uint32_t pdruncfg;   //!< Power-down run configuration register     offset 0x238

} reg_syscon_t;

static reg_syscon_t *p_syscon = (reg_syscon_t *const)SYSCON_BASE_ADDR;

/**
 * @brief System memory remap register (SYSMEMREMAP)
 *
 * @details The system memory remap register contains the addresses of the ARM interrupt vectors.
 * Valid addresses are on 1024 byte boundaries in Flash or SRAM.
 *
 */
#define SYSCON_SYSMEMREMAP_MAP_BIT (1U)       //!< interrupt vector remap 0=Flash, 1=SRAM
#define SYSCON_SYSMEMREMAP_OFFSETMASK (0x3EU) //!< offset for SRAM remap

/**
 * @brief Peripheral reset control register (PRESETCTRL)
 *
 * @details This register allows software to reset some peripherals. Writing a logic 0 to the bits
 * resets the corresponding peripheral. Writing a logic 1 de-asserts the reset. Remark: Before
 * accessing the SPI and I2C peripherals, write a logic 1 to this register. This action ensures that
 * the reset signals to the SPI and I2C are de-asserted.
 */
#define SYSCON_PRESETCTRL_SSP_BIT (1U)       //!< Bit 0 SPI/SSP reset control bit
#define SYSCON_PRESETCTRL_I2C_BIT (1U << 1U) //!< Bit 1 I2C reset control bit
#define SYSCON_PRESETCTRL_EE_BIT (1U << 2U)  //!< Bit 2 EEPROM reset control bit
#define SYSCON_PRESETCTRL_NFC_BIT (1U << 3U) //!< Bit 3 MFC reset control bit

/**
 * @brief System clock control register (SYSCLKCTRL)
 *
 * @details The system clock control register controls the analog part of the internal 8 MHz
 * oscillator as well as the system clock divider. For changes to the system clock divider to take
 * effect, first write a logic 0 to the SYSCLKUEN register, then write a logic 1 to SYSCLKUEN. See
 * Section 4.5.3 "System clock divider" for more information.
 */
#define SYSCON_SYSCLKDIV_MASK (0x0000000EU) //!< Mask for the system clock divider value (bits 1-3)
#define SYSCON_SYSCLKDIV_SHIFT (1U)         //!< Shift for the system clock divider value
#define SYSCON_SYSCLKDIV_VALUE(x) (((x) & SYSCON_SYSCLKDIV_MASK) >> SYSCON_SYSCLKDIV_SHIFT)
#define SYSCON_SYSCLKDIV_NO_DIV                                                                    \
    (0U << SYSCON_SYSCLKDIV_SHIFT) //!< System clock divider: no division (8 Mhz)
#define SYSCON_SYSCLKDIV_DIV_2                                                                     \
    (1U << SYSCON_SYSCLKDIV_SHIFT) //!< System clock divider: divide by 2 (4 MHz)
#define SYSCON_SYSCLKDIV_DIV_4                                                                     \
    (2U << SYSCON_SYSCLKDIV_SHIFT) //!< System clock divider: divide by 4 (2 MHz)
#define SYSCON_SYSCLKDIV_DIV_8                                                                     \
    (3U << SYSCON_SYSCLKDIV_SHIFT) //!< System clock divider: divide by 8 (1 MHz)
#define SYSCON_SYSCLKDIV_DIV_16                                                                    \
    (4U << SYSCON_SYSCLKDIV_SHIFT) //!< System clock divider: divide by 16 (500 kHz)
#define SYSCON_SYSCLKDIV_DIV_32                                                                    \
    (5U << SYSCON_SYSCLKDIV_SHIFT) //!< System clock divider: divide by 32 (250 kHz)
#define SYSCON_SYSCLKDIV_DIV_64                                                                    \
    (6U << SYSCON_SYSCLKDIV_SHIFT) //!< System clock divider: divide by 64 (125 kHz)
#define SYSCON_SYSCLKDIV_DIV_128                                                                   \
    (7U << SYSCON_SYSCLKDIV_SHIFT) //!< System clock divider: divide by 128 (62.5 kHz)

/**
 * @brief System clock update enable register (SYSCLKUEN)
 * @details This register updates the clock source of the system with the new input clock after the
 * SYSCLKCTRL register has been written to. For the update to take effect, first write a
 * logic 0 to the SYSCLKUEN register and then write a logic 1 to SYSCLKUEN.
 */
#define SYSCON_SYSCLKUEN_ENABLE_BIT (1U) //!< Bit 0 ENABLE: System clock update enable bit

/**
 * @brief System reset status register (SYSRSTSTAT)
 *
 * @details The system reset status register indicates the source of the last system reset.
 * Writing a logic 1 to any of the bits clears the corresponding reset source flag.
 * Writing a logic 0 has no effect.
 */
#define SYSCON_SYSRSTSTAT_POR_BIT (1U)          //!< Bit 0 POR: POR reset status bit
#define SYSCON_SYSRSTSTAT_EXTRST_BIT (1U << 1U) //!< Bit 1 EXTRST: External reset status bit
#define SYSCON_SYSRSTSTAT_WDT_BIT (1U << 2U)    //!< Bit 2 WDT: WDT reset status bit
#define SYSCON_SYSRSTSTAT_SYSRST_BIT (1U << 3U) //!< Bit 3 SYSRST: Software reset status bit

/**
 * @brief AHB clock control register (AHBCLKCTRL)
 *
 * @details The SYSAHBCLKCTRL register enables the clocks to individual system and peripheral
 * blocks. The system clock provides the clock for the AHB to APB bridge, the AHB matrix,
 * the ARM Cortex-M0+, the SYSCON block, and the PMU. This clock cannot be disabled.
 */

#define SYSCON_AHBCLKCTRL_BITS_THAT_CAN_BE_SET (0x1DBFFC) //!< bits that can be set in AHBCLKCTRL
#define SYSCON_AHBCLKCTRL_
#define SYSCON_AHBCLKCTRL_RAM_BIT (1U << 2U)     //!< Bit 2 RAM clock enable bit
#define SYSCON_AHBCLKCTRL_FLASH_MASK (0x18U)     //!< Bit 3:4 FLASH clock enable bit
#define SYSCON_AHBCLKCTRL_FLASH_SHIFT (1U << 3U) //!< Bit 3:4 FLASH shift
#define SYSCON_AHBCLKCTRL_FLASH_VALUE(x)                                                           \
    (((x) & SYSCON_AHBCLKCTRL_FLASH_MASK) >> SYSCON_AHBCLKCTRL_FLASH_SHIFT)
#define SYSCON_AHBCLKCTRL_FLASH_DISABLE (0U << SYSCON_SYSCLKDIV_SHIFT) //!< Disable flash clock
#define SYSCON_AHBCLKCTRL_FLASH_ENABLE1 (1U << SYSCON_SYSCLKDIV_SHIFT) //!< Enable flash clock
#define SYSCON_AHBCLKCTRL_FLASH_ENABLE2 (2U << SYSCON_SYSCLKDIV_SHIFT) //!< Enable flash clock
#define SYSCON_AHBCLKCTRL_FLASH_ENABLE3 (3U << SYSCON_SYSCLKDIV_SHIFT) //!< Enable flash clock

#define SYSCON_AHBCLKCTRL_I2C_BIT (1U << 5U)     //!< Bit 5 I2C clock enable bit
#define SYSCON_AHBCLKCTRL_GPIO_BIT (1U << 6U)    //!< Bit 6 GPIO clock enable bit
#define SYSCON_AHBCLKCTRL_SSISSP_BIT (1U << 7U)  //!< Bit 7 SSI/SSP clock enable bit
#define SYSCON_AHBCLKCTRL_CT16B_BIT (1U << 8U)   //!< Bit 8 CT16B clock enable bit
#define SYSCON_AHBCLKCTRL_CT32B_BIT (1U << 9U)   //!< Bit 9 CT32B clock enable bit
#define SYSCON_AHBCLKCTRL_RTC_BIT (1U << 10U)    //!< Bit 10 RTC clock enable bit
#define SYSCON_AHBCLKCTRL_WDT_BIT (1U << 11U)    //!< Bit 11 WDT clock enable bit
#define SYSCON_AHBCLKCTRL_TSEN_BIT (1U << 12U)   //!< Bit 12 TEMP SENSOR clock enable bit
#define SYSCON_AHBCLKCTRL_C2D_BIT (1U << 13U)    //!< Bit 13 C2D clock enable bit
#define SYSCON_AHBCLKCTRL_I2D_BIT (1U << 15U)    //!< Bit 15 I2D clock enable bit
#define SYSCON_AHBCLKCTRL_ADCDAC_BIT (1U << 16U) //!< Bit 16 ADCDAC clock enable bit
#define SYSCON_AHBCLKCTRL_IOCON_BIT (1U << 18U)  //!< Bit 18 IOCON clock enable bit
#define SYSCON_AHBCLKCTRL_EEREG_MASK (0x180000U) //!< Bit 19:20 FLASH clock enable bit
#define SYSCON_AHBCLKCTRL_EEREG_SHIFT (1U << 3U) //!< Bit 3:4 FLASH shift
#define SYSCON_AHBCLKCTRL_EEREG_VALUE(x)                                                           \
    (((x) & SYSCON_AHBCLKCTRL_EEREG_MASK) >> SYSCON_AHBCLKCTRL_EEREG_SHIFT)
#define SYSCON_AHBCLKCTRL_EEREG_DISABLE                                                            \
    (0U << SYSCON_AHBCLKCTRL_EEREG_SHIFT) //!< Disable eeprom clock
#define SYSCON_AHBCLKCTRL_EEREG_ENABLE1                                                            \
    (1U << SYSCON_AHBCLKCTRL_EEREG_SHIFT) //!< Enable eeprom clock
#define SYSCON_AHBCLKCTRL_EEREG_ENABLE2                                                            \
    (2U << SYSCON_AHBCLKCTRL_EEREG_SHIFT) //!< Enable eeprom clock
#define SYSCON_AHBCLKCTRL_EEREG_ENABLE3                                                            \
    (3U << SYSCON_AHBCLKCTRL_EEREG_SHIFT) //!< Enable eeprom clock

/**
 * @brief SSP/SSI clock divider register (SSPCLKDIV)
 *
 * @details This register configures the SPI peripheral clock SPI_PCLK. The SPI_PCLK can be shut
 * down by setting the DIV bits to logic 0.
 */
#define SYSCON_SSPCLKDIV_DIV_MASK (0xFFU) //!< Mask for the DIV value (bits 0-7)

/**
 * @brief WDT clock select register (WDTCLKSEL)
 *
 * @details This register selects the clock source for the watchdog timer. The WDTCLKUEN register
 * must be toggled from logic 0 to logic 1 for the update to take effect.
 */
#define SYSCON_WDTCLKSEL_FRO (0U)     //!< Internal RC oscillator selected as WDT clock
#define SYSCON_WDTCLKSEL_DISABLE (2U) //!< Disable WDT clock

/**
 * @brief WDT clock update enable register (WDTCLKUEN)
 *
 * @details This register updates the clock source of the watchdog timer with the new input source
 * after the WDTCLKSEL register has been written to. In order for the update to take effect at
 * the input of the watchdog timer, the following procedure must be applied:
 *  1. Write a logic 0 to the WDTCLKUEN register
 *  2. Write a logic 1 to WDTCLKUEN to preset the value
 *  3. Write a logic 0 again to apply the value
 * The 0-1-0 pulse length should be longer than the period of the actual and selected clock
 * sources
 */
/**
 * @brief WDT clock divider register (WDTCLKDIV)
 *
 * @details This register determines the divider values for the watchdog clock wdt_clk. For the
 * update to take effect at the input of the watchdog timer, a 1-0-1 sequence has to be written to
 * register WDTCLKEN. See Section 4.5.9
 * When the division factor is set to 0, the WDT_CLK is disabled. Apart from 1, only even
 * division factors can be used.
 */

/**
 * @brief CLKOUT enable register (CLKOUTEN)
 *
 * @details This register enables the CLKOUT function on the CLKOUT pin. The CLKOUT function
 * outputs a clock signal derived from the system clock.
 */
#define SYSCON_CLKOUTEN_ENABLE_BIT (1U) //!< Bit 0 ENABLE: CLKOUT enable bit

#define SYSCON_CLKOUTEN_CLKOUTSRC_SHIFT (1U << 1U) //!< Bit 1:2 CLKOUT source shift
#define SYSCON_CLKOUTEN_CLKOUTSRC_SFRO (0U)        //!< SFRO clock selected as CLKOUT source
#define SYSCON_CLKOUTEN_CLKOUTSRC_SYSCLK (1U)      //!< System clock selected as CLKOUT source
#define SYSCON_CLKOUTEN_CLKOUTSRC_FRO (2U)         //!< FRO clock selected as CLKOUT source
#define SYSCON_CLKOUTEN_CLKOUTSRC_NFC (3U)         //!< NFC clock selected as CLKOUT source

/**
 * @brief SysTick calibration register (SYSTICKCAL)
 *
 * @details The SysTick calibration value register provides a calibration value for the SysTick
 * timer. This value represents the number of clock cycles for 10 ms, assuming that the
 * system clock is running at the frequency intended for use with the CALIB value.
 */

/**
 * @brief Start logic registers (STARTAPRP0, STARTERP0, STARTRSRP0CLR, STARTSRP0)
 *
 * @details The start logic registers control the start logic functions that can generate
 * wake-up events from power-down mode. There are four start logic units that can be
 * configured to respond to various pin events or to the RTC alarm.
 */
/**
 * @brief Power-down configuration registers (PDAWAKECFG, PDRUNCFG)
 *
 * @details The power-down configuration registers control the behavior of the device
 * when entering and exiting power-down mode.
 */

/**
 * @brief Device ID register (DEVICEID)
 *
 * @details The device ID register contains a factory-programmed device identifier.
 * This identifier is unique for each device variant.
 */

#endif // DEVICE_SYSCON_H