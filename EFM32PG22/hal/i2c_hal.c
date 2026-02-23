/******************************************************************************
 * Copyright (C) 2026 by Jack Wilson
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
 ************************(C) COPYRIGHT 2026 Jack Wilson **********************/
/**
 * @file i2c_hal.c
 * @brief Generic I2C interface implementation.
 * @details Outlines the portable I2C abstraction used to configure and
 * interact with I2C peripherals on any supported platform.
 *
 * @author  Jack Wilson
 * @date    February 12, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

/********************************************************************************
 * Includes
 ********************************************************************************/

#include <assert.h>
#include <clock_hal.h>
#include <device_reg.h>
#include <efm32pg22c200f512im40.h>
#include <gpio_hal.h>
#include <hw_config.h>
#include <i2c_hal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h> /*lint -e129*/

#if HW_CONFIG_I2C == 1u

/********************************************************************************
 * Defines
 ********************************************************************************/
#define I2C_TXBUFFER_SIZE 10u
#define I2C_RXBUFFER_SIZE 10u
#define I2C_FLAG_WRITE 0x0001u
#define I2C_FLAG_READ 0x0002u
#define I2C_FLAG_WRITE_READ 0x0004u
#define I2C_FLAG_WRITE_WRITE 0x0008u
/** Use 10 bit address. */
#define I2C_FLAG_10BIT_ADDR 0x0010U
#define I2C_IF_ERRORS (I2C_IF_BUSERR | I2C_IF_ARBLOST)
#define I2C_IEN_ERRORS (I2C_IEN_BUSERR | I2C_IEN_ARBLOST)
/********************************************************************************
 * Typedefs & Enums
 ********************************************************************************/

static const uint8_t i2cNSum[]       = {4u + 4u, 6u + 3u, 11u + 6u, 4u + 4u};
static bool          i2c_initialized = false;
/********************************************************************************
 * Defines
 ********************************************************************************/
typedef struct
{
    /**
     * @brief
     *   Address to use after (repeated) start.
     * @details
     *   Layout details, A = Address bit, X = don't care bit (set to 0):
     *   @li 7 bit address - Use format AAAA AAAX
     *   @li 10 bit address - Use format XXXX XAAX AAAA AAAA
     */
    uint16_t addr;

    /** Flags defining sequence type and details, see I2C_FLAG_ defines. */
    uint16_t flags;

    /**
     * Buffers used to hold data to send from or receive into, depending
     * on sequence type.
     */
    struct
    {
        /** Buffer used for data to transmit/receive, must be @p len long. */
        uint8_t *data;

        /**
         * Number of bytes in @p data to send or receive. Notice that when
         * receiving data to this buffer, at least 1 byte must be received.
         * Setting @p len to 0 in the receive case is considered a usage fault.
         * Transmitting 0 bytes is legal, in which case only the address
         * is transmitted after the start condition.
         */
        uint16_t len;
    } buf[2];
} I2C_TransferSeq_TypeDef;

typedef enum
{
    /* In progress code (>0) */
    i2cTransferInProgress = 1, /**< Transfer in progress. */

    /* Complete code (=0) */
    i2cTransferDone = 0, /**< Transfer completed successfully. */

    /* Transfer error codes (<0). */
    i2cTransferNack       = -1, /**< NACK received during transfer. */
    i2cTransferBusErr     = -2, /**< Bus error during transfer (misplaced START/STOP). */
    i2cTransferArbLost    = -3, /**< Arbitration lost during transfer. */
    i2cTransferUsageFault = -4, /**< Usage fault. */
    i2cTransferSwFault    = -5  /**< SW fault. */
} I2C_TransferReturn_TypeDef;

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Master mode transfer states. */
typedef enum
{
    i2cStateStartAddrSend,    /**< Send start + (first part of) address. */
    i2cStateAddrWFAckNack,    /**< Wait for ACK/NACK on (the first part of) address. */
    i2cStateAddrWF2ndAckNack, /**< Wait for ACK/NACK on the second part of a 10 bit address. */
    i2cStateRStartAddrSend,   /**< Send a repeated start + (first part of) address. */
    i2cStateRAddrWFAckNack,   /**< Wait for ACK/NACK on an address sent after a repeated start. */
    i2cStateDataSend,         /**< Send data. */
    i2cStateDataWFAckNack,    /**< Wait for ACK/NACK on data sent. */
    i2cStateWFData,           /**< Wait for data. */
    i2cStateWFStopSent,       /**< Wait for STOP to have been transmitted. */
    i2cStateDone              /**< Transfer completed successfully. */
} I2C_TransferState_TypeDef;

typedef struct
{
    /** Current state. */
    I2C_TransferState_TypeDef state;

    /** Result return code. */
    I2C_TransferReturn_TypeDef result;

    /** Offset in the current sequence buffer. */
    uint16_t offset;

    /* Index to the current sequence buffer in use. */
    uint8_t bufIndx;

    /** Reference to the I2C transfer sequence definition provided by the user. */
    I2C_TransferSeq_TypeDef *seq;

    /** Timeout counter for polled mode */
    uint32_t timeout;
} I2C_Transfer_TypeDef;

