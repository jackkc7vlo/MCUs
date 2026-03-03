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

#include <clock_hal.h>
#include <device_i2c.h>
#include <device_iocon.h>
#include <device_syscon.h>
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
#define NUM_I2C_MODULES 1U /**< Number of I2C modules available on this MCU */

/********************************************************************************
 * Typedefs & Enums
 ********************************************************************************/

static bool i2c_initialized = false;

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
    uint32_t               in_use_count;  /*!< Indicates if this I2C instance is in use */
    bool                   enabled;       /**< true when hardware is powered on */
    uint32_t               i2c_module_id; /**< I2C peripheral index (0 – 3) */
    reg_i2c_t             *p_regs;        /**< Pointer to memory-mapped registers */
    p_gpio_hal_t           p_scl_gpio;
    p_gpio_hal_t           p_sda_gpio; /*lint !e551*/
    uint8_t                scl_pin;    /**< SCL pin number within the port */
    uint8_t                sda_pin;    /**< SDA pin number within the port */
    i2c_callback_context_t callback;   /**< Registered interrupt callbacks */
};

static i2c_hal_t i2c_hal_devices[NUM_I2C_DEVICES] = {0};

/********************************************************************************
 * I2C clock configuration
 *
 * The NHS3100 I2C block is always clocked from the 8 MHz SFRO.
 * SCLH = SCLL = I2C_PCLK_HZ / (2 * target_baud_hz)
 ********************************************************************************/
#define I2C_PCLK_HZ 8000000U
#define I2C_SPEED_100KHZ 100000U
#define I2C_SCLL_100KHZ (I2C_PCLK_HZ / (2U * I2C_SPEED_100KHZ)) /**< 40 counts = 100 kHz */

/** Maximum poll iterations before declaring a bus hang */
#define I2C_SI_TIMEOUT_CYCLES 200000U

/********************************************************************************
 * @brief  Poll the SI (interrupt) flag until it is set, then return the I2C
 *         status code.  Returns I2C_STAT_CODE_ERROR on timeout.
 ********************************************************************************/
static uint32_t i2c_wait_for_si(const reg_i2c_t *const p_regs)
{
    uint32_t timeout = I2C_SI_TIMEOUT_CYCLES;
    while ((p_regs->conset & I2C_CON_SI) == 0U)
    {
        if (--timeout == 0U)
        {
            return (uint32_t)I2C_STAT_CODE_ERROR;
        }
    }
    return (p_regs->stat & (uint32_t)I2C_STAT_CODE_BITMASK);
}

/********************************************************************************
 * @brief  Issue a STOP condition and release the bus.
 *
 * @details On the NXP LPC I2C engine STO must be set BEFORE SI is cleared,
 *          otherwise the engine resumes without knowing a STOP is wanted and
 *          may leave the bus in an undefined state.
 ********************************************************************************/
