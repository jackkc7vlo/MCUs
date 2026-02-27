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
#include <device_rcc.h>
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

/********************************************************************************
 * Typedefs & Enums
 ********************************************************************************/

static bool i2c_initialized = false;
/********************************************************************************
 * Defines
 ********************************************************************************/

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
    uint32_t               in_use_count; /*!< Indicates if this I2C instance is in use */
    uint32_t               board_id;     /**< I2C board id (0, 1, 2) */
    bool                   enabled;      /**< true when hardware is powered on */
    reg_i2c_t             *p_i2c_regs;   /**< Pointer to the I2C peripheral registers */
    p_gpio_hal_t           p_scl_gpio;
    p_gpio_hal_t           p_sda_gpio; /*lint !e551*/
    uint8_t                scl_pin;    /**< SCL pin number within the port */
    uint8_t                sda_pin;    /**< SDA pin number within the port */
    i2c_callback_context_t callback;   /**< Registered interrupt callbacks */
};

/********************************************************************************
 * Helper: map board ID to I2C peripheral registers
 ********************************************************************************/
static reg_i2c_t *get_i2c_registers(uint32_t board_id)
{
    switch (board_id)
    {
    case I2C_BOARD_ID_1:
        return p_device_i2c1;
    case I2C_BOARD_ID_2:
        return p_device_i2c2;
    case I2C_BOARD_ID_3:
        return p_device_i2c3;
    default:
        return NULL;
    }
}

/********************************************************************************
 * Helper: enable the APB1 clock for the selected I2C peripheral
 ********************************************************************************/
static void i2c_enable_clock(uint32_t board_id)
{
    switch (board_id)
    {
    case I2C_BOARD_ID_1:
        p_device_rcc->apb1enr |= RCC_APB1ENR_I2C1EN_BIT;
        break;
    case I2C_BOARD_ID_2:
        p_device_rcc->apb1enr |= RCC_APB1ENR_I2C2EN_BIT;
        break;
    case I2C_BOARD_ID_3:
        p_device_rcc->apb1enr |= RCC_APB1ENR_I2C3EN_BIT;
        break;
    default:
        break;
    }
}

/********************************************************************************
 * Helper: disable the APB1 clock for the selected I2C peripheral
 ********************************************************************************/
static void i2c_disable_clock(uint32_t board_id)
{
    switch (board_id)
    {
    case I2C_BOARD_ID_1:
        p_device_rcc->apb1enr &= ~RCC_APB1ENR_I2C1EN_BIT;
        break;
    case I2C_BOARD_ID_2:
        p_device_rcc->apb1enr &= ~RCC_APB1ENR_I2C2EN_BIT;
        break;
    case I2C_BOARD_ID_3:
        p_device_rcc->apb1enr &= ~RCC_APB1ENR_I2C3EN_BIT;
        break;
    default:
        break;
    }
}

/********************************************************************************
 * Helper: poll for a flag with timeout (returns true when flag is set)
 ********************************************************************************/
static bool i2c_wait_flag(volatile const uint32_t *p_reg, uint32_t flag, uint32_t timeout_ms)
{
    /* Simple busy-wait; for production use a tick-based timeout */
    volatile uint32_t count = timeout_ms * 1000U; /* rough SW loop */
    while (!((*p_reg) & flag))
    {
        if (--count == 0U)
        {
            return false;
        }
    }
    return true;
}

static i2c_hal_t i2c_hal_devices[NUM_I2C_DEVICES] = {0};

/********************************************************************************
 * @brief  Compute the APB1 peripheral clock (PCLK1) from HCLK and the PPRE1
 *         prescaler in RCC_CFGR.  This is local to the I2C HAL so that the
 *         portable clock_hal API is not modified.
 * @return PCLK1 frequency in Hz
 ********************************************************************************/
