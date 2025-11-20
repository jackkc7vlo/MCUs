/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       i2c_hal.c
 * @author     Jack Wilson
 * @brief
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Dec-11-2023     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 * Portions Copyright (c), NXP Semiconductors
 * (C)NXP B.V. 2014-2018
 * All rights are reserved. Reproduction in whole or in part is prohibited without
 * the written consent of the copyright owner. NXP reserves the right to make
 * changes without notice at any time. NXP makes no warranty, expressed, implied or
 * statutory, including but not limited to any implied warranty of merchantability
 * or fitness for any particular purpose, or that the use will not infringe any
 * third party patent, copyright or trademark. NXP must not be liable for any loss
 * or damage arising from its use.
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
#include <hw_config.h>
#include <stdbool.h>
#include <reg_hal.h>
#include <i2c_hal.h>
#include <i2c_nss.h>
#include <gpio_hal.h>
#include <assert.h>
#include <stdlib.h> /*lint -e129*/
#include <clock_hal.h>
#if UNIT_TESTS == 1u
#include <unittest.h>

#endif
    /*lint -esym(793,__*)*/
    /*lint -e749 -e754 -e751 -e750 -e9026*/
    /********************************************************************************
     * Defines
     ********************************************************************************/
#define NSS_I2Cx(id) (NSS_I2C)
/** I2C Common Control Assert acknowledge bit */
#define I2C_CON_AA (1UL << 2)
/** I2C Common Control I2C interrupt bit */
#define I2C_CON_SI (1UL << 3)
/** I2C Common Control I2C STOP bit */
#define I2C_CON_STO (1UL << 4)
/** I2C Common Control START flag bit */
#define I2C_CON_STA (1UL << 5)
/** I2C Common Control I2C interface bit */
#define I2C_CON_I2EN (1UL << 6)

/** Return Code mask in I2C status register */
#define I2C_STAT_CODE_BITMASK ((0xF8u))
/** Return Code error mask in I2C status register */
#define I2C_STAT_CODE_ERROR ((0xFF))

#define SLAVE_ACTIVE(iic) (((iic)->flags & 0xFF00) != 0)

#define I2C_CON_FLAGS (I2C_CON_AA | I2C_CON_SI | I2C_CON_STO | I2C_CON_STA)
#include "i2c_nss_data.h"
    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
