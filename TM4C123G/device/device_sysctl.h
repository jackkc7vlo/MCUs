/**
 * @file sysctl.h
 * @author Jack Wilon
 * @brief
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 *
 * System configuration controller (SYSCTL) definitions for the TI TM4C123GH6PM microcontroller.
 * This file provides register definitions and bit masks for configuring the system control
 * features of the microcontroller, including clock control, reset control, and power management.
 */

#include <stdint.h>

#ifndef SYSCTL_H
#define SYSCTL_H
// lint -save -esym(750,*) -esym(7,__*) -esym(451, std*)
// lint -restore
// lint -esym(751,syscon_*) There are definitions they may not be used
// lint -esym(754,syscon_*) There are definitions they may not be used

// System Control Base Address
#define SYSCTL_BASE_ADDR 0x400FE000U

/**
 * @brief System Control registers
 */
typedef struct
{
    volatile uint32_t did0;          //!< Device identification register 0         offset 0x000
    volatile uint32_t did1;          //!< Device identification register 1         offset 0x004
    volatile uint32_t unused1[10];   //!< Reserved                                 offset 0x008 - 0x02C
    volatile uint32_t pborctl;       //!< Brown-Out Reset Control register         offset 0x030
    volatile uint32_t unused2[7];    //!< Reserved                                 offset 0x034 - 0x04C
    volatile uint32_t ris;           //!< Raw Interrupt Status                     offset 0x050
    volatile uint32_t imc;           //!< Interrupt Mask Control                   offset 0x054
    volatile uint32_t misc;          //!< Masked Interrupt Status and Clear        offset 0x058
    volatile uint32_t resc;          //!< Reset Cause                              offset 0x05C
    volatile uint32_t rcc;           //!< Run-Mode Clock Configuration             offset 0x060
    volatile uint32_t unused3[2];    //!< Reserved                                 offset 0x064 - 0x068
    volatile uint32_t gpiohbctl;     //!< GPIO High-Performance Bus Control        offset 0x06C
    volatile uint32_t rcc2;          //!< Run-Mode Clock Configuration 2           offset 0x070
    volatile uint32_t unused4[2];    //!< Reserved                                 offset 0x074 - 0x078
    volatile uint32_t moscctl;       //!< Main Oscillator Control                  offset 0x07C
    volatile uint32_t unused5[49];   //!< Reserved                                 offset 0x080 - 0x140
    volatile uint32_t dslpclkcfg;    //!< Deep Sleep Clock Configuration           offset 0x144
    volatile uint32_t unused6;       //!< Reserved                                 offset 0x148
    volatile uint32_t sysprop;       //!< System Properties                        offset 0x14C
    volatile uint32_t piosccal;      //!< Precision Internal Oscillator Calibration offset 0x150
    volatile uint32_t pioscstat;     //!< Precision Internal Oscillator Statistics offset 0x154
    volatile uint32_t unused7[2];    //!< Reserved                                 offset 0x158 - 0x15C
    volatile uint32_t pllfreq0;      //!< PLL Frequency 0                          offset 0x160
    volatile uint32_t pllfreq1;      //!< PLL Frequency 1                          offset 0x164
    volatile uint32_t pllstat;       //!< PLL Status                               offset 0x168
    volatile uint32_t unused8[7];    //!< Reserved                                 offset 0x16C - 0x184
    volatile uint32_t slppwrccfg;    //!< Sleep Power Control Configuration        offset 0x188
    volatile uint32_t dslppwrccfg;   //!< Deep Sleep Power Control Configuration   offset 0x18C
    volatile uint32_t unused9[9];    //!< Reserved                                 offset 0x190 - 0x1B0
    volatile uint32_t ldospctl;      //!< LDO Sleep Power Control                  offset 0x1B4
    volatile uint32_t ldospcal;      //!< LDO Sleep Power Calibration              offset 0x1B8
    volatile uint32_t ldodpctl;      //!< LDO Deep Sleep Power Control             offset 0x1BC
    volatile uint32_t ldodpcal;      //!< LDO Deep Sleep Power Calibration         offset 0x1C0
    volatile uint32_t unused10[2];   //!< Reserved                                 offset 0x1C4 - 0x1C8
    volatile uint32_t sdpmst;        //!< SDPM Status                              offset 0x1CC
    volatile uint32_t unused11[76];  //!< Reserved                                 offset 0x1D0 - 0x2FC
    volatile uint32_t ppwd;          //!< Watchdog Timer Peripheral Present        offset 0x300
    volatile uint32_t pptimer;       //!< Timer Peripheral Present                 offset 0x304
    volatile uint32_t ppgpio;        //!< GPIO Peripheral Present                  offset 0x308
    volatile uint32_t ppdma;         //!< DMA Peripheral Present                   offset 0x30C
    volatile uint32_t unused12;      //!< Reserved                                 offset 0x310
    volatile uint32_t pphib;         //!< Hibernation Peripheral Present           offset 0x314
    volatile uint32_t ppuart;        //!< UART Peripheral Present                  offset 0x318
    volatile uint32_t ppssi;         //!< SSI Peripheral Present                   offset 0x31C
    volatile uint32_t ppi2c;         //!< I2C Peripheral Present                   offset 0x320
    volatile uint32_t unused13;      //!< Reserved                                 offset 0x324
    volatile uint32_t ppusb;         //!< USB Peripheral Present                   offset 0x328
    volatile uint32_t unused14[2];   //!< Reserved                                 offset 0x32C - 0x330
    volatile uint32_t ppcan;         //!< CAN Peripheral Present                   offset 0x334
    volatile uint32_t ppadc;         //!< ADC Peripheral Present                   offset 0x338
    volatile uint32_t ppacmp;        //!< Analog Comparator Peripheral Present     offset 0x33C
    volatile uint32_t pppwm;         //!< Motor Control PWM Peripheral Present     offset 0x340
    volatile uint32_t ppqei;         //!< Quadrature Encoder Interface Peripheral Present offset 0x344
    volatile uint32_t unused15[4];   //!< Reserved                                 offset 0x348 - 0x354
    volatile uint32_t ppeeprom;      //!< EEPROM Peripheral Present                offset 0x358
    volatile uint32_t ppwtimer;      //!< Wide Timer Peripheral Present            offset 0x35C
    volatile uint32_t unused16[104]; //!< Reserved                                 offset 0x360 - 0x4FC
    volatile uint32_t srwd;          //!< Watchdog Timer Software Reset            offset 0x500
    volatile uint32_t srtimer;       //!< Timer Software Reset                     offset 0x504
    volatile uint32_t srgpio;        //!< GPIO Software Reset                      offset 0x508
    volatile uint32_t srdma;         //!< DMA Software Reset                       offset 0x50C
    volatile uint32_t unused17;      //!< Reserved                                 offset 0x510
    volatile uint32_t srhib;         //!< Hibernation Software Reset               offset 0x514
    volatile uint32_t sruart;        //!< UART Software Reset                      offset 0x518
    volatile uint32_t srssi;         //!< SSI Software Reset                       offset 0x51C
    volatile uint32_t sri2c;         //!< I2C Software Reset                       offset 0x520
    volatile uint32_t unused18;      //!< Reserved                                 offset 0x524
    volatile uint32_t srusb;         //!< USB Software Reset                       offset 0x528
    volatile uint32_t unused19[2];   //!< Reserved                                 offset 0x52C - 0x530
    volatile uint32_t srcan;         //!< CAN Software Reset                       offset 0x534
    volatile uint32_t sradc;         //!< ADC Software Reset                       offset 0x538
    volatile uint32_t sracmp;        //!< Analog Comparator Software Reset         offset 0x53C
    volatile uint32_t srpwm;         //!< Motor Control PWM Software Reset         offset 0x540
    volatile uint32_t srqei;         //!< Quadrature Encoder Interface Software Reset offset 0x544
    volatile uint32_t unused20[4];   //!< Reserved                                 offset 0x548 - 0x554
    volatile uint32_t sreeprom;      //!< EEPROM Software Reset                    offset 0x558
    volatile uint32_t srwtimer;      //!< Wide Timer Software Reset                offset 0x55C
    volatile uint32_t unused21[40];  //!< Reserved                                 offset 0x560 - 0x5FC
    volatile uint32_t rcgcwd;        //!< Watchdog Timer Run Mode Clock Gating Control offset 0x600
    volatile uint32_t rcgctimer;     //!< Timer Run Mode Clock Gating Control      offset 0x604
    volatile uint32_t rcgcgpio;      //!< GPIO Run Mode Clock Gating Control       offset 0x608
    volatile uint32_t rcgcdma;       //!< DMA Run Mode Clock Gating Control        offset 0x60C
    volatile uint32_t unused22;      //!< Reserved                                 offset 0x610
    volatile uint32_t rcgchib;       //!< Hibernation Run Mode Clock Gating Control  offset 0x614
    volatile uint32_t rcgcuart;      //!< UART Run Mode Clock Gating Control       offset 0x618
    volatile uint32_t rcgcssi;       //!< SSI Run Mode Clock Gating Control        offset 0x61C
    volatile uint32_t rcgci2c;       //!< I2C Run Mode Clock Gating Control        offset 0x620
    volatile uint32_t unused23;      //!< Reserved                                 offset 0x624
    volatile uint32_t rcgcusb;       //!< USB Run Mode Clock Gating Control        offset 0x628
    volatile uint32_t unused24[2];   //!< Reserved                                 offset 0x62C - 0x630
    volatile uint32_t rcgccan;       //!< CAN Run Mode Clock Gating Control         offset 0x634
    volatile uint32_t rcgcadc;       //!< ADC Run Mode Clock Gating Control        offset 0x638
    volatile uint32_t rcgcacmp;      //!< Analog Comparator Run Mode Clock Gating Control offset 0x63C
    volatile uint32_t rcgcpwm;       //!< Motor Control PWM Run Mode Clock Gating Control  offset 0x640
    volatile uint32_t rcgcqei;       //!< Quadrature Encoder Interface Run Mode Clock Gating Control offset 0x644

    // ... Additional registers can be added here as needed

} reg_sysctl_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static reg_sysctl_t *p_device_sysctl = (reg_sysctl_t *const)SYSCTL_BASE_ADDR;
#pragma GCC diagnostic pop
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
#define SYSCON_SYSCLKDIV_NO_DIV (0U << SYSCON_SYSCLKDIV_SHIFT)  //!< System clock divider: no division (8 Mhz)
#define SYSCON_SYSCLKDIV_DIV_2 (1U << SYSCON_SYSCLKDIV_SHIFT)   //!< System clock divider: divide by 2 (4 MHz)
#define SYSCON_SYSCLKDIV_DIV_4 (2U << SYSCON_SYSCLKDIV_SHIFT)   //!< System clock divider: divide by 4 (2 MHz)
#define SYSCON_SYSCLKDIV_DIV_8 (3U << SYSCON_SYSCLKDIV_SHIFT)   //!< System clock divider: divide by 8 (1 MHz)
#define SYSCON_SYSCLKDIV_DIV_16 (4U << SYSCON_SYSCLKDIV_SHIFT)  //!< System clock divider: divide by 16 (500 kHz)
#define SYSCON_SYSCLKDIV_DIV_32 (5U << SYSCON_SYSCLKDIV_SHIFT)  //!< System clock divider: divide by 32 (250 kHz)
#define SYSCON_SYSCLKDIV_DIV_64 (6U << SYSCON_SYSCLKDIV_SHIFT)  //!< System clock divider: divide by 64 (125 kHz)
#define SYSCON_SYSCLKDIV_DIV_128 (7U << SYSCON_SYSCLKDIV_SHIFT) //!< System clock divider: divide by 128 (62.5 kHz)

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
#define SYSCON_AHBCLKCTRL_FLASH_VALUE(x) (((x) & SYSCON_AHBCLKCTRL_FLASH_MASK) >> SYSCON_AHBCLKCTRL_FLASH_SHIFT)
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
#define SYSCON_AHBCLKCTRL_EEREG_VALUE(x) (((x) & SYSCON_AHBCLKCTRL_EEREG_MASK) >> SYSCON_AHBCLKCTRL_EEREG_SHIFT)
#define SYSCON_AHBCLKCTRL_EEREG_DISABLE (0U << SYSCON_AHBCLKCTRL_EEREG_SHIFT) //!< Disable eeprom clock
#define SYSCON_AHBCLKCTRL_EEREG_ENABLE1 (1U << SYSCON_AHBCLKCTRL_EEREG_SHIFT) //!< Enable eeprom clock
#define SYSCON_AHBCLKCTRL_EEREG_ENABLE2 (2U << SYSCON_AHBCLKCTRL_EEREG_SHIFT) //!< Enable eeprom clock
#define SYSCON_AHBCLKCTRL_EEREG_ENABLE3 (3U << SYSCON_AHBCLKCTRL_EEREG_SHIFT) //!< Enable eeprom clock

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

/**
 * @brief General-Purpose Input/Output Run Mode Clock Gating Control (RCGCGPIO)
 *
 */
#define SYSCON_RCGCGPIO_PORTA_BIT (1U << 0U) //!< Bit 0 PORTA clock enable bit
#define SYSCON_RCGCGPIO_PORTB_BIT (1U << 1U) //!< Bit 1 PORTB clock enable bit
#define SYSCON_RCGCGPIO_PORTC_BIT (1U << 2U) //!< Bit 2 PORTC clock enable bit
#define SYSCON_RCGCGPIO_PORTD_BIT (1U << 3U) //!< Bit 3 PORTD clock enable bit
#define SYSCON_RCGCGPIO_PORTE_BIT (1U << 4U) //!< Bit 4 PORTE clock enable bit
#define SYSCON_RCGCGPIO_PORTF_BIT (1U << 5U) //!< Bit 5 PORTF clock enable bit
#endif                                       // SYSCTL_H