static uint32_t i2c_get_pclk1_freq(void)
{
    /* PPRE1 is bits [12:10] of RCC_CFGR */
    const uint32_t ppre1_bits = (p_device_rcc->cfgr & 0x00001C00U) >> 10U;
    uint32_t       ppre1_div  = 1U;

    /* Encoding: 0xx => not divided, 100 => /2, 101 => /4, 110 => /8, 111 => /16 */
    if (ppre1_bits >= 4U)
    {
        ppre1_div = 1U << (ppre1_bits - 3U); /* 4->2, 5->4, 6->8, 7->16 */
    }

    return clock_hal_get_freq() / ppre1_div;
}

uint32_t i2c_determine_speed(const uint32_t pclk, const uint32_t speed, const uint32_t duty_cycle)
{
    uint32_t i2c_speed;
    uint32_t fast;

    if (duty_cycle == I2C_DUTYCYCLE_2)
    {
        fast = (pclk / (speed * 3u));
    }
    else
    {
        fast = ((pclk / (speed * 25u)) | I2C_DUTYCYCLE_16_9);
    }
    if (speed <= 100000u)
    {
        if (((pclk / (speed << 1u)) & I2C_CCR_CCR) < 4u) /*lint !e835*/
        {
            i2c_speed = 4u;
        }
        else
        {
            i2c_speed = (pclk / (speed << 1u));
        }
    }
    else if ((fast & I2C_CCR_CCR) == 0u) /*lint !e835*/
    {
        i2c_speed = 1u;
    }
    else
    {
        i2c_speed = (fast | I2C_CCR_FS);
    }
    return i2c_speed;
}

static inline void i2c_hal_stop(reg_i2c_t *p_i2c) /*lint !e528*/
{
    /* Generate a STOP condition on the bus */
    p_i2c->cr1 |= I2C_CR1_STOP;

    /* Wait until the BUSY flag clears */
    i2c_wait_flag(&p_i2c->sr2, 0U, I2C_TIMEOUT_FLAG); /* sr2 BUSY will clear - polled below */
    volatile uint32_t count = I2C_TIMEOUT_FLAG * 1000U;
    while ((p_i2c->sr2 & I2C_SR2_BUSY) && (--count > 0U))
    {
        /* spin */
    }
}

