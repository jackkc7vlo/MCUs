/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       reg_hal.h
 * @author     Jack Wilson
 * @brief
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-13-2023     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 *
 * ==============================================================================
 * @endverbatim
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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
 ****************************(C) COPYRIGHT 2023 Old Man Software *****************/

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef reg_hal_h_
#define reg_hal_h_

/********************************************************************************
 *  * Includes
 *  ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
// #include <stdint.h>
#include <cmsis.h>

/*lint -e768 -e769 -e755 -e756 -e714 -e754*/
// #include <core_cm0plus.h>

/********************************************************************************
 * Defines
 ********************************************************************************/
#define NSS_SFRO_FREQUENCY 8000000u /*!< System Free-Running Oscillator (SFRO) frequency in Hz */
#define NSS_TFRO_FREQUENCY 32768u   /*!< Timer Oscillator (TFRO) frequency in Hz */

    /* Base addresses of HW Peripherals */

#define NSS_I2C_BASE 0x40000000u       /*!< Base address of the I2C peripheral */
#define NSS_WWDT_BASE 0x40004000u      /*!< Base address of the Watchdog Timer peripheral */
#define NSS_TIMER16_0_BASE 0x4000C000u /*!< Base address of the Timer0 16-bit peripheral */
#define NSS_TIMER32_0_BASE 0x40014000u /*!< Base address of the Timer0 32-bit peripheral */
#define NSS_FLASH_BASE 0x4003C000u     /*!< Base address of the Flash memory controller */
#define NSS_EEPROM_BASE 0x40034000u    /*!< Base address of the EEPROM memory controller */
#define NSS_PMU_BASE 0x40038000u       /*!< Base address of the Power Management Unit */
#define NSS_SSP0_BASE 0x40040000u      /*!< Base address of the Synchronous Serial Port peripheral */
#define NSS_IOCON_BASE 0x40044000u     /*!< Base address of the I/O configuration */
#define NSS_SYSCON_BASE 0x40048000u    /*!< Base address of the System configuration */
#define NSS_RTC_BASE 0x40054000u       /*!< Base address of the Real-Time-Clock peripheral */
#define NSS_NFC_BASE 0x40058000u       /*!< Base address of the Near-Field Communication peripheral */
#define NSS_TSEN_BASE 0x40060000u      /*!< Base address of the Temperature sensor peripheral */
#define NSS_GPIO_BASE 0x50000000u      /*!< Base address of the General Purpose I/O peripheral */

    /* Handles to HW Peripherals */