static void i2c_send_stop(reg_i2c_t *const p_regs)
{
    p_regs->conset = I2C_CON_STO;              /* request STOP           */
    p_regs->conclr = I2C_CON_SI | I2C_CON_STA; /* release engine to act  */
    /* STO is cleared automatically by hardware after the STOP is generated */
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
/********************************************************************************
 * @brief          Create an I2C device
 * @param[in]      i2c_board_id - I2C device ID
 * @param[in]      p_callback - Pointer to the callback function
 * @retval         Pointer to the I2C device
 ********************************************************************************/
p_i2c_hal_t i2c_hal_create_device(const uint32_t i2c_board_id, const uint32_t sda_port,
                                  const uint32_t sda_pin, const uint32_t scl_port,
                                  const uint32_t scl_pin, const p_i2c_callback_t p_callback)
{
    if (i2c_board_id >= NUM_I2C_DEVICES)
    {
        return NULL; // Invalid device ID
    }

    i2c_hal_t *p_device = &i2c_hal_devices[i2c_board_id];

    if (p_device->in_use_count)
    {
        return p_device; // Device already in use
    }

    /* The NHS3100 has one physical I2C module; all logical board IDs map to the
     * same hardware registers.  The array bounds check above (>= NUM_I2C_DEVICES)
     * is the only guard needed here. */

    p_device->in_use_count++;
    p_device->i2c_module_id              = i2c_board_id;
    p_device->p_regs                     = p_i2c; // Assign the base address of the I2C registers
    p_device->enabled                    = false;
    p_device->callback.callback          = p_callback;
    p_device->callback.p_callback_handle = NULL; // Set as needed
    p_device->callback.callback_context  = NULL; // Set as needed

    /* Create GPIO handles for SCL and SDA (resource allocation only) */
    p_device->p_scl_gpio = gpio_hal_create(scl_port);
    p_device->p_sda_gpio = gpio_hal_create(sda_port);
    if (p_device->p_scl_gpio == NULL || p_device->p_sda_gpio == NULL)
    {
        p_device->in_use_count--;
        return NULL;
    }

    /* Store pin numbers for enable/disable/cleanup */
    p_device->scl_pin = (uint8_t)scl_pin;
    p_device->sda_pin = (uint8_t)sda_pin;

    return p_device;
}

void i2c_hal_remove_device(p_i2c_hal_t p_handle)
{
    if (p_handle == NULL || p_handle->in_use_count == 0u)
    {
        return; // Nothing to remove
    }

    if (--p_handle->in_use_count == 0u)
    {
        /* Power down the hardware first */
        i2c_hal_enable(p_handle, false, 0U);

        /* Release GPIO handles */
        gpio_hal_remove(p_handle->p_scl_gpio);
        gpio_hal_remove(p_handle->p_sda_gpio);
        p_handle->p_scl_gpio = NULL;
        p_handle->p_sda_gpio = NULL;

        /* Clear the callback */
        p_handle->callback.callback          = NULL;
        p_handle->callback.p_callback_handle = NULL;
        p_handle->callback.callback_context  = NULL;
    }
}

/********************************************************************************
 * @brief          Write function for the I2C driver
 * @param[in]      p_handle - pointer to the i2c device
 * @param[in]      cmd_or_register - The register or command to write data to
 *(i.e. memory address)
 * @param[in]      cmd_or_register_len - Length of the register or command in
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
    if ((p_handle == NULL) || (!p_handle->enabled) || (data == NULL))
    {
        return 0U;
    }

    reg_i2c_t *p_regs        = p_handle->p_regs;
    uint32_t   bytes_written = 0U;
    uint32_t   status;
    bool       ok = true;

    /* ---- START ---- */
    p_regs->conset = I2C_CON_STA | I2C_CON_I2EN;
    p_regs->conclr = I2C_CON_SI;

    status = i2c_wait_for_si(p_regs);
    ok     = (status == 0x08U); /* START transmitted */

    /* ---- SLA+W ---- */
    if (ok)
    {
        p_regs->dat    = (uint8_t)((slave_address << 1U) & 0xFEU);
        p_regs->conclr = I2C_CON_STA | I2C_CON_SI;

        status = i2c_wait_for_si(p_regs);
        ok     = (status == 0x18U); /* SLA+W ACK */
    }

    /* ---- Register / command bytes (MSB first) ---- */
    if (ok)
    {
        for (uint32_t i = cmd_or_register_len; (i > 0U) && ok; i--)
        {
            p_regs->dat    = (uint8_t)((cmd_or_register >> (8U * (i - 1U))) & 0xFFU);
            p_regs->conclr = I2C_CON_SI;
            status         = i2c_wait_for_si(p_regs);
            ok             = (status == 0x28U); /* data ACK */
        }
    }

    /* ---- Data bytes ---- */
    if (ok)
    {
        for (uint_fast16_t i = 0U; (i < data_len) && ok; i++)
        {
            p_regs->dat    = data[i];
            p_regs->conclr = I2C_CON_SI;
            status         = i2c_wait_for_si(p_regs);
            ok             = (status == 0x28U);
            if (ok)
            {
                bytes_written++;
            }
        }
    }

    /* ---- STOP ---- */
    i2c_send_stop(p_regs);

    return bytes_written;
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
    if ((p_handle == NULL) || (!p_handle->enabled) || (data == NULL) || (data_len == 0U))
    {
        return 0U;
    }

    reg_i2c_t *p_regs     = p_handle->p_regs;
    uint32_t   bytes_read = 0U;
    uint32_t   status;
    bool       ok = true;

    /* ---- Optional write phase: START -> SLA+W -> register/command bytes ---- */
    if (cmd_or_register_len > 0U)
    {
        p_regs->conset = I2C_CON_STA | I2C_CON_I2EN;
        p_regs->conclr = I2C_CON_SI;

        status = i2c_wait_for_si(p_regs);
        ok     = (status == 0x08U); /* START transmitted */

        if (ok)
        {
            p_regs->dat    = (uint8_t)((slave_address << 1U) & 0xFEU);
            p_regs->conclr = I2C_CON_STA | I2C_CON_SI;

            status = i2c_wait_for_si(p_regs);
            ok     = (status == 0x18U); /* SLA+W ACK */
        }

        if (ok)
        {
            for (uint32_t i = cmd_or_register_len; (i > 0U) && ok; i--)
            {
                p_regs->dat    = (uint8_t)((cmd_or_register >> (8U * (i - 1U))) & 0xFFU);
                p_regs->conclr = I2C_CON_SI;
                status         = i2c_wait_for_si(p_regs);
                ok             = (status == 0x28U); /* data ACK */
            }
        }

        /* If write phase failed, stop now */
        if (!ok)
        {
            i2c_send_stop(p_regs);
            return 0U;
        }
    }

    /* ---- (Repeated) START then SLA+R ---- */
    p_regs->conset = I2C_CON_STA | I2C_CON_I2EN;
    p_regs->conclr = I2C_CON_SI;

    status = i2c_wait_for_si(p_regs);
    ok     = (status == 0x10U) || (status == 0x08U); /* repeated or first START */

    if (ok)
    {
        p_regs->dat    = (uint8_t)(((slave_address << 1U) & 0xFEU) | 0x01U);
        p_regs->conclr = I2C_CON_STA | I2C_CON_SI;

        status = i2c_wait_for_si(p_regs);
        ok     = (status == 0x40U); /* SLA+R ACK */
    }

    /* ---- Read data bytes ---- */
    if (ok)
    {
        for (uint_fast16_t i = 0U; i < data_len; i++)
        {
            if (i < (data_len - 1U))
            {
                /* Return ACK to continue reading */
                p_regs->conset = I2C_CON_AA;
                p_regs->conclr = I2C_CON_SI;
                status         = i2c_wait_for_si(p_regs);
                if (status != 0x50U) /* data received, ACK returned */
                {
                    break;
                }
            }
            else
            {
                /* Last byte: return NACK to signal end of transfer */
                p_regs->conclr = I2C_CON_AA | I2C_CON_SI;
                status         = i2c_wait_for_si(p_regs);
                if (status != 0x58U) /* data received, NACK returned */
                {
                    break;
                }
            }
            data[i] = (uint8_t)(p_regs->dat & 0xFFU);
            bytes_read++;
        }
    }

    /* ---- STOP ---- */
    i2c_send_stop(p_regs);

    return bytes_read;
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
    if (p_handle == NULL)
    {
        return;
    }

    if (turn_on)
    {
        /* Enable I2C peripheral and IOCON clocks in SYSCON */
        p_syscon->ahbclkctrl |= SYSCON_AHBCLKCTRL_I2C_BIT | SYSCON_AHBCLKCTRL_IOCON_BIT;

        /* De-assert the I2C peripheral reset */
        p_syscon->presetctrl |= SYSCON_PRESETCTRL_I2C_BIT;

        /* Ensure GPIO/IOCON clocks are enabled (gpio_hal_init sets SYSCON bits) */
        gpio_hal_init(p_handle->p_scl_gpio);
        gpio_hal_init(p_handle->p_sda_gpio);

        /* Configure SCL and SDA IOCON registers for I2C alt-function.
         * PIO0_4 and PIO0_5 are the fixed NHS3100 I2C pins; these indices are
         * hardware-defined and independent of the board-level GPIO pin numbers
         * in hw_config.h. */
        volatile uint32_t *p_scl_iocon = &p_iocon->pio0_0 + I2C_SCL_PIN;
        volatile uint32_t *p_sda_iocon = &p_iocon->pio0_0 + I2C_SDA_PIN;
        *p_scl_iocon                   = IOCON_FUNC_ALT1 | IOCON_I2C_MODE_STANDARD;
        *p_sda_iocon                   = IOCON_FUNC_ALT1 | IOCON_I2C_MODE_STANDARD;

        /* Set SCL clock for 100 kHz operation (PCLK = 8 MHz SFRO) */
        p_handle->p_regs->sclh = I2C_SCLL_100KHZ;
        p_handle->p_regs->scll = I2C_SCLL_100KHZ;

        /* Clear any stale control flags then enable the I2C interface */
        p_handle->p_regs->conclr = I2C_CON_FLAGS;
        p_handle->p_regs->conset = I2C_CON_I2EN;

        p_handle->enabled = true;

        if (settle_time_ms > 0U)
        {
            clock_hal_delay(settle_time_ms);
        }
    }
    else
    {
        /* Disable the I2C interface and clear all control flags */
        p_handle->p_regs->conclr =
            I2C_CON_I2EN | I2C_CON_AA | I2C_CON_STA | I2C_CON_SI | I2C_CON_STO;

        /* Return SCL and SDA pins to floating GPIO function */
        volatile uint32_t *p_scl_iocon = &p_iocon->pio0_0 + I2C_SCL_PIN;
        volatile uint32_t *p_sda_iocon = &p_iocon->pio0_0 + I2C_SDA_PIN;
        *p_scl_iocon                   = IOCON_FUNC_GPIO | IOCON_MODE_INACTIVE;
        *p_sda_iocon                   = IOCON_FUNC_GPIO | IOCON_MODE_INACTIVE;

        /* Disable the I2C peripheral clock */
        p_syscon->ahbclkctrl &= ~SYSCON_AHBCLKCTRL_I2C_BIT;

        p_handle->enabled = false;
    }
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

uint32_t i2c_hal_scan(const uint32_t i2c_board_id, const uint32_t sda_port, const uint32_t sda_pin,
                      const uint32_t scl_port, const uint32_t scl_pin, uint32_t last_found_address)
{
    p_i2c_hal_t p_handle =
        i2c_hal_create_device(i2c_board_id, sda_port, sda_pin, scl_port, scl_pin, NULL);
    if (p_handle == NULL)
    {
        return 0; // Failed to create I2C device
    }

    i2c_hal_enable(p_handle, true, 10U);

    reg_i2c_t *p_regs        = p_handle->p_regs;
    uint32_t   start_address = (last_found_address < 0x08U) ? 0x08U : (last_found_address + 1u);
    uint32_t   found_address = 0U;

    for (uint32_t addr = start_address; addr <= 0x77U; addr++)
    {
        /* Probe: START -> SLA+W -> check ACK/NACK -> STOP */
        p_regs->conset = I2C_CON_STA | I2C_CON_I2EN;
        p_regs->conclr = I2C_CON_SI;

        uint32_t status = i2c_wait_for_si(p_regs);
        if (status != 0x08U) /* START not transmitted */
        {
            i2c_send_stop(p_regs);
            continue;
        }

        /* Load 7-bit address + write bit into DAT, then release engine */
        p_regs->dat    = (uint8_t)((addr << 1U) & 0xFEU);
        p_regs->conclr = I2C_CON_STA | I2C_CON_SI;

        status = i2c_wait_for_si(p_regs);
        i2c_send_stop(p_regs);

        if (status == 0x18U) /* SLA+W transmitted, ACK received: device present */
        {
            found_address = addr;
            break;
        }
        /* 0x20 = SLA+W NACK (no device) -- try next address */
    }

    i2c_hal_remove_device(p_handle);
    return found_address;
}

#endif /* USE_I2C */