// support for I2C callbacks
typedef struct i2c_callback_context_s
{                                       /**< The GPIO pin number */
    p_i2c_callback_t callback;          /**< Registered interrupt callback */
    void            *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
    void            *callback_context;  /**< User callback context */
} i2c_callback_context_t;

/********************************************************************************
 * Private I2C HAL struct (definition kept in the implementation file).
 ********************************************************************************/
struct i2c_hal
{
    bool                           in_use;    /*!< Indicates if this I2C instance is in use */
    uint32_t                       device_id; /*!< bus id */
    bool                           address_is_7_bit; /*!< 7 bit address */
    p_gpio_hal_t                   p_scl_gpio;
    p_gpio_hal_t                   p_sda_gpio; /*lint !e551*/
    p_i2c_callback_t               p_callback; /*!< I2C IRQ callback */
    I2C_TypeDef                   *i2c_port;   /*!< I2C peripheral */
    volatile I2C_Transfer_TypeDef *transfer;   /*!< I2C transfer state information */
    i2c_callback_context_t         callback;   /**< Registered interrupt callbacks */
};

I2C_TransferReturn_TypeDef I2C_TransferInit(p_i2c_hal_t p_handle, I2C_TransferSeq_TypeDef *seq);
I2C_TransferReturn_TypeDef I2C_Transfer(p_i2c_hal_t p_handle);

typedef enum
{
    i2cClockHLRStandard  = _I2C_CTRL_CLHR_STANDARD,   /**< Ratio is 4:4 */
    i2cClockHLRAsymetric = _I2C_CTRL_CLHR_ASYMMETRIC, /**< Ratio is 6:3 */
    i2cClockHLRFast      = _I2C_CTRL_CLHR_FAST        /**< Ratio is 11:3 */
} I2C_ClockHLR_TypeDef;

typedef struct
{
    I2C_TypeDef         *port;       /**< Peripheral port */
    uint32_t             sclPort;    /**< SCL pin port number */
    uint8_t              sclPin;     /**< SCL pin number */
    uint32_t             sdaPort;    /**< SDA pin port number */
    uint8_t              sdaPin;     /**< SDA pin number */
    uint32_t             i2cRefFreq; /**< I2C reference clock */
    uint32_t             i2cMaxFreq; /**< I2C max bus frequency to use */
    I2C_ClockHLR_TypeDef i2cClhr;    /**< Clock low/high ratio control */
} I2CSPM_Init_TypeDef;

const I2CSPM_Init_TypeDef init_sensor = {.port       = I2C1,
                                         .sclPort    = GPIO_PORTD,
                                         .sclPin     = 3u,
                                         .sdaPort    = GPIO_PORTD,
                                         .sdaPin     = 2u,
                                         .i2cRefFreq = 0u,
                                         .i2cMaxFreq = 100000u,
                                         .i2cClhr    = i2cClockHLRStandard};

static i2c_hal_t            i2c_hal_devices[NUM_I2C_DEVICES] = {0};
static I2C_Transfer_TypeDef i2c_transfers[NUM_I2C_DEVICES]   = {0};