#if USE_I2C == 1u
    static bool i2c_initialized = false; /*lint !e956*/

    typedef struct i2c_hal_def_s
    {
        uint32_t device_id;          /*!< bus id */
        uint32_t cmd_or_register;    /*!< device register addr */
        p_i2c_callback_t p_callback; /*!< I2C IRQ callback */
        I2C_ID_T iic;                /*!< I2C peripheral */
    } i2c_hal_def_t;

    static p_gpio_hal_def_t p_i2c_enable = NULL; /*lint !e956*/

    static p_gpio_hal_def_t p_scl_gpio = NULL; /*lint !e956*/

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    /* Check if I2C bus is free */
    static inline bool isI2CBusFree(const NSS_I2C_T *const pI2C)
    {
        return !(pI2C->CONSET & I2C_CON_STO);
    }

    /* Transmit and Receive data in master mode */
    I2C_STATUS_T i2c_hal_master_transfer(const I2C_ID_T iic, I2C_XFER_T *xfer)
    {
        iic->mEvent(iic, I2C_EVENT_LOCK);
        xfer->status = I2C_STATUS_BUSY;
        iic->mXfer = xfer;

        /* If slave xfer not in progress */
        //    if (!iic->sXfer) {
        /* Reset STA, STO, SI */
        iic->ip->CONCLR = I2C_CON_SI | I2C_CON_STO | I2C_CON_STA | I2C_CON_AA;

        /* Enter to Master Transmitter mode */
        iic->ip->CONSET = I2C_CON_I2EN | I2C_CON_STA;
        //    }
        iic->mEvent(iic, I2C_EVENT_WAIT);
        iic->mXfer = 0;

        /* Wait for stop condition to appear on bus */
        /* Wait for stop condition to appear on bus */
        while (!isI2CBusFree(iic->ip))
        {
        }

        /* Start slave if one is active */
        if (SLAVE_ACTIVE(iic)) /*lint !e9130 !e9117*/
        {
            // startSlaverXfer(iic->ip);
            /* Reset STA, STO, SI */
            iic->ip->CONCLR = I2C_CON_SI | I2C_CON_STO | I2C_CON_STA;

            /* Enter to Master Transmitter mode */
            iic->ip->CONSET = I2C_CON_I2EN | I2C_CON_AA;
        }

        iic->mEvent(iic, I2C_EVENT_UNLOCK);
        return xfer->status;
    } /*lint !e952*/

    /* Chip event handler interrupt based */
    void i2c_hal_event_handler(const I2C_ID_T iic, const I2C_EVENT_T event)
    {
        volatile I2C_STATUS_T *stat;

        /* Only WAIT event needs to be handled */
        if (event != I2C_EVENT_WAIT)
        {
            return;
        }

        stat = &iic->mXfer->status;
        /* Wait for the status to change */
        int32_t counter = 200; /* A safety counter to avoid possible infinite loops. Does its precise value matter that much? */
        do
        {
            clock_hal_delay((uint32_t)(1u));
            counter--; /* wait */
        } while ((*stat == I2C_STATUS_BUSY) && counter);

    } /*lint !e818 !e954*/

    /********************************************************************************/

    void i2c_hal_initialize(const uint32_t speed)
    {
        if (i2c_initialized)
        {
            return;
        }
        const I2C_ID_T iic = I2C0;

        NSS_SYSCON->PRESETCTRL |= (uint32_t)SYSCON_PERIPHERAL_RESET_I2C0;
        NSS_SYSCON->SYSAHBCLKCTRL |= ((uint32_t)CLOCK_PERIPHERAL_I2C0 & 0x1DBFFCu);

        /* Set I2C operation to default */
        NSS_I2Cx(id)->CONCLR = (I2C_CON_AA | I2C_CON_SI | I2C_CON_STA | I2C_CON_I2EN);

        p_scl_gpio = gpio_hal_create_device(I2C_SCL_PORT, I2C_SCL_PIN,
                                            GPIO_HAL_MODE_OUTPUT, 1u, NULL);

        gpio_hal_init(p_scl_gpio);

        /* Initialize the internal structure */
        iic->ip = NSS_I2C;
        iic->mEvent = i2c_hal_event_handler;
        iic->flags = 0u;
        iic->mXfer = NULL;
        // Use 100 kHz; assumes running at 2MHz; scale appropriately if not.
        if (speed <= 100000u)
        {
            (NSS_I2C)->SCLH = (NSS_I2C)->SCLL = 10u;
        }
        else
        {
            (NSS_I2C)->SCLH = (NSS_I2C)->SCLL = 5u;
        }
        NVIC_EnableIRQ(I2C0_IRQn);

        p_i2c_enable = gpio_hal_create_device(I2C_ENABLE_PORT, I2C_ENABLE_PIN,
                                              GPIO_HAL_MODE_OUTPUT, 1u, NULL);

        gpio_hal_init(p_i2c_enable);
        gpio_hal_set_state(p_i2c_enable, (bool)false); // disable by default

        i2c_initialized = true;
    }

    /********************************************************************************/
    bool i2c_hal_is_initialized(void)
    {
        return i2c_initialized;
    }

    /********************************************************************************/
    p_i2c_hal_def_t i2c_hal_create_device(const uint32_t i2c_device_id,
                                          const uint32_t device_id_len,
                                          const p_i2c_callback_t p_callback)
    {
        const p_i2c_hal_def_t p_i2c_device = calloc(1u, sizeof(i2c_hal_def_t));
        if (p_i2c_device != NULL)
        {
            // and offset of 1 for len of 1.
            if (device_id_len <= 1u)
            {
                p_i2c_device->device_id = i2c_device_id;
            }
            else
            {
                assert(false); /*lint !e506*/
            }

            p_i2c_device->p_callback = p_callback;
            p_i2c_device->iic = I2C0;
        }
        return p_i2c_device;
    }

    void i2c_hal_free(const p_i2c_hal_def_t p_i2c_device)
    {
        assert(p_i2c_device != NULL);
        free((void *)p_i2c_device);
    }

    /********************************************************************************/

    uint_fast16_t i2c_hal_write(const p_i2c_hal_def_t p_i2c_device, const uint32_t cmd_or_register,
                                const uint32_t cmd_or_register_len,
                                const uint8_t *const data, const uint_fast16_t data_len)
    {
        I2C_STATUS_T status = I2C_STATUS_BUSY;
        const uint16_t *ptr = (uint16_t *)data; /*lint !e826*/
        uint16_t address = (uint16_t)cmd_or_register;

        // determine the offset into buf for the data.  If it's
        const uint32_t buf_offset = 2u - cmd_or_register_len;

#ifdef I2C_POWER_CHIP
        if (i2c_auto_power)
            i2c_power((bool)true, 1000u); // Monza2K takes up to 2ms to power up
#endif
        // transfer 1 uint16_t at a time
        for (uint_fast16_t i = 0u;
             i < (data_len + 1u) / 2u;
             i++, ptr += 1, address += 2u) // 2 bytes at a time
        {
            const uint8_t buf[4] = {
                (uint8_t)(address >> 8), // 16-bits of address (big-endian)
                (uint8_t)address,
                (uint8_t)*ptr, // 16-bits of data (little-endian?)
                (uint8_t)(*ptr >> 8)};

            /* The I2C timing constraints are unknown, but need quite some time to handle a previous transaction and be
             * prepared for the next: around 5 ms (likely the time required by the EEPROM in the UCode-I2C to flush the data),
             * but it is unknown what the variation is and where it depends upon.
             * To be sure, we try multiple times, each time waiting some time to be sure attempts have been made for a period
             * longer than 20 msec (Monza2K can sulk that long handling an RF transaction).
             */
            int32_t tries;
            for (tries = 22; tries > 0; --tries)
            {
                clock_hal_delay(1u); // Delay 1ms per try

                // Fill in xfer for each attempt as it's changed by Chip_I2C_MasterTransfer()
                I2C_XFER_T xfer = {
                    .slaveAddr = (uint8_t)p_i2c_device->device_id,
                    .txBuff = buf + buf_offset,
                    .txSz = (int32_t)(cmd_or_register_len + 1U),
                    .rxBuff = 0,
                    .rxSz = 0}; /*lint !e785*/
                status = i2c_hal_master_transfer(I2C0, &xfer);
                if (status == I2C_STATUS_DONE)
                {
#if defined(MONZA2K) || defined(MONZA8K)
                    clock_hal_delay(47u); // Nominal time for write
#endif

                    break;
                }
            }

            if (tries <= 0)
            {
#ifdef USE_PRINTF
                // printf("i2c Send timeout");
#endif
                break;
            }
        }

#if defined(MONZA2K) || defined(MONZA8K)
        clock_hal_delay(10u); // Give the final write a little more time
#ifdef I2C_POWER_CHIP
        if (i2c_auto_power)
            i2c_power((bool)false, 0u);
#endif
#endif

        if (status == I2C_STATUS_DONE)
        {
            return data_len;
        }
        return 0u;
    } /*lint !e818*/

    /********************************************************************************
     * @brief          Read function for the I2C driver
     * @param[in]      i2c_device_id - I2C device ID
     * @param[in]      cmd_or_register - The memory address in the i2c device (i.e. registers)
     * @param[in]      data -  pointer to data (in bytes!) to read
     * @param[in]      data_len - Length of data to read in bytes
     * @retval         Number of bytes read
     ********************************************************************************/

    uint_fast16_t i2c_hal_read(const p_i2c_hal_def_t p_i2c_device, const uint32_t cmd_or_register,
                               const uint32_t cmd_or_register_len,
                               uint8_t *const data, const uint_fast16_t data_len)
    {
        uint_fast16_t ret_len = 0u;

        const uint8_t chipdata[2] = {
            (uint8_t)(cmd_or_register >> 8),
            (uint8_t)cmd_or_register};
#ifdef I2C_POWER_CHIP
        if (i2c_auto_power)
            i2c_power((bool)true, 1000u); // make sure on regardless of chip
                                          // Per loop delay below makes 2ms
#endif
        I2C_STATUS_T status;
        for (int32_t tries = 22; tries > 0; --tries)
        {
            // Delay 1ms per try
            clock_hal_delay(1u);

            //                    .txBuff = buf + (cmd_or_register_len & 1u),
            //                    .txSz = (int32_t)(cmd_or_register_len + 2U),

            // Fill in xfer for each attempt as it's changed by Chip_I2C_MasterTransfer()
            I2C_XFER_T xfer = {
                .slaveAddr = (uint8_t)p_i2c_device->device_id,
                .txBuff = chipdata + (cmd_or_register_len & 1u),
                .txSz = (int32_t)cmd_or_register_len,
                .rxBuff = data,
                .rxSz = (int32_t)data_len}; /*lint !e785 */
            status = i2c_hal_master_transfer(I2C0, &xfer);
            if (status == I2C_STATUS_DONE)
            {
                ret_len = data_len;
                break;
            }
        }
        // clock_hal_delay(10u);
#ifdef I2C_POWER_CHIP
#if defined(MONZA2K) || defined(MONZA8K)
        // turn off power to the chip so RF can read for Monza
        if (i2c_auto_power)
            i2c_power((bool)false, 5000u);
#endif
#endif

        return ret_len;
    } /*lint !e818*/

    /********************************************************************************
     * @brief  Enable (power on) or disable (power off) to the i2c devices
     * @retval
     ********************************************************************************/
    void i2c_hal_enable(const bool turn_on, const uint32_t settle_time_us)
    {
        gpio_hal_set_state(p_i2c_enable, (bool)turn_on);
        clock_hal_delay(settle_time_us);
    }