#define NSS_I2C ((NSS_I2C_T *)NSS_I2C_BASE)               /*!< Handle for the I2C peripheral */
#define NSS_WWDT ((NSS_WWDT_T *)NSS_WWDT_BASE)            /*!< Handle for the Watchdog Timer peripheral */
#define NSS_TIMER16_0 ((NSS_TIMER_T *)NSS_TIMER16_0_BASE) /*!< Handle for the Timer0 16-bit peripheral */
#define NSS_TIMER32_0 ((NSS_TIMER_T *)NSS_TIMER32_0_BASE) /*!< Handle for the Timer0 32-bit peripheral */
#define NSS_FLASH ((NSS_FLASH_T *)NSS_FLASH_BASE)         /*!< Handle for the Flash memory controller */
#define NSS_EEPROM ((NSS_EEPROM_T *)NSS_EEPROM_BASE)      /*!< Handle for the EEPROM memory controller */
#define NSS_PMU ((NSS_PMU_T *)NSS_PMU_BASE)               /*!< Handle for the Power Management Unit */
#define NSS_SSP0 ((NSS_SSP_T *)NSS_SSP0_BASE)             /*!< Handle for the Synchronous Serial Port peripheral */
#define NSS_IOCON ((NSS_IOCON_T *)NSS_IOCON_BASE)         /*!< Handle for the I/O configuration */
#define NSS_SYSCON ((NSS_SYSCON_T *)NSS_SYSCON_BASE)      /*!< Handle for the System configuration */
#define NSS_RTC ((NSS_RTC_T *)NSS_RTC_BASE)               /*!< Handle for the Real-Time-Clock peripheral */
#define NSS_NFC ((NSS_NFC_T *)NSS_NFC_BASE)               /*!< Handle for the Near-Field Communication peripheral */
#define NSS_TSEN ((NSS_TSEN_T *)NSS_TSEN_BASE)            /*!< Handle for the Temperature sensor peripheral */
#define NSS_GPIO ((NSS_GPIO_T *)NSS_GPIO_BASE)            /*!< Handle for the General Purpose I/O peripheral */

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    /** Possible Peripheral clocks that can be enabled/disabled */
    typedef enum CLOCK_PERIPHERAL
    {
        CLOCK_PERIPHERAL_RAM = (1u << 2u),       /*!< Represents the RAM memory clock */
        CLOCK_PERIPHERAL_FLASH = (3u << 3u),     /*!< Represents the Flash memory clock*/
        CLOCK_PERIPHERAL_I2C0 = (1u << 5u),      /*!< Represents the I2C0 clock */
        CLOCK_PERIPHERAL_GPIO = (1u << 6u),      /*!< Represents the GPIO clock */
        CLOCK_PERIPHERAL_SPI0 = (1u << 7u),      /*!< Represents the SPI0 register access clock (The SPI0 block itself
                                                    is clocked by a separate clock) */
        CLOCK_PERIPHERAL_16TIMER0 = (1u << 8u),  /*!< Represents the 16 bit timer 0 clock */
        CLOCK_PERIPHERAL_32TIMER0 = (1u << 9u),  /*!< Represents the 32 bit timer 0 clock */
        CLOCK_PERIPHERAL_RTC = (1u << 10u),      /*!< Represents the RTC clock register access clock only (The RTC block
                                                    itself is clocked by a separate clock source) */
        CLOCK_PERIPHERAL_WATCHDOG = (1u << 11u), /*!< Represents the Watchdog register access clock only (The Watchdog
                                                    block itself is clocked by a separate clock) */
        CLOCK_PERIPHERAL_TSEN = (1u << 12u),     /*!< Represents the Temperature Sensor clock */
        CLOCK_PERIPHERAL_C2D = (1u << 13u),      /*!< Represents the Capacitance to Digital converter clock */
        CLOCK_PERIPHERAL_I2D = (1u << 15u),      /*!< Represents the Current to Digital converter clock */
        CLOCK_PERIPHERAL_ADCDAC = (1u << 16u),   /*!< Represents the Analog-to-Digital/Digital-to-Analog converter clock */
        CLOCK_PERIPHERAL_IOCON = (1u << 18u),    /*!< Represents the I/O configuration block clock */
        CLOCK_PERIPHERAL_EEPROM = (3u << 19u)    /*!< Represents the EEPROM memory clock */
    } CLOCK_PERIPHERAL_T;

    /**
     * System Configuration register block structure
     */
    typedef struct NSS_SYSCON_S
    {
        __IO uint32_t SYSMEMREMAP;    /*!< System memory remap */
        __IO uint32_t PRESETCTRL;     /*!< Peripheral reset control */
        __I uint32_t RESERVED1[6];    /*   next field at offset 0x020 */
        __IO uint32_t SYSCLKCTRL;     /*!< System clock control register - Used by Clock driver */
        __IO uint32_t SYSCLKUEN;      /*!< System clock update enable - Used by Clock driver */
        __I uint32_t RESERVED2[2];    /*   next field at offset 0x030 */
        __IO uint32_t SYSRSTSTAT;     /*!< System reset status register */
        __I uint32_t RESERVED3[19];   /*   next field at offset 0x080 */
        __IO uint32_t SYSAHBCLKCTRL;  /*!< AHB clock control - Used by Clock driver */
        __I uint32_t RESERVED4[4];    /*   next field at offset 0x094 */
        __IO uint32_t SSP0CLKDIV;     /*!< SSP0 clock divider - Used by Clock driver */
        __I uint32_t RESERVED5[14];   /*   next field at offset 0x0D0 */
        __IO uint32_t WDTCLKSEL;      /*!< Watchdog timer clock selector - Used by Clock driver */
        __IO uint32_t WDTCLKUEN;      /*!< Watchdog timer clock update enable - Used by Clock driver */
        __IO uint32_t WDTCLKDIV;      /*!< Watchdog timer clock divider - Used by Clock driver */
        __I uint32_t RESERVED6[3];    /*   next field at offset 0x0E8 */
        __IO uint32_t CLKOUTEN;       /*!< CLKOUT enable - Used by Clock driver */
        __I uint32_t RESERVED7[26];   /*   next field at offset 0x154 */
        __IO uint32_t SYSTCKCAL;      /*!< System tick counter calibration */
        __I uint32_t RESERVED8[42];   /*   next field at offset 0x200 */
        __IO uint32_t STARTAPRP0;     /*!< Start logic edge control register 0 */
        __IO uint32_t STARTERP0;      /*!< Start logic signal enable register 0 */
        __IO uint32_t STARTRSRP0CLR;  /*!< Start logic reset register 0 */
        __IO uint32_t STARTSRP0;      /*!< Start logic status register 0 */
        __I uint32_t RESERVED9[10];   /*   next field at offset 0x238 */
        __IO uint32_t PDRUNCFG;       /*!< Power-down configuration register */
        __I uint32_t RESERVED10[110]; /*   next field at offset 0x3F4 */
        __I uint32_t DEVICEID;        /*!< Device ID register */
    } NSS_SYSCON_T;

    /** Possible system wake-up start logic sources */
    typedef enum SYSCON_STARTSOURCE
    {
        SYSCON_STARTSOURCE_PIO0_0 = (1 << 0),   /*!< System wake-up start source - pin PIO0_0 */
        SYSCON_STARTSOURCE_PIO0_1 = (1 << 1),   /*!< System wake-up start source - pin PIO0_1 */
        SYSCON_STARTSOURCE_PIO0_2 = (1 << 2),   /*!< System wake-up start source - pin PIO0_2 */
        SYSCON_STARTSOURCE_PIO0_3 = (1 << 3),   /*!< System wake-up start source - pin PIO0_3 */
        SYSCON_STARTSOURCE_PIO0_4 = (1 << 4),   /*!< System wake-up start source - pin PIO0_4 */
        SYSCON_STARTSOURCE_PIO0_5 = (1 << 5),   /*!< System wake-up start source - pin PIO0_5 */
        SYSCON_STARTSOURCE_PIO0_6 = (1 << 6),   /*!< System wake-up start source - pin PIO0_6 */
        SYSCON_STARTSOURCE_PIO0_7 = (1 << 7),   /*!< System wake-up start source - pin PIO0_7 */
        SYSCON_STARTSOURCE_PIO0_8 = (1 << 8),   /*!< System wake-up start source - pin PIO0_8 */
        SYSCON_STARTSOURCE_PIO0_9 = (1 << 9),   /*!< System wake-up start source - pin PIO0_9 */
        SYSCON_STARTSOURCE_PIO0_10 = (1 << 10), /*!< System wake-up start source - pin PIO0_10 */
        SYSCON_STARTSOURCE_NFC = (1 << 11),     /*!< System wake-up start source - NFC block
                                                     (This source is triggered when the NFC core is activated) */
        SYSCON_STARTSOURCE_RTC = (1 << 12),     /*!< System wake-up start source - RTC
                                                     (This source is triggered when the RTC wake-up downcounter expires)*/
        SYSCON_STARTSOURCE_NONE = 0,            /*!< This is used to select none of the sources (or deselect all sources)*/
        SYSCON_STARTSOURCE_ALL = 0x1FFF         /*!< This is used to select all sources */
    } SYSCON_STARTSOURCE_T;

    /** 16/32-bit Timer register block structure */
    typedef struct NSS_TIMER_S
    {
        __IO uint32_t IR;           /*!< Interrupt Register. The IR can be written to clear interrupts. The IR can be read to
                                        identify which of eight possible interrupt sources are pending. */
        __IO uint32_t TCR;          /*!< Timer Control Register. The TCR is used to control the Timer Counter functions.
                                       The Timer Counter can be disabled or reset through the TCR. */
        __IO uint32_t TC;           /*!< Timer Counter. This register is incremented every PR+1 cycles of PCLK.
                                       The TC is controlled through the TCR. */
        __IO uint32_t PR;           /*!< Prescale Register. When the Prescale Counter (below) is equal to this value, the next clock
                                       increments the TC and clears the PC. */
        __IO uint32_t PC;           /*!< Prescale Counter. The PC is a counter which is incremented to the value stored in PR. When
                                       the value in PR is reached, the TC is incremented and the PC is cleared. The PC is observable
                                       and controllable through the bus interface. */
        __IO uint32_t MCR;          /*!< Match Control Register. The MCR is used to control if an interrupt is generated and if the
                                       TC is reset when a Match occurs. */
        __IO uint32_t MR[4];        /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the
                                       TC and PC, and/or generate an interrupt every time MR matches the TC. */
        __I uint32_t RESERVED0[5];  /*  Next address is 0x03C */
        __IO uint32_t EMR;          /*!< External Match Register. The EMR controls the external match pins MATn.0-3
                                       (MAT0.0-3 and MAT1.0-3 respectively). */
        __I uint32_t RESERVED1[13]; /*  0x040 - 0x070. Next address 0x074 */
        __IO uint32_t PWMC;         /*!< The PWMC enables PWM mode of the external match pins */
    } NSS_TIMER_T;

    /** GPIO port register block structure */
    typedef struct NSS_GPIO_S
    {
        __IO uint32_t DATA[4096]; /*!< Offset: 0x0000 to 0x3FFC Data address masking register (R/W) */
        uint32_t RESERVED1[4096];
        __IO uint32_t DIR;        /*!< Offset: 0x8000 Data direction register (R/W) */
        __IO uint32_t IS;         /*!< Offset: 0x8004 Interrupt sense register (R/W) */
        __IO uint32_t IBE;        /*!< Offset: 0x8008 Interrupt both edges register (R/W) */
        __IO uint32_t IEV;        /*!< Offset: 0x800C Interrupt event register  (R/W) */
        __IO uint32_t IE;         /*!< Offset: 0x8010 Interrupt mask register (R/W) */
        __I uint32_t RIS;         /*!< Offset: 0x8014 Raw interrupt status register (R) */
        __I uint32_t MIS;         /*!< Offset: 0x8018 Masked interrupt status register (R) */
        __O uint32_t IC;          /*!< Offset: 0x801C Interrupt clear register (W) */
        uint32_t RESERVED2[8184]; /* Padding added for aligning contiguous GPIO blocks */
    } NSS_GPIO_T;

    /** Peripherals that can be reset */
    typedef enum SYSCON_PERIPHERAL_RESET
    {
        SYSCON_PERIPHERAL_RESET_SPI0 = (1 << 0),   /*!< Represents the SPI0/SSP0 block reset */
        SYSCON_PERIPHERAL_RESET_I2C0 = (1 << 1),   /*!< Represents the I2C0 block reset */
        SYSCON_PERIPHERAL_RESET_EEPROM = (1 << 2), /*!< Represents the EEPROM controller block reset */
        SYSCON_PERIPHERAL_RESET_NFC = (1 << 3)     /*!< Represents the NFC shared memory interface (APB side), not the NFC block */
    } SYSCON_PERIPHERAL_RESET_T;

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* reg_hal_h_ */