void i2c_hal_bus_freq_set(I2C_TypeDef *const i2c, uint32_t freqRef, const uint32_t freqScl,
                          uint32_t i2cMode)
{
    uint32_t n;
    uint32_t minFreq;
    uint32_t denominator;
    int32_t  divideby;

    /* Avoid dividing by 0. */
    // assert(freqScl != 0u);
    if (freqScl == 0u)
    {
        return;
    }

    /* Ensure mode is valid */
    i2cMode &= _I2C_CTRL_CLHR_MASK >> _I2C_CTRL_CLHR_SHIFT;

    /* Set the CLHR (clock low-to-high ratio). */
    i2c->CTRL &= ~_I2C_CTRL_CLHR_MASK;
    device_reg_masked_write(&i2c->CTRL, _I2C_CTRL_CLHR_MASK, i2cMode << _I2C_CTRL_CLHR_SHIFT);

    /*
             case cmuClock_PCLK:
          ret = (CMU->SYSCLKCTRL & _CMU_SYSCLKCTRL_PCLKPRESC_MASK)
                >> _CMU_SYSCLKCTRL_PCLKPRESC_SHIFT;
    */

    if (freqRef == 0u)
    {
        const uint32_t clock_div = ((CMU->SYSCLKCTRL & _CMU_SYSCLKCTRL_PCLKPRESC_MASK) >>
                                    _CMU_SYSCLKCTRL_PCLKPRESC_SHIFT) +
                                   1u;
        if (i2c == I2C0)
        {
            // freqRef = CMU_ClockFreqGet(cmuClock_I2C0);
            freqRef = SystemHCLKGet() / (clock_div / 2U);
#if defined(I2C1)
        }
        else if (i2c == I2C1)
        {
            freqRef = SystemHCLKGet() / clock_div;
            // freqRef = CMU_ClockFreqGet(cmuClock_I2C1);
#endif
        }
        else
        {
            assert((bool)false); /*lint !e506*/
        }
    }

    /* Check the minumum HF peripheral clock. */
    minFreq = 14000000u; /*lint !e9117*/
    if (i2c->CTRL & I2C_CTRL_SLAVE)
    {
        switch (i2cMode)
        {
        case i2cClockHLRStandard:
            minFreq = 2000000u;
            break;
        case i2cClockHLRAsymetric:
            minFreq = 5000000u;
            break;
        case i2cClockHLRFast:
            minFreq = 14000000u;
            break;
        default:
            /* MISRA requires the default case. */
            break;
        }
    }
    else
    {
        /* For master mode, platform 1 and 2 share the same
           minimum frequencies. */
        switch (i2cMode)
        {
        case i2cClockHLRStandard:
            minFreq = 2000000u;
            break;
        case i2cClockHLRAsymetric:
            minFreq = 9000000u;
            break;
        case i2cClockHLRFast:
            minFreq = 20000000u;
            break;
        default:
            /* MISRA requires default case */
            break;
        }
    }

    /* Frequency must be larger-than. */
    assert(freqRef > minFreq);

    /* SCL frequency is given by:
     * freqScl = freqRef/((Nlow + Nhigh) * (DIV + 1) + I2C_CR_MAX)
     *
     * Therefore,
     * DIV = ((freqRef - (I2C_CR_MAX * freqScl))/((Nlow + Nhigh) * freqScl)) - 1
     *
     * For more details, see the reference manual
     * I2C Clock Generation chapter.  */

    /* n = Nlow + Nhigh */
    n           = (uint32_t)i2cNSum[i2cMode];
    denominator = n * freqScl;

    /* Explicitly ensure denominator is never zero. */
    if (denominator == 0u)
    {
        // assert(0);
        return;
    }
    /* Perform integer division so that div is rounded up. */
    divideby = ((freqRef - (8 * freqScl) + denominator - 1) / denominator) -
               1; /*lint !e9117 !e834 !e713 */
    assert(divideby >= 0);
    assert((uint32_t)divideby <= _I2C_CLKDIV_DIV_MASK);

    /* The clock divisor must be at least 1 in slave mode according to the reference */
    /* manual (in which case there is normally no need to set the bus frequency). */
    if ((i2c->CTRL & I2C_CTRL_SLAVE) && (divideby == 0))
    {
        divideby = 1;
    }
    i2c->CLKDIV = (uint32_t)divideby;
}
/********************************************************************************
 * @brief          Initialization function for the I2C driver
 * @retval         None
 ********************************************************************************/
void i2c_hal_initialize(void)
{
    if (!i2c_initialized)
    {
        // perform any necessary hardware initialization here
        i2c_initialized = true;
    }
}

/********************************************************************************
 * @brief          Check if the I2C driver is initialized
 * @retval         true if initialized, false otherwise
 ********************************************************************************/
bool i2c_hal_is_initialized(void)
{
    return i2c_initialized;
}

