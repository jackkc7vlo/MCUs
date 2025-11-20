/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       chip_hal.c
 * @author     Jack Wilson
 * @brief      Chip specific hardware abstraction layer
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Dec-20-2023     Jack Wilson                     jackkc7vlo@gmail.com
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
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************/

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <chip_hal.h>
#include <reg_hal.h>
#include <iocon_nss.h>

    /*lint -e9130 -e750 -e835*/
    /********************************************************************************
     * Defines
     ********************************************************************************/
#define BOARD_PIO0_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO1_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO2_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO3_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO4_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO5_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO6_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO7_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO8_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO9_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO10_PULL IOCON_RMODE_PULLDOWN
#define BOARD_PIO11_PULL IOCON_RMODE_PULLDOWN
    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    void chip_hal_initialize_IO(void)
    {
        // setup IO
        Chip_IOCON_Init(NSS_IOCON);
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_0, IOCON_FUNC_0 | IOCON_RMODE_PULLUP); // Wakeup
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_4, IOCON_FUNC_0 | BOARD_PIO4_PULL);    // SCL
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_5, IOCON_FUNC_0 | BOARD_PIO5_PULL);    // SDA
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_10, IOCON_FUNC_2 | BOARD_PIO10_PULL);  // SWCLK
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_11, IOCON_FUNC_2 | BOARD_PIO11_PULL);  // SWDIO
#if USE_SPI == 1u
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_1, IOCON_FUNC_0 | BOARD_PIO1_PULL); // Red Led
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_3, IOCON_FUNC_0 | BOARD_PIO3_PULL); // Green Led
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_2, IOCON_FUNC_1);                   // CS
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_6, IOCON_FUNC_1);                   // SCLK
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_8, IOCON_FUNC_1);                   // MISO
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_9, IOCON_FUNC_1);                   // MOSI

        // Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_2, IOCON_FUNC_1); /* SSEL */ /*lint !e9066 */
        Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_7, IOCON_FUNC_0 | IOCON_RMODE_INACT); // Chip power
#else
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_1, IOCON_FUNC_0 | IOCON_RMODE_PULLUP);
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_3, IOCON_FUNC_0 | IOCON_RMODE_PULLUP);
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_8, IOCON_FUNC_0 | BOARD_PIO8_PULL); // Green Led
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_9, IOCON_FUNC_0 | BOARD_PIO9_PULL); // Red Led
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_2, IOCON_FUNC_0 | BOARD_PIO2_PULL);
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_6, IOCON_FUNC_0 | IOCON_RMODE_INACT); /*lint !e845*/
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_7, IOCON_FUNC_0 | IOCON_RMODE_PULLUP);

    /* Configure PIOs for I2C operation. */
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_4, IOCON_FUNC_1);
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_5, IOCON_FUNC_1);

    /* Setup I2C block. */
    NSS_SYSCON->PRESETCTRL |= (uint32_t)SYSCON_PERIPHERAL_RESET_I2C0 & 0xFu;
    NSS_GPIO[0].DIR &= ~((1u << 0u) | (1u << 1u) | (1u << 3u) | (1u << 7u));

    // TODO move this to i2c_hal.c?
    // Initialize wakeup source (SCL is connected to PIO0)
    NSS_SYSCON->STARTERP0 = (uint32_t)SYSCON_STARTSOURCE_PIO0_0 & 0x1FFFu;
    // ModifyRegister(&NSS_PMU->PCON, PMU_PCON_WAKEUPFLAG_MASK, (uint32_t)(enabled != 0) << PMU_PCON_WAKEUPFLAG_POS);
    // Chip_SysCon_Peripheral_DeassertReset(SYSCON_PERIPHERAL_RESET_I2C0);
    //  Chip_I2C_Init(I2C0);
    //  Chip_I2C_SetClockRate(I2C0, 100000);
    //(void)Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
    //  NVIC_EnableIRQ(I2C0_IRQn);

    // Pullups are set in Board_IO_Init()
    // Chip_GPIO_SetPortDIRInput(NSS_GPIO, 0, (1 << 0) | (1 << 1) | (1 << 3) | (1 << 7));

    /* Power up Ucode-I2C. */
    // Chip_GPIO_SetPinDIROutput(NSS_GPIO, 0, I2C_POWER_PIN);
    // Chip_GPIO_SetPinState(NSS_GPIO, 0, I2C_POWER_PIN, false);
#endif
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