#if USE_I2C_INTERRUPTS == 1u
    /**
     * @brief  Handle an I2C interrupt
     * @retval None
     */
    /* Get current state of the I2C peripheral */
    static inline int32_t getCurState(const NSS_I2C_T *const pI2C)
    {
        return (int32_t)(pI2C->STAT & I2C_STAT_CODE_BITMASK);
    }

    /* Check if the active state belongs to master mode*/
    static inline int32_t isMasterState(const NSS_I2C_T *const pI2C)
    {
        return getCurState(pI2C) < 0x60;
    }

    /* Master transfer state change handler handler */
    /*lint -e9130 -e704*/
    int32_t handleMasterXferState(NSS_I2C_T *pI2C, I2C_XFER_T *xfer)
    {
        uint32_t cclr = I2C_CON_FLAGS;

        // OE:	getCurState ands the register with 0xF8, so the bottom 3 bits are always 0
        //		Right-shifting by 3 avoids the switch jump table being sparse and in fact,
        //		makes it one eighth as big as it would be otherwise!
        switch (getCurState(pI2C) >> 3u)
        {
        case 0x08 >> 3:                                                         /* Start condition on bus */
        case 0x10 >> 3:                                                         /* Repeated start condition */
            pI2C->DAT = (uint32_t)((xfer->slaveAddr << 1) | (xfer->txSz == 0)); /*lint !e514 !e9117*/
            break;

            /* Tx handling */
        case 0x18 >> 3: /* SLA+W sent and ACK received */
        case 0x28 >> 3: /* DATA sent and ACK received */
            if (!xfer->txSz)
            {
                cclr &= ~(xfer->rxSz ? I2C_CON_STA : I2C_CON_STO);
            }
            else
            {
                pI2C->DAT = *xfer->txBuff++;
                xfer->txSz--;
            }
            break;

            /* Rx handling */
        case 0x58 >> 3: /* Data Received and NACK sent */
            cclr &= ~I2C_CON_STO;
            /* fallthrough */
            /* no break */

        case 0x50 >> 3: /* Data Received and ACK sent */ /*lint !e825*/
            *xfer->rxBuff++ = (uint8_t)pI2C->DAT;
            xfer->rxSz--;
            /* fallthrough */
            /* no break */

        case 0x40 >> 3: /* SLA+R sent and ACK received */ /*lint !e825*/
            if (xfer->rxSz > 1)
            {
                cclr &= ~I2C_CON_AA;
            }
            break;

            /* NAK Handling */
        case 0x30 >> 3: /* DATA sent NAK received */
            xfer->txBuff--;
            xfer->txSz++;
            /* no break */
            /* fallthrough */
        case 0x20 >> 3: /* SLA+W sent NAK received */ /*lint !e825*/
        /* fallthrough */
        case 0x48 >> 3: /* SLA+R sent NAK received */
            xfer->status = I2C_STATUS_NAK;
            cclr &= ~I2C_CON_STO;
            break;

        case 0x38 >> 3: /* Arbitration lost */
            xfer->status = I2C_STATUS_ARBLOST;
            break;

            /* Bus Error */
        case 0x00 >> 3: /*lint !e572 !e835*/
            xfer->status = I2C_STATUS_BUSERR;
            cclr &= ~I2C_CON_STO;
            break;
        default:
            break;
        }

        /* Set clear control flags */
        pI2C->CONSET = cclr ^ I2C_CON_FLAGS;
        pI2C->CONCLR = cclr;

        /* If stopped return 0 */
        if (!(cclr & I2C_CON_STO) || (xfer->status == I2C_STATUS_ARBLOST))
        {
            if (xfer->status == I2C_STATUS_BUSY)
            {
                xfer->status = I2C_STATUS_DONE;
            }
            return 0;
        }
        return 1;
    } /*lint !e952*/

    /* State change handler for master transfer */
    void Chip_I2C_MasterStateHandler(const I2C_ID_T iic)
    {
        if (!handleMasterXferState(iic->ip, iic->mXfer))
        {
            iic->mEvent(iic, I2C_EVENT_DONE);
        }
    }

    void I2C0_IRQHandler(void)
    {
        if (isMasterState(I2C0->ip))

        {
            Chip_I2C_MasterStateHandler(I2C0);
        }
        /*
        else
        {
            Chip_I2C_SlaveStateHandler(I2C0);
        }
        */
    }