p_i2c_hal_t i2c_hal_create_device(const uint32_t i2c_board_id, const uint32_t sda_port,
                                  const uint32_t sda_pin, const uint32_t scl_port,
                                  const uint32_t scl_pin, const p_i2c_callback_t p_callback)
{
    if (i2c_board_id > NUM_I2C_DEVICES)
    {
        return NULL; // Invalid device ID
    }

    p_i2c_hal_t p_free_slot = NULL;
    if (!i2c_initialized)
    {
        i2c_hal_initialize();
    }
    for (uint32_t i = 0; i < NUM_I2C_DEVICES; i++)
    {
        if (i2c_hal_devices[i].in_use == true)
        {
            return &i2c_hal_devices[i]; // Device already in use, return existing device
        }
        else if (p_free_slot == NULL)
        {
            p_free_slot = (p_i2c_hal_t)&i2c_hal_devices[i]; /* Track first free */
            p_free_slot->callback.callback          = p_callback;
            p_free_slot->callback.p_callback_handle = NULL;
            p_free_slot->callback.callback_context =
                (void *)i; // pass the timer index as the callback context

            // p_free_slot->enabled           = false;
            p_free_slot->device_id        = i2c_board_id; /*!< bus id */
            p_free_slot->address_is_7_bit = true;         /*!< 7 bit address */

            p_free_slot->in_use = true;
            uint32_t i2cClock;
            // I2C_Init_TypeDef i2cInit;
            I2CSPM_Init_TypeDef *init = (I2CSPM_Init_TypeDef *)&init_sensor;
            if (i2c_board_id == 0)
            {
                init->port = I2C0;
                i2cClock   = (1u << 5u) | (14u << 0u); /*lint !e835*/
            }
            else if (i2c_board_id == 1)
            {
                init->port = I2C1;
                i2cClock   = (1u << 5u) | (15u << 0u); /*lint !e835*/
            }
            else
            {
                assert((bool)false); /*lint !e506*/
                return NULL;         /*lint !e527*/
            }

            p_free_slot->i2c_port = init->port;        /*!< I2C peripheral */
            p_free_slot->transfer = &i2c_transfers[i]; /*!< I2C transfer state */

            init->sclPort    = scl_port;
            init->sclPin     = scl_pin;
            init->sdaPort    = sda_port;
            init->sdaPin     = sda_pin;
            init->i2cRefFreq = 0u;
            init->i2cMaxFreq = 100000u;
            init->i2cClhr    = i2cClockHLRStandard;

            clock_hal_enable(i2cClock, (bool)true);

            /* Configure GPIO pins for I2C */
            /* SDA and SCL should be configured as wired-and with filter and pull-up */
            if (scl_pin < 8u)
            {
                GPIO->P[scl_port].MODEL &= ~(_GPIO_P_MODEL_MODE0_MASK << (scl_pin * 4u));
                GPIO->P[scl_port].MODEL |=
                    (GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER << (scl_pin * 4u));
            }
            else
            {
                GPIO->P[scl_port].MODEH &= ~(_GPIO_P_MODEH_MODE0_MASK << ((scl_pin - 8u) * 4u));
                GPIO->P[scl_port].MODEH |=
                    (GPIO_P_MODEH_MODE0_WIREDANDPULLUPFILTER << ((scl_pin - 8u) * 4u));
            }

            if (sda_pin < 8u)
            {
                GPIO->P[sda_port].MODEL &= ~(_GPIO_P_MODEL_MODE0_MASK << (sda_pin * 4u));
                GPIO->P[sda_port].MODEL |=
                    (GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER << (sda_pin * 4u));
            }
            else
            {
                GPIO->P[sda_port].MODEH &= ~(_GPIO_P_MODEH_MODE0_MASK << ((sda_pin - 8u) * 4u));
                GPIO->P[sda_port].MODEH |=
                    (GPIO_P_MODEH_MODE0_WIREDANDPULLUPFILTER << ((sda_pin - 8u) * 4u));
            }

            /* Enable pins and set location */
            if (init->port == I2C0)
            {
                GPIO->I2CROUTE[0].ROUTEEN =
                    GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN; /*lint !e835*/
                GPIO->I2CROUTE[0].SCLROUTE =
                    (uint32_t)((init->sclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT) |
                               (init->sclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT)); /*lint !e835*/
                GPIO->I2CROUTE[0].SDAROUTE =
                    (uint32_t)((init->sdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT) |
                               (init->sdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT)); /*lint !e835*/
            }

            if (init->port == I2C1)
            {
                GPIO->I2CROUTE[1].ROUTEEN =
                    GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN; /*lint !e835*/
                GPIO->I2CROUTE[1].SCLROUTE =
                    (uint32_t)((init->sclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT) |
                               (init->sclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT)); /*lint !e835*/
                GPIO->I2CROUTE[1].SDAROUTE =
                    (uint32_t)((init->sdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT) |
                               (init->sdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT)); /*lint !e835*/
            }

            init->port->IEN    = 0u;
            init->port->IF_CLR = _I2C_IF_MASK;

            // set i2c as a master
            // set i2c as a master
            device_reg_bit_write(&(init->port->CTRL), _I2C_CTRL_SLAVE_SHIFT, 0u); /*lint !e9117*/

            i2c_hal_bus_freq_set(init->port, init->i2cRefFreq, init->i2cMaxFreq,
                                 (int32_t)init->i2cClhr); /*lint !e9117*/

            // enable i2c
            device_reg_bit_write(&(init->port->EN), _I2C_EN_EN_SHIFT, true); /*lint !e9117*/
        }
    }
    // Additional hardware-specific initialization can be done here

    return p_free_slot;
}

/********************************************************************************
 * @brief          Write function for the I2C driver
 * @param[in]      p_handle - pointer to the i2c device
 * @param[in]      cmd_or_register - The register or command to write data to
 *(i.e. memory address)
 * @param[in]      cmd_or_register_len - Length of the I2 register or command in
 *bytes
 * @param[in]      data -  pointer to data to send (may be words depending on
 *MCU)
 * @param[in]      data_len - Length of data to write in bytes
 * @retval         Number of bytes written
 ********************************************************************************/
uint32_t i2c_hal_write(const p_i2c_hal_t p_handle, const uint32_t slave_address,
                       const uint32_t cmd_or_register, const uint32_t cmd_or_register_len,
                       const uint8_t *const data, const uint_fast16_t data_len)
{
    // char          error          = 0;
    // uint_fast16_t bytes_written  = 0;
    // uint_fast16_t bytes_to_write = data_len;
    // uint8_t      *pdata          = (uint8_t *)data;

    I2C_TransferSeq_TypeDef    i2cTransferData;
    I2C_TransferReturn_TypeDef result;
    const uint8_t chipdata[2] = {(uint8_t)(cmd_or_register >> 8), (uint8_t)cmd_or_register};

    // Initialize I2C transfer
    i2cTransferData.addr  = (uint16_t)slave_address;
    i2cTransferData.flags = I2C_FLAG_WRITE;
    if (cmd_or_register_len > 0u)
    {
        i2cTransferData.flags       = I2C_FLAG_WRITE_WRITE;
        i2cTransferData.buf[0].data = (uint8_t *)(chipdata + (cmd_or_register_len & 1u));
        i2cTransferData.buf[0].len  = (uint16_t)cmd_or_register_len;

        i2cTransferData.buf[1].data = (uint8_t *)data;
        i2cTransferData.buf[1].len  = (uint16_t)data_len;
    }
    else
    {
        i2cTransferData.buf[0].data = (uint8_t *)data;
        i2cTransferData.buf[0].len  = (uint16_t)data_len;
    }

    result = I2C_TransferInit(p_handle, &i2cTransferData);

    // Send data
    while (result == i2cTransferInProgress)
    {
        result = I2C_Transfer(p_handle);
    }

    if (result != i2cTransferDone)
    {
        return 0u;
    }

    return data_len;
    /* wait until bus is not busy */
}