static inline void i2c_hal_reset(reg_i2c_t *p_i2c)
{
    /* Software reset: assert SWRST, then deassert */
    p_i2c->cr1 |= I2C_CR1_SWRST;
    p_i2c->cr1 &= ~I2C_CR1_SWRST;
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
p_i2c_hal_t i2c_hal_create_device(const uint32_t i2c_board_id, const uint32_t sda_port, const uint32_t sda_pin,
                                  const uint32_t scl_port, const uint32_t scl_pin, const p_i2c_callback_t p_callback)
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

    p_device->in_use_count++;
    p_device->board_id                   = i2c_board_id;
    p_device->enabled                    = false;
    p_device->p_i2c_regs                 = get_i2c_registers(i2c_board_id);
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
uint32_t i2c_hal_write(const p_i2c_hal_t p_handle, const uint32_t slave_address, const uint32_t cmd_or_register,
                       const uint32_t cmd_or_register_len, const uint8_t *const data, const uint_fast16_t data_len)
{
    if (p_handle == NULL || p_handle->p_i2c_regs == NULL || !p_handle->enabled)
    {
        return (uint32_t)-1;
    }

    reg_i2c_t     *p_i2c          = p_handle->p_i2c_regs;
    uint_fast16_t  bytes_to_write = data_len;
    const uint8_t *pdata          = data;

    if (bytes_to_write == 0u && cmd_or_register_len == 0u)
    {
        return (uint32_t)-1; /* nothing to send */
    }

    /* Wait until the bus is free */
    {
        volatile uint32_t count = I2C_TIMEOUT_FLAG * 1000U;
        while ((p_i2c->sr2 & I2C_SR2_BUSY) && (--count > 0U))
        { /* spin */
        }
        if (count == 0U)
        {
            return (uint32_t)-1;
        }
    }

    /* 1. Generate START condition */
    p_i2c->cr1 |= I2C_CR1_START;
    if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_SB, I2C_TIMEOUT_FLAG))
    {
        i2c_hal_stop(p_i2c);
        return (uint32_t)-1;
    }

    /* 2. Send slave address (write: LSB = 0) */
    p_i2c->dr = (uint32_t)((slave_address << 1U) & 0xFEU);
    if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_ADDR, I2C_TIMEOUT_FLAG))
    {
        i2c_hal_stop(p_i2c);
        return (uint32_t)-1;
    }
    /* Clear ADDR flag by reading SR1 then SR2 */
    (void)p_i2c->sr1;
    (void)p_i2c->sr2;

    /* 3. Send register / command bytes (MSB first) */
    for (uint32_t i = 0U; i < cmd_or_register_len; i++)
    {
        if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_TXE, I2C_TIMEOUT_FLAG))
        {
            i2c_hal_stop(p_i2c);
            return (uint32_t)-1;
        }
        /* Send byte from most-significant down */
        uint32_t shift = 8U * (cmd_or_register_len - 1U - i);
        p_i2c->dr      = (cmd_or_register >> shift) & 0xFFU;
    }

    /* 4. Send data bytes */
    while (bytes_to_write > 0U)
    {
        if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_TXE, I2C_TIMEOUT_FLAG))
        {
            i2c_hal_stop(p_i2c);
            return (uint32_t)-1;
        }
        p_i2c->dr = (uint32_t)*pdata++;
        bytes_to_write--;
    }

    /* Wait for the last byte to finish transmitting */
    if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_BTF, I2C_TIMEOUT_FLAG))
    {
        i2c_hal_stop(p_i2c);
        return (uint32_t)-1;
    }

    /* 5. Generate STOP */
    i2c_hal_stop(p_i2c);

    return 0U; /* success */
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
uint32_t i2c_hal_read(const p_i2c_hal_t p_handle, const uint32_t slave_address, const uint32_t cmd_or_register,
                      const uint32_t cmd_or_register_len, uint8_t *const data, const uint_fast16_t data_len)
{
    if (p_handle == NULL || p_handle->p_i2c_regs == NULL || !p_handle->enabled || data == NULL)
    {
        return (uint32_t)-1;
    }

    reg_i2c_t    *p_i2c         = p_handle->p_i2c_regs;
    uint_fast16_t bytes_to_read = data_len;
    uint8_t      *pdata         = data;

    if (bytes_to_read == 0u)
    {
        return (uint32_t)-1;
    }

    /* Wait until the bus is free */
    {
        volatile uint32_t count = I2C_TIMEOUT_FLAG * 1000U;
        while ((p_i2c->sr2 & I2C_SR2_BUSY) && (--count > 0U))
        { /* spin */
        }
        if (count == 0U)
        {
            return (uint32_t)-1;
        }
    }

    /* Ensure ACK is enabled */
    p_i2c->cr1 |= I2C_CR1_ACK;

    /* ---- If a register/command needs to be sent first (combined write-then-read) ---- */
    if (cmd_or_register_len > 0U)
    {
        /* START */
        p_i2c->cr1 |= I2C_CR1_START;
        if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_SB, I2C_TIMEOUT_FLAG))
        {
            i2c_hal_stop(p_i2c);
            return (uint32_t)-1;
        }

        /* Send slave address (write) */
        p_i2c->dr = (uint32_t)((slave_address << 1U) & 0xFEU);
        if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_ADDR, I2C_TIMEOUT_FLAG))
        {
            i2c_hal_stop(p_i2c);
            return (uint32_t)-1;
        }
        (void)p_i2c->sr1;
        (void)p_i2c->sr2; /* clear ADDR */

        /* Send register / command bytes */
        for (uint32_t i = 0U; i < cmd_or_register_len; i++)
        {
            if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_TXE, I2C_TIMEOUT_FLAG))
            {
                i2c_hal_stop(p_i2c);
                return (uint32_t)-1;
            }
            uint32_t shift = 8U * (cmd_or_register_len - 1U - i);
            p_i2c->dr      = (cmd_or_register >> shift) & 0xFFU;
        }

        /* Wait for last byte */
        if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_BTF, I2C_TIMEOUT_FLAG))
        {
            i2c_hal_stop(p_i2c);
            return (uint32_t)-1;
        }
    }

    /* ---- Repeated START for the read phase ---- */
    p_i2c->cr1 |= I2C_CR1_START;
    if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_SB, I2C_TIMEOUT_FLAG))
    {
        i2c_hal_stop(p_i2c);
        return (uint32_t)-1;
    }

    /* Send slave address (read: LSB = 1) */
    p_i2c->dr = (uint32_t)((slave_address << 1U) | 0x01U);
    if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_ADDR, I2C_TIMEOUT_FLAG))
    {
        i2c_hal_stop(p_i2c);
        return (uint32_t)-1;
    }

    if (bytes_to_read == 1U)
    {
        /* For single-byte read: disable ACK before clearing ADDR, then STOP */
        p_i2c->cr1 &= ~I2C_CR1_ACK;
        (void)p_i2c->sr1;
        (void)p_i2c->sr2; /* clear ADDR */
        p_i2c->cr1 |= I2C_CR1_STOP;

        if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_RXNE, I2C_TIMEOUT_FLAG))
        {
            return (uint32_t)-1;
        }
        *pdata = (uint8_t)(p_i2c->dr & 0xFFU);
    }
    else
    {
        /* Clear ADDR to start reception */
        (void)p_i2c->sr1;
        (void)p_i2c->sr2;

        while (bytes_to_read > 0U)
        {
            if (bytes_to_read == 1U)
            {
                /* Last byte: NACK + STOP */
                p_i2c->cr1 &= ~I2C_CR1_ACK;
                p_i2c->cr1 |= I2C_CR1_STOP;
            }

            if (!i2c_wait_flag(&p_i2c->sr1, I2C_SR1_RXNE, I2C_TIMEOUT_FLAG))
            {
                i2c_hal_stop(p_i2c);
                return (uint32_t)-1;
            }

            *pdata++ = (uint8_t)(p_i2c->dr & 0xFFU);
            bytes_to_read--;
        }
    }

    /* Re-enable ACK for next transfer */
    p_i2c->cr1 |= I2C_CR1_ACK;

    return 0U; /* success */
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
    if (p_handle == NULL || p_handle->in_use_count == 0u)
    {
        return;
    }

    if (turn_on && !p_handle->enabled)
    {
        reg_i2c_t *p_i2c = p_handle->p_i2c_regs;

        /* Enable GPIO port clocks */
        gpio_hal_init(p_handle->p_scl_gpio);
        gpio_hal_init(p_handle->p_sda_gpio);

        /* Enable I2C peripheral clock */
        i2c_enable_clock(p_handle->board_id);

        /* Configure SCL pin: alternate function 4 (AF4 = I2C on F401), open-drain, pull-up */
        gpio_hal_pin_direction(p_handle->p_scl_gpio, p_handle->scl_pin, PIN_DIRECTION_ALT);
        gpio_hal_set_alt_function(p_handle->p_scl_gpio, p_handle->scl_pin, 4U);
        gpio_hal_pin_mode(p_handle->p_scl_gpio, p_handle->scl_pin, OPENDRAIN);
        gpio_hal_pin_mode(p_handle->p_scl_gpio, p_handle->scl_pin, PULLUP);
        gpio_hal_pin_speed(p_handle->p_scl_gpio, p_handle->scl_pin, FASTSPEED);

        /* Configure SDA pin: alternate function 4 (AF4), open-drain, pull-up */
        gpio_hal_pin_direction(p_handle->p_sda_gpio, p_handle->sda_pin, PIN_DIRECTION_ALT);
        gpio_hal_set_alt_function(p_handle->p_sda_gpio, p_handle->sda_pin, 4U);
        gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, OPENDRAIN);
        gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, PULLUP);
        gpio_hal_pin_speed(p_handle->p_sda_gpio, p_handle->sda_pin, FASTSPEED);

        /* --- I2C peripheral configuration (RM0368 §18.3) --- */

        /* 1. Disable the peripheral before configuration */
        p_i2c->cr1 &= ~I2C_CR1_PE;

        /* 2. Software reset to clear any error state */
        i2c_hal_reset(p_i2c);

        /* 3. Set peripheral clock frequency in CR2 (must equal APB1 freq in MHz) */
        uint32_t pclk1    = i2c_get_pclk1_freq();
        uint32_t freq_mhz = pclk1 / 1000000U;
        if (freq_mhz < 2U)
            freq_mhz = 2U;
        if (freq_mhz > 50U)
            freq_mhz = 50U;
        p_i2c->cr2 = (p_i2c->cr2 & ~I2C_CR2_FREQ_MASK) | (freq_mhz & I2C_CR2_FREQ_MASK);

        /* 4. Configure clock control register (standard mode 100 kHz) */
        uint32_t ccr_val = i2c_determine_speed(pclk1, I2C_DEFAULT_SPEED, I2C_DUTYCYCLE_2);
        p_i2c->ccr       = ccr_val;

        /* 5. Configure maximum rise time
         *    Standard mode: TRISE = freq_mhz + 1
         *    Fast mode:     TRISE = (freq_mhz * 300) / 1000 + 1  */
        if (I2C_DEFAULT_SPEED <= I2C_SPEED_STANDARD)
        {
            p_i2c->trise = freq_mhz + 1U;
        }
        else
        {
            p_i2c->trise = ((freq_mhz * 300U) / 1000U) + 1U;
        }

        /* 6. Enable ACK and enable the peripheral */
        p_i2c->cr1 |= I2C_CR1_ACK;
        p_i2c->cr1 |= I2C_CR1_PE;

        p_handle->enabled = true;

        /* Allow bus devices time to stabilize after power-on */
        if (settle_time_ms > 0u)
        {
            clock_hal_delay(settle_time_ms);
        }
    }
    else if (!turn_on && p_handle->enabled)
    {
        reg_i2c_t *p_i2c = p_handle->p_i2c_regs;

        /* Disable the I2C peripheral */
        p_i2c->cr1 &= ~I2C_CR1_PE;

        /* Reset SCL pin back to default (input, floating) */
        if (p_handle->p_scl_gpio != NULL)
        {
            gpio_hal_set_alt_function(p_handle->p_scl_gpio, p_handle->scl_pin, 0U);
            gpio_hal_pin_direction(p_handle->p_scl_gpio, p_handle->scl_pin, PIN_DIRECTION_INPUT);
            gpio_hal_pin_mode(p_handle->p_scl_gpio, p_handle->scl_pin, FLOAT);
        }

        /* Reset SDA pin back to default (input, floating) */
        if (p_handle->p_sda_gpio != NULL)
        {
            gpio_hal_set_alt_function(p_handle->p_sda_gpio, p_handle->sda_pin, 0U);
            gpio_hal_pin_direction(p_handle->p_sda_gpio, p_handle->sda_pin, PIN_DIRECTION_INPUT);
            gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, FLOAT);
        }

        /* Disable I2C peripheral clock */
        i2c_disable_clock(p_handle->board_id);

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
int i2c_hal_register_callback(const p_i2c_hal_t p_handle, p_i2c_callback_t callback, void *p_callback_context)
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
    p_i2c_hal_t p_handle = i2c_hal_create_device(i2c_board_id, sda_port, sda_pin, scl_port, scl_pin, NULL);
    if (p_handle == NULL)
    {
        return 0; // Failed to create I2C device
    }

    i2c_hal_enable(p_handle, true, 10U);

    uint8_t  chipdata[2];
    uint32_t cmd_data      = 0u;
    uint32_t start_address = (last_found_address < 0x08U) ? 0x08U : (last_found_address + 1u);
    for (uint32_t i = start_address; i <= 0x77U; i++)
    {
        if (i2c_hal_read(p_handle, i, cmd_data, 1u, chipdata, 1u) == 0u)
        {
            i2c_hal_remove_device(p_handle);
            return i; // Found a device at this address
        }
        clock_hal_delay(1U); // Small delay between scans
    }
    i2c_hal_remove_device(p_handle); // Clean up when no device found
    return 0;
}

#endif /* USE_I2C */