#endif

#if UNIT_TESTS == 1u
    void i2c_hal_unit_test(void)
    {
        unittest_start_test("i2c_hal_unit_test");

        unittest_start_testcase("find devices");

        i2c_hal_initialize(100000u);

        uint8_t chipdata[2];
        i2c_hal_enable((bool)true, 100u);

        for (uint32_t i = 0u; i < 128u; i++)
        {
            const p_i2c_hal_def_t p_i2c_device = i2c_hal_create_device(i, 1u, NULL);
            const uint32_t cmd_data = 0u;

            if (i2c_hal_read(p_i2c_device, cmd_data, 0u, chipdata, 1u) > 0u)
            {
                switch (i)
                {
                case 0x22:
#ifdef USE_PRINTF
                    (void)unittest_printf("   i2c\n");
#endif
                    break;
                case 0x29:
#ifdef USE_PRINTF
                    (void)unittest_printf("   TLS2561\n");
#endif
                    break;
                case 0x48:
#ifdef USE_PRINTF
                    (void)unittest_printf("   lm75\n");
#endif
                    break;
                case 0x52:
#ifdef USE_PRINTF
                    (void)unittest_printf("   rtc\n");
#endif
                    break;
                case 0x53:
#ifdef USE_PRINTF
                    (void)unittest_printf("   ADXL345\n");
#endif
                    break;
                case 0x6e:
#ifdef USE_PRINTF
                    (void)unittest_printf("   Monza\n");
#endif
                    break;
                case 0x76:
#ifdef USE_PRINTF
                    (void)unittest_printf("   BME280\n");
#endif
                    break;
                default:
#ifdef USE_PRINTF
                    (void)unittest_printf("   Unknown %x\n", i);
#endif
                    break;
                }
            }

            i2c_hal_free(p_i2c_device);
        }

        i2c_hal_enable((bool)false, 100u);
        (void)unittest_check_equal(true, true);

        unittest_end_test();
    }

#endif /* UNIT_TESTS == 1u */
#endif /* USE_I2C == 1u */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