/********************************************************************************
 * @brief          Read function for the I2C driver
 * @param[in]      p_handle - pointer to the i2c device
 * @param[in]      cmd_or_register - The register or command to write data to
 *(i.e. memory address)
 * @param[in]      cmd_or_register_len - Length of the register or command in
 *bytes
 * @param[in]      data -  pointer to data (in bytes!) to read
 * @param[in]      data_len - Length of data to read in bytes
 * @retval         Number of bytes read
 ********************************************************************************/
uint32_t i2c_hal_read(const p_i2c_hal_t p_handle, const uint32_t slave_address,
                      const uint32_t cmd_or_register, const uint32_t cmd_or_register_len,
                      uint8_t *const data, const uint_fast16_t data_len)
{
    // char          error;
    // uint_fast16_t bytes_to_read = data_len;
    // uint8_t      *pdata         = data;

    // Transfer structure
    I2C_TransferSeq_TypeDef    i2cTransferData;
    I2C_TransferReturn_TypeDef result;
    uint8_t chipdata[2] = {(uint8_t)(cmd_or_register >> 8), (uint8_t)cmd_or_register};
    // Initialize I2C transfer
    i2cTransferData.addr  = (uint16_t)slave_address;
    i2cTransferData.flags = I2C_FLAG_WRITE_READ; // must write target address before reading
    if (cmd_or_register_len > 0u)
    {
        i2cTransferData.buf[0].data = (uint8_t *)(chipdata + (cmd_or_register_len & 1u));
        i2cTransferData.buf[0].len  = (uint16_t)cmd_or_register_len;

        i2cTransferData.buf[1].data = (uint8_t *)data;
        i2cTransferData.buf[1].len  = (uint16_t)data_len;
    }
    else
    {
        i2cTransferData.flags       = I2C_FLAG_READ;
        i2cTransferData.buf[0].data = (uint8_t *)data;
        i2cTransferData.buf[0].len  = (uint16_t)data_len;
    }

    result = I2C_TransferInit(p_handle, &i2cTransferData);

    // Send data
    while (result == i2cTransferInProgress)
    {
        result = I2C_Transfer(p_handle);
    }

    if (result != i2cTransferDone)
    {
        return 0u;
    }
    return data_len;

    if (data_len <= 0)
        return -1; /* no read was performed */

    return 0; /* no error */
}

/********************************************************************************
 * @brief  Enable (power on) or disable (power off) to the i2c devices
 * @param[in]      p_handle - pointer to the i2c device
 * @param[in]      turn_on - enable or disable the i2c devices (POWER_ON or
 *POWER_OFF)
 * @param[in]      settle_time_ms - number of milliseconds to wait for the i2c
 *                                  devices power on
 * @retval
 ********************************************************************************/
void i2c_hal_enable(const p_i2c_hal_t p_handle, const bool turn_on, const uint32_t settle_time_ms)
{
}

/**
 * @brief Register a callback to be invoked on GPIO interrupt events.
 * @param[in] handle Pointer to platform-specific context associated with the
 * callback.
 * @param callback Function to invoke when an interrupt fires. May be NULL to
 * clear the registration.
 *
 * @return 0 on success, negative value when registration fails.
 */
int i2c_hal_register_callback(const p_i2c_hal_t p_handle, p_i2c_callback_t callback,
                              void *p_callback_context)
{
    if (p_handle == NULL)
    {
        return -1; // Invalid handle
    }

    p_handle->callback.callback          = callback;
    p_handle->callback.p_callback_handle = p_callback_context;

    return 0; // Success
}

#if USE_I2C_INTERRUPTS == 1
/**
 * @brief  Handle an I2C interrupt
 * @retval None
 */
void i2c_hal_irqhandler(void);
#endif

static void flushRx(I2C_TypeDef *const i2c)
{
    uint32_t timeout = 10000u; // Timeout to prevent infinite loop
    while ((i2c->STATUS & I2C_STATUS_RXDATAV) && (--timeout > 0u))
    {
        i2c->RXDATA;
    }

    /* SW needs to clear RXDATAV IF on Series 2 devices.
       Flag is kept high by HW if buffer is not empty. */
    // I2C_IntClear(i2c, I2C_IF_RXDATAV);
    i2c->IF_CLR = I2C_IF_RXDATAV;
}

/** @endcond */

I2C_TransferReturn_TypeDef I2C_Transfer(p_i2c_hal_t p_handle)
{
    uint32_t                              tmp;
    uint32_t                              pending;
    static volatile I2C_Transfer_TypeDef *transfer; /*lint !e956*/
    const I2C_TransferSeq_TypeDef        *seq;
    bool                                  finished = false;

    // assert(I2C_REF_VALID(i2c));

    // transfer         = p_handle->transfer;
    I2C_TypeDef *i2c = p_handle->i2c_port;

    seq = p_handle->transfer->seq;
    while (!finished)
    {
        pending = i2c->IF;

        /* Check timeout to prevent infinite loop */
        if (p_handle->transfer->timeout == 0u)
        {
            p_handle->transfer->result = i2cTransferNack;
            p_handle->transfer->state  = i2cStateDone;
            i2c->CMD                   = I2C_CMD_STOP | I2C_CMD_ABORT;
            break;
        }
        p_handle->transfer->timeout--;

        /* If some sort of fault, abort transfer. */
        if (pending & I2C_IF_ERRORS)
        {
            if (pending & I2C_IF_ARBLOST)
            {
                /* If an arbitration fault, indicates either a slave device */
                /* not responding as expected, or other master which is not */
                /* supported by this software. */
                p_handle->transfer->result = i2cTransferArbLost;
            }
            else if (pending & I2C_IF_BUSERR)
            {
                /* A bus error indicates a misplaced start or stop, which should */
                /* not occur in master mode controlled by this software. */
                p_handle->transfer->result = i2cTransferBusErr;
            }

            /* Ifan error occurs, it is difficult to know */
            /* an exact cause and how to resolve. It will be up to a wrapper */
            /* to determine how to handle a fault/recovery if possible. */
            p_handle->transfer->state = i2cStateDone;
            break;
        }

        switch (p_handle->transfer->state)
        {
        /***************************************************/
        /* Send the first start+address (first byte if 10 bit). */
        /***************************************************/
        case i2cStateStartAddrSend:
            if (seq->flags & I2C_FLAG_10BIT_ADDR)
            {
                tmp = (((uint32_t)(seq->addr) >> 8u) & 0x06u) | 0xf0u;

                /* In 10 bit address mode, the address following the first */
                /* start always indicates write. */
            }
            else
            {
                tmp = (uint32_t)(seq->addr) & 0xfeu;

                if (seq->flags & I2C_FLAG_READ)
                {
                    /* Indicate read request */
                    tmp |= 1u;
                }
            }

            p_handle->transfer->state = i2cStateAddrWFAckNack;
            i2c->TXDATA               = tmp; /* Data not transmitted until the START is sent. */
            i2c->CMD                  = I2C_CMD_START; /*lint !e835*/
            break;

        /*******************************************************/
        /* Wait for ACK/NACK on the address (first byte if 10 bit). */
        /*******************************************************/
        case i2cStateAddrWFAckNack:
            if (pending & I2C_IF_NACK)
            {
                // I2C_IntClear(i2c, I2C_IF_NACK);
                i2c->IF_CLR                = I2C_IF_NACK;
                p_handle->transfer->result = i2cTransferNack;
                p_handle->transfer->state  = i2cStateWFStopSent;
                i2c->CMD                   = I2C_CMD_STOP;
                finished                   = true;
            }
            else if (pending & I2C_IF_ACK)
            {
                // I2C_IntClear(i2c, I2C_IF_ACK);
                i2c->IF_CLR = I2C_IF_ACK;

                /* If a 10 bit address, send the 2nd byte of the address. */
                if (seq->flags & I2C_FLAG_10BIT_ADDR)
                {
                    p_handle->transfer->state = i2cStateAddrWF2ndAckNack;
                    i2c->TXDATA               = (uint32_t)(seq->addr) & 0xffu;
                }
                else
                {
                    /* Determine whether receiving or sending data. */
                    if (seq->flags & I2C_FLAG_READ)
                    {
                        p_handle->transfer->state = i2cStateWFData;
                        if (seq->buf[transfer->bufIndx].len == 1u)
                        {
                            i2c->CMD = I2C_CMD_NACK;
                        }
                    }
                    else
                    {
                        p_handle->transfer->state = i2cStateDataSend;
                        continue;
                    }
                }
                finished = true;
            }
            break;

        /******************************************************/
        /* Wait for ACK/NACK on the second byte of a 10 bit address. */
        /******************************************************/
        case i2cStateAddrWF2ndAckNack:
            if (pending & I2C_IF_NACK)
            {
                // I2C_IntClear(i2c, I2C_IF_NACK);
                i2c->IF_CLR                = I2C_IF_NACK;
                p_handle->transfer->result = i2cTransferNack;
                p_handle->transfer->state  = i2cStateWFStopSent;
                i2c->CMD                   = I2C_CMD_STOP;
                finished                   = true;
            }
            else if (pending & I2C_IF_ACK)
            {
                // I2C_IntClear(i2c, I2C_IF_ACK);
                i2c->IF_CLR = I2C_IF_ACK;

                /* If using a plain read sequence with a 10 bit address, switch to send */
                /* a repeated start. */
                if (seq->flags & I2C_FLAG_READ)
                {
                    p_handle->transfer->state = i2cStateRStartAddrSend;
                }
                /* Otherwise, expected to write 0 or more bytes. */
                else
                {
                    p_handle->transfer->state = i2cStateDataSend;
                }
                continue;
            }
            break;

        /*******************************/
        /* Send a repeated start+address */
        /*******************************/
        case i2cStateRStartAddrSend:
            if (seq->flags & I2C_FLAG_10BIT_ADDR)
            {
                tmp = (uint32_t)((seq->addr >> 8) & 0x06u) | 0xf0u;
            }
            else
            {
                tmp = (uint32_t)(seq->addr & 0xfeu);
            }

            /* If this is a write+read combined sequence, read is about to start. */
            if (seq->flags & I2C_FLAG_WRITE_READ)
            {
                /* Indicate a read request. */
                tmp |= 1u;
                /* If reading only one byte, prepare the NACK now before START command. */
                if (seq->buf[transfer->bufIndx].len == 1u)
                {
                    i2c->CMD = I2C_CMD_NACK;
                }
            }

            p_handle->transfer->state = i2cStateRAddrWFAckNack;
            /* The START command has to be written first since repeated start. Otherwise, */
            /* data would be sent first. */
            i2c->CMD    = I2C_CMD_START; /*lint !e835*/
            i2c->TXDATA = tmp;

            break;

        /**********************************************************************/
        /* Wait for ACK/NACK on the repeated start+address (first byte if 10 bit) */
        /**********************************************************************/
        case i2cStateRAddrWFAckNack:
            if (pending & I2C_IF_NACK)
            {
                // I2C_IntClear(i2c, I2C_IF_NACK);
                i2c->IF_CLR                = I2C_IF_NACK;
                p_handle->transfer->result = i2cTransferNack;
                p_handle->transfer->state  = i2cStateWFStopSent;
                i2c->CMD                   = I2C_CMD_STOP;
                finished                   = true;
            }
            else if (pending & I2C_IF_ACK)
            {
                // I2C_IntClear(i2c, I2C_IF_ACK);
                i2c->IF_CLR = I2C_IF_ACK;

                /* Determine whether receiving or sending data. */
                if (seq->flags & I2C_FLAG_WRITE_READ)
                {
                    p_handle->transfer->state = i2cStateWFData;
                }
                else
                {
                    p_handle->transfer->state = i2cStateDataSend;
                    continue;
                }
                finished = true;
            }
            break;

        /*****************************/
        /* Send a data byte to the slave */
        /*****************************/
        case i2cStateDataSend:
            /* Reached end of data buffer. */
            if (p_handle->transfer->offset >= seq->buf[p_handle->transfer->bufIndx].len)
            {
                /* Move to the next message part. */
                p_handle->transfer->offset = 0u;
                p_handle->transfer->bufIndx++;

                /* Send a repeated start when switching to read mode on the 2nd buffer. */
                if (seq->flags & I2C_FLAG_WRITE_READ)
                {
                    p_handle->transfer->state = i2cStateRStartAddrSend;
                    continue;
                }

                /* Only writing from one buffer or finished both buffers. */
                if ((seq->flags & I2C_FLAG_WRITE) || (p_handle->transfer->bufIndx > 1u))
                {
                    p_handle->transfer->state = i2cStateWFStopSent;
                    i2c->CMD                  = I2C_CMD_STOP;
                    finished                  = true;
                    break;
                }

                /* Reprocess in case the next buffer is empty. */
                continue;
            }

            /* Send byte. */
            i2c->TXDATA               = (uint32_t)(seq->buf[p_handle->transfer->bufIndx]
                                         .data[p_handle->transfer->offset++]);
            p_handle->transfer->state = i2cStateDataWFAckNack;
            break;

        /*********************************************************/
        /* Wait for ACK/NACK from the slave after sending data to it. */
        /*********************************************************/
        case i2cStateDataWFAckNack:
            if (pending & I2C_IF_NACK)
            {
                // I2C_IntClear(i2c, I2C_IF_NACK);
                i2c->IF_CLR                = I2C_IF_NACK;
                p_handle->transfer->result = i2cTransferNack;
                p_handle->transfer->state  = i2cStateWFStopSent;
                i2c->CMD                   = I2C_CMD_STOP;
                finished                   = true;
            }
            else if (pending & I2C_IF_ACK)
            {
                // I2C_IntClear(i2c, I2C_IF_ACK);
                i2c->IF_CLR               = I2C_IF_ACK;
                p_handle->transfer->state = i2cStateDataSend;
                continue;
            }
            break;

        /****************************/
        /* Wait for data from slave */
        /****************************/
        case i2cStateWFData:
            if (pending & I2C_IF_RXDATAV)
            {
                uint8_t        data;
                const uint32_t rxLen = seq->buf[p_handle->transfer->bufIndx].len;

                /* Must read out data not to block further progress. */
                data = (uint8_t)(i2c->RXDATA);

#if (defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1) ||                                               \
     defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2) || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3))
                // Errata I2C_E303. I2C Fails to Indicate New Incoming Data.
                const uint32_t status = i2c->STATUS;
                // look for invalid RXDATAV = 0 and RXFULL = 1 condition
                if (((status & I2C_IF_RXDATAV) == 0u) &
                    ((status & I2C_IF_RXFULL) != 0u)) /*lint !e514 !e9130*/
                {
                    // Performing a dummy read of the RXFIFO (I2C_RXDATA).
                    // This restores the expected RXDATAV = 1 and RXFULL = 0 condition.
                    (void)i2c->RXDATA;
                    // The dummy read will also set the RXUFIF flag bit, which should be ignored and
                    // cleared. I2C_IntClear(i2c, I2C_IF_RXUF);
                    i2c->IF_CLR = I2C_IF_RXUF;
                }
#endif

                /* SW needs to clear RXDATAV IF on Series 2 devices.
                   Flag is kept high by HW if buffer is not empty. */
                // 2C_IntClear(i2c, I2C_IF_RXDATAV);
                i2c->IF_CLR = I2C_IF_RXDATAV;

                /* Make sure that there is no storing beyond the end of the buffer (just in case).
                 */
                if (p_handle->transfer->offset < rxLen)
                {
                    seq->buf[p_handle->transfer->bufIndx].data[p_handle->transfer->offset++] = data;
                }

                /* If all requested data is read, the sequence should end. */
                if (p_handle->transfer->offset >= rxLen)
                {
                    p_handle->transfer->state = i2cStateWFStopSent;
                    i2c->CMD                  = I2C_CMD_STOP;
                }
                else
                {
                    /* Send ACK and wait for the next byte. */
                    i2c->CMD = I2C_CMD_ACK;

                    if ((1u < rxLen) && (p_handle->transfer->offset == (rxLen - 1u)))
                    {
                        /* If receiving more than one byte and this is the next
                           to last byte, transmit the NACK now before receiving
                           the last byte. */
                        i2c->CMD = I2C_CMD_NACK;
                    }
                }
                finished = true;
            }
            break;

        /***********************************/
        /* Wait for STOP to have been sent */
        /***********************************/
        case i2cStateWFStopSent:
            if (pending & I2C_IF_MSTOP)
            {
                // I2C_IntClear(i2c, I2C_IF_MSTOP);
                i2c->IF_CLR               = I2C_IF_MSTOP;
                p_handle->transfer->state = i2cStateDone;
                finished                  = true;
            }
            break;

        /******************************/
        /* An unexpected state, software fault */
        /******************************/
        default:
            p_handle->transfer->result = i2cTransferSwFault;
            p_handle->transfer->state  = i2cStateDone;
            finished                   = true;
            break;
        } /*lint !e788*/
    }

    if (p_handle->transfer->state == i2cStateDone)
    {
        /* Disable interrupt sources when done. */
        i2c->IEN = 0u;

        /* Update the result unless a fault has already occurred. */
        if (p_handle->transfer->result == i2cTransferInProgress)
        {
            p_handle->transfer->result = i2cTransferDone;
        }
    }
    /* Until transfer is done, keep returning i2cTransferInProgress. */
    else
    {
        return i2cTransferInProgress;
    }

    return p_handle->transfer->result;
} /*lint !e952*/

I2C_TransferReturn_TypeDef I2C_TransferInit(p_i2c_hal_t p_handle, I2C_TransferSeq_TypeDef *seq)
{
    // static volatile I2C_Transfer_TypeDef *transfer; /*lint !e956*/

    assert(seq);

    // transfer         = p_handle->transfer;
    I2C_TypeDef *i2c = p_handle->i2c_port;

    /* Check if in a busy state. Since this software assumes a single master, */
    /* issue an abort. The BUSY state is normal after a reset. */
    if (i2c->STATE & I2C_STATE_BUSY) /*lint !e835*/
    {
        i2c->CMD = I2C_CMD_ABORT;
        /* Wait for abort to complete */
        uint32_t abort_timeout = 1000u;
        while ((i2c->STATE & I2C_STATE_BUSY) && (--abort_timeout > 0u))
        {
            /* Wait */
        }
        if (abort_timeout == 0u)
        {
            return i2cTransferBusErr; /* Bus stuck */
        }
    }

    /* Do not try to read 0 bytes. It is not */
    /* possible according to the I2C spec, since the slave will always start */
    /* sending the first byte ACK on an address. The read operation can */
    /* only be stopped by NACKing a received byte, i.e., minimum 1 byte. */
    if (((seq->flags & I2C_FLAG_READ) && !(seq->buf[0].len)) ||
        ((seq->flags & I2C_FLAG_WRITE_READ) && !(seq->buf[1].len))) /*lint !e9130 !e9117*/
    {
        return i2cTransferUsageFault;
    }

    /* Prepare for a transfer. */
    p_handle->transfer->state   = i2cStateStartAddrSend;
    p_handle->transfer->result  = i2cTransferInProgress;
    p_handle->transfer->offset  = 0u;
    p_handle->transfer->bufIndx = 0u;
    p_handle->transfer->seq     = seq;
    p_handle->transfer->timeout = 100000u; /* Timeout for polled mode */

    /* Ensure buffers are empty. */
    i2c->CMD = I2C_CMD_CLEARPC | I2C_CMD_CLEARTX;
    flushRx(i2c);

    /* Clear all pending interrupts prior to starting a transfer. */
    // I2C_IntClear(i2c, _I2C_IF_MASK);
    i2c->IF_CLR = _I2C_IF_MASK;
    /* Enable relevant interrupts. */
    /* Notice that the I2C interrupt must also be enabled in the NVIC, but */
    /* that is left for an additional driver wrapper. */
    i2c->IEN |= I2C_IEN_NACK | I2C_IEN_ACK | I2C_IEN_MSTOP | I2C_IEN_RXDATAV | I2C_IEN_ERRORS;

    /* Start a transfer. */
    return I2C_Transfer(p_handle);
} /*lint !e952*/

#endif /* USE_I2C */
