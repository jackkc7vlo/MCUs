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
        p_device_rcc->apb1enr1 |= RCC_APB1ENR1_I2C1EN_BIT;
        break;
    case I2C_BOARD_ID_2:
        p_device_rcc->apb1enr1 |= RCC_APB1ENR1_I2C2EN_BIT;
        break;
    case I2C_BOARD_ID_3:
        p_device_rcc->apb1enr1 |= RCC_APB1ENR1_I2C3EN_BIT;
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
        p_device_rcc->apb1enr1 &= ~RCC_APB1ENR1_I2C1EN_BIT;
        break;
    case I2C_BOARD_ID_2:
        p_device_rcc->apb1enr1 &= ~RCC_APB1ENR1_I2C2EN_BIT;
        break;
    case I2C_BOARD_ID_3:
        p_device_rcc->apb1enr1 &= ~RCC_APB1ENR1_I2C3EN_BIT;
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

/********************************************************************************
 * Helper: wait for flag to be cleared (returns true when flag is cleared)
 ********************************************************************************/
static bool i2c_wait_flag_clear(volatile const uint32_t *p_reg, uint32_t flag, uint32_t timeout_ms)
{
    volatile uint32_t count = timeout_ms * 1000U;
    while ((*p_reg) & flag)
    {
        if (--count == 0U)
        {
            return false;
        }
    }
    return true;
}

static inline void i2c_hal_stop(reg_i2c_t *p_i2c) /*lint !e528*/
{
    /* Generate a STOP condition on the bus */
    p_i2c->cr2 |= I2C_CR2_STOP;

    /* Wait until STOPF flag is set */
    i2c_wait_flag(&p_i2c->isr, I2C_ISR_STOPF, I2C_TIMEOUT_FLAG);

    /* Clear STOPF — ICR is write-to-clear; use direct write, not |= */
    p_i2c->icr = I2C_ICR_STOPCF;

    /* Wait until the BUSY flag clears */
    i2c_wait_flag_clear(&p_i2c->isr, I2C_ISR_BUSY, I2C_TIMEOUT_FLAG);
}

/********************************************************************************
 * Helper: flush error flags and generate STOP after a failed transfer
 ********************************************************************************/
static inline void i2c_hal_flush_errors(reg_i2c_t *p_i2c)
{
    /* Clear NACK flag if set — ICR is write-to-clear, use direct write */
    if (p_i2c->isr & I2C_ISR_NACKF)
    {
        p_i2c->icr = I2C_ICR_NACKCF;
    }

    /* Generate STOP to release the bus */
    i2c_hal_stop(p_i2c);
}

static inline void i2c_hal_reset(reg_i2c_t *p_i2c)
{
    /* Disable peripheral */
    p_i2c->cr1 &= ~I2C_CR1_PE;

    /* Small delay */
    volatile uint32_t delay = 1000U;
    while (delay--)
    {
    }

    /* Re-enable peripheral - this resets internal state */
    p_i2c->cr1 |= I2C_CR1_PE;
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
    /* PPRE1 is bits [10:8] of RCC_CFGR (RM0394 §6.4.3) */
    const uint32_t ppre1_bits = (p_device_rcc->cfgr & 0x00000700U) >> 8U;
    uint32_t       ppre1_div  = 1U;

    /* Encoding: 0xx => not divided, 100 => /2, 101 => /4, 110 => /8, 111 => /16 */
    if (ppre1_bits >= 4U)
    {
        ppre1_div = 1U << (ppre1_bits - 3U); /* 4->2, 5->4, 6->8, 7->16 */
    }

    return clock_hal_get_freq() / ppre1_div;
}

/********************************************************************************
 * @brief  Calculate TIMINGR value for STM32L4 I2C peripheral
 * @param  i2c_clk: I2C kernel clock frequency in Hz (PCLK1 by default)
 * @param  speed: Desired I2C speed in Hz (100kHz or 400kHz typically)
 * @return TIMINGR register value
 *
 * @note   PRESC is computed dynamically so that the prescaled clock period
 *         (t_PRESC) yields SCL counter values that satisfy the I2C timing
 *         specification at any supported I2C kernel clock frequency.
 *
 *         Standard mode (100 kHz): target t_PRESC ~ 250 ns
 *           t_LOW  = (SCLL+1) * t_PRESC >= 4.7 us
 *           t_HIGH = (SCLH+1) * t_PRESC >= 4.0 us
 *
 *         Fast mode (400 kHz): target t_PRESC ~ 125 ns
 *           t_LOW  = (SCLL+1) * t_PRESC >= 1.3 us
 *           t_HIGH = (SCLH+1) * t_PRESC >= 0.6 us
 ********************************************************************************/
static uint32_t i2c_calculate_timingr(uint32_t i2c_clk, uint32_t speed)
{
    uint32_t presc, scll, sclh, sdadel, scldel;
    uint32_t i2c_clk_mhz = i2c_clk / 1000000U;

    if (i2c_clk_mhz < 1U)
    {
        i2c_clk_mhz = 1U;
    }

    if (speed <= 100000U) /* Standard mode 100 kHz */
    {
        /* Target t_PRESC ~ 250 ns  =>  PRESC+1 = i2c_clk * 250ns = MHz / 4
         * SCLL/SCLH are then computed so t_LOW >= 4.7 us, t_HIGH >= 4.0 us */
        uint32_t target_div = (i2c_clk_mhz + 2U) / 4U; /* round(MHz / 4) */
        if (target_div < 1U)
        {
            target_div = 1U;
        }
        if (target_div > 16U)
        {
            target_div = 16U;
        }
        presc = target_div - 1U;

        /* Prescaled clock period in ns (integer approximation) */
        uint32_t t_presc_ns = ((presc + 1U) * 1000U) / i2c_clk_mhz;

        /* SCLL: t_LOW = (SCLL+1) * t_PRESC >= 4700 ns */
        scll = ((4700U + t_presc_ns - 1U) / t_presc_ns);
        if (scll > 0U)
        {
            scll -= 1U;
        }

        /* SCLH: t_HIGH = (SCLH+1) * t_PRESC >= 4000 ns */
        sclh = ((4000U + t_presc_ns - 1U) / t_presc_ns);
        if (sclh > 0U)
        {
            sclh -= 1U;
        }

        sdadel = 2U;
        scldel = 4U;
    }
    else /* Fast mode 400 kHz */
    {
        /* Target t_PRESC ~ 125 ns  =>  PRESC+1 = i2c_clk * 125ns = MHz / 8 */
        uint32_t target_div = (i2c_clk_mhz + 4U) / 8U;
        if (target_div < 1U)
        {
            target_div = 1U;
        }
        if (target_div > 16U)
        {
            target_div = 16U;
        }
        presc = target_div - 1U;

        uint32_t t_presc_ns = ((presc + 1U) * 1000U) / i2c_clk_mhz;

        /* SCLL: t_LOW = (SCLL+1) * t_PRESC >= 1300 ns */
        scll = ((1300U + t_presc_ns - 1U) / t_presc_ns);
        if (scll > 0U)
        {
            scll -= 1U;
        }

        /* SCLH: t_HIGH = (SCLH+1) * t_PRESC >= 600 ns */
        sclh = ((600U + t_presc_ns - 1U) / t_presc_ns);
        if (sclh > 0U)
        {
            sclh -= 1U;
        }

        sdadel = 1U;
        scldel = 3U;
    }

    /* Build TIMINGR register value:
     * [31:28] PRESC
     * [23:20] SCLDEL
     * [19:16] SDADEL
     * [15:8]  SCLH
     * [7:0]   SCLL
     */
    return ((presc & 0xFU) << 28U) | ((scldel & 0xFU) << 20U) | ((sdadel & 0xFU) << 16U) | ((sclh & 0xFFU) << 8U) |
           (scll & 0xFFU);
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

    reg_i2c_t    *p_i2c       = p_handle->p_i2c_regs;
    uint_fast16_t total_bytes = cmd_or_register_len + data_len;

    if (total_bytes == 0u)
    {
        return (uint32_t)-1; /* nothing to send */
    }

    /* Wait until the bus is free */
    if (!i2c_wait_flag_clear(&p_i2c->isr, I2C_ISR_BUSY, I2C_TIMEOUT_FLAG))
    {
        return (uint32_t)-1;
    }

    /* Setup transfer: slave address, number of bytes, write direction, AUTOEND.
     * Include START in the same write for an atomic configuration. */
    p_i2c->cr2 = ((slave_address & 0x7FU) << 1U) | /* Slave address */
                 ((total_bytes & 0xFFU) << 16U) |  /* NBYTES */
                 I2C_CR2_AUTOEND |                 /* Generate STOP automatically */
                 I2C_CR2_START;                    /* Generate START */

    /* Send command/register bytes first (MSB first) */
    for (uint32_t i = 0U; i < cmd_or_register_len; i++)
    {
        /* Wait for TXIS (Transmit Data Register Empty) */
        if (!i2c_wait_flag(&p_i2c->isr, I2C_ISR_TXIS, I2C_TIMEOUT_FLAG))
        {
            i2c_hal_flush_errors(p_i2c);
            return (uint32_t)-1;
        }
        /* Send byte from most-significant down */
        uint32_t shift = 8U * (cmd_or_register_len - 1U - i);
        p_i2c->txdr    = (cmd_or_register >> shift) & 0xFFU;
    }

    /* Send data bytes */
    for (uint_fast16_t i = 0U; i < data_len; i++)
    {
        /* Wait for TXIS */
        if (!i2c_wait_flag(&p_i2c->isr, I2C_ISR_TXIS, I2C_TIMEOUT_FLAG))
        {
            i2c_hal_flush_errors(p_i2c);
            return (uint32_t)-1;
        }
        p_i2c->txdr = data[i];
    }

    /* Wait for STOPF (AUTOEND generates STOP automatically) */
    if (!i2c_wait_flag(&p_i2c->isr, I2C_ISR_STOPF, I2C_TIMEOUT_FLAG))
    {
        return (uint32_t)-1;
    }

    /* Clear STOPF — ICR is write-to-clear, use direct write */
    p_i2c->icr = I2C_ICR_STOPCF;

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

    reg_i2c_t *p_i2c = p_handle->p_i2c_regs;

    if (data_len == 0u)
    {
        return (uint32_t)-1;
    }

    /* Wait until the bus is free */
    if (!i2c_wait_flag_clear(&p_i2c->isr, I2C_ISR_BUSY, I2C_TIMEOUT_FLAG))
    {
        return (uint32_t)-1;
    }

    /* ---- If a register/command needs to be sent first ---- */
    if (cmd_or_register_len > 0U)
    {
        /* Setup write transfer: no AUTOEND (we'll do repeated START).
         * Include START in the same write for an atomic configuration. */
        p_i2c->cr2 = ((slave_address & 0x7FU) << 1U) |        /* Slave address */
                     ((cmd_or_register_len & 0xFFU) << 16U) | /* NBYTES */
                     I2C_CR2_START;                           /* Generate START */

        /* Send register / command bytes (MSB first) */
        for (uint32_t i = 0U; i < cmd_or_register_len; i++)
        {
            if (!i2c_wait_flag(&p_i2c->isr, I2C_ISR_TXIS, I2C_TIMEOUT_FLAG))
            {
                i2c_hal_flush_errors(p_i2c);
                return (uint32_t)-1;
            }
            uint32_t shift = 8U * (cmd_or_register_len - 1U - i);
            p_i2c->txdr    = (cmd_or_register >> shift) & 0xFFU;
        }

        /* Wait for TC (Transfer Complete) before repeated START */
        if (!i2c_wait_flag(&p_i2c->isr, I2C_ISR_TC, I2C_TIMEOUT_FLAG))
        {
            return (uint32_t)-1;
        }
    }

    /* ---- Repeated START for the read phase (or initial START if no cmd) ---- */
    p_i2c->cr2 = ((slave_address & 0x7FU) << 1U) | /* Slave address */
                 ((data_len & 0xFFU) << 16U) |     /* NBYTES */
                 I2C_CR2_RD_WRN |                  /* Read direction */
                 I2C_CR2_AUTOEND |                 /* Generate STOP automatically */
                 I2C_CR2_START;                    /* Generate START (or repeated START) */

    /* Read data bytes */
    for (uint_fast16_t i = 0U; i < data_len; i++)
    {
        /* Wait for RXNE (Receive Data Register Not Empty) */
        if (!i2c_wait_flag(&p_i2c->isr, I2C_ISR_RXNE, I2C_TIMEOUT_FLAG))
        {
            i2c_hal_flush_errors(p_i2c);
            return (uint32_t)-1;
        }
        data[i] = (uint8_t)(p_i2c->rxdr & 0xFFU);
    }

    /* Wait for STOPF (AUTOEND generates STOP automatically) */
    if (!i2c_wait_flag(&p_i2c->isr, I2C_ISR_STOPF, I2C_TIMEOUT_FLAG))
    {
        return (uint32_t)-1;
    }

    /* Clear STOPF — ICR is write-to-clear, use direct write */
    p_i2c->icr = I2C_ICR_STOPCF;

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

        /* Read-back to ensure the clock is enabled before accessing the
         * peripheral registers.  The STM32 bus matrix requires at least
         * two AHB cycles between an RCC enable write and the first access
         * to the peripheral; reading back the enable register satisfies
         * this requirement (RM0394 §6.4.17 note). */
        (void)p_device_rcc->apb1enr1;
        __asm volatile("dsb" ::: "memory");

        /* ---- RCC reset pulse: force the I2C peripheral to a known state.
         * This clears every internal register and state machine, which is
         * essential after an interrupted debug session. ---- */
        {
            uint32_t rst_bit = 0U;
            switch (p_handle->board_id)
            {
            case I2C_BOARD_ID_1:
                rst_bit = RCC_APB1ENR1_I2C1EN_BIT;
                break;
            case I2C_BOARD_ID_2:
                rst_bit = RCC_APB1ENR1_I2C2EN_BIT;
                break;
            case I2C_BOARD_ID_3:
                rst_bit = RCC_APB1ENR1_I2C3EN_BIT;
                break;
            default:
                break;
            }
            p_device_rcc->apb1rstr1 |= rst_bit; /* assert reset   */
            __asm volatile("dsb" ::: "memory");
            p_device_rcc->apb1rstr1 &= ~rst_bit; /* release reset  */
            __asm volatile("dsb" ::: "memory");
        }

        /* ---- Bus recovery: free a stuck SDA line ----
         * If a previous transfer was interrupted (e.g. by debugger break),
         * a slave may still be holding SDA low.  The standard recovery is
         * to toggle SCL up to 9 times until SDA releases, then issue a
         * manual STOP (SDA low → high while SCL is high).
         */
        {
            /* 1. Make sure PE is off so the I2C doesn't drive the pins */
            p_i2c->cr1 &= ~I2C_CR1_PE;

            /* 2. Configure SCL as output open-drain + pull-up */
            gpio_hal_pin_direction(p_handle->p_scl_gpio, p_handle->scl_pin, PIN_DIRECTION_OUTPUT);
            gpio_hal_pin_mode(p_handle->p_scl_gpio, p_handle->scl_pin, OPENDRAIN);
            gpio_hal_pin_mode(p_handle->p_scl_gpio, p_handle->scl_pin, PULLUP);
            gpio_hal_pin_speed(p_handle->p_scl_gpio, p_handle->scl_pin, FASTSPEED);

            /* 3. Configure SDA as input + pull-up so we can read it */
            gpio_hal_pin_direction(p_handle->p_sda_gpio, p_handle->sda_pin, PIN_DIRECTION_INPUT);
            gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, PULLUP);

            /* Drive SCL high initially */
            gpio_hal_set_state(p_handle->p_scl_gpio, p_handle->scl_pin, true);

            /* 4. Toggle SCL up to 9 times to clock out any stuck byte */
            for (uint32_t clk = 0U; clk < 9U; clk++)
            {
                /* Check SDA — read IDR directly (gpio_hal_get_state only
                   reads IDR for INPUT mode, which we've set above) */
                if (gpio_hal_get_state(p_handle->p_sda_gpio, p_handle->sda_pin))
                {
                    break; /* SDA released — bus is free */
                }
                /* SCL low */
                gpio_hal_set_state(p_handle->p_scl_gpio, p_handle->scl_pin, false);
                volatile uint32_t d = 200U;
                while (d--)
                {
                }
                /* SCL high */
                gpio_hal_set_state(p_handle->p_scl_gpio, p_handle->scl_pin, true);
                d = 200U;
                while (d--)
                {
                }
            }

            /* 5. Generate manual STOP:  SDA low → high while SCL is high */
            gpio_hal_pin_direction(p_handle->p_sda_gpio, p_handle->sda_pin, PIN_DIRECTION_OUTPUT);
            gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, OPENDRAIN);
            gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, PULLUP);

            gpio_hal_set_state(p_handle->p_sda_gpio, p_handle->sda_pin, false); /* SDA low  */
            volatile uint32_t d = 200U;
            while (d--)
            {
            }
            gpio_hal_set_state(p_handle->p_scl_gpio, p_handle->scl_pin, true); /* SCL high */
            d = 200U;
            while (d--)
            {
            }
            gpio_hal_set_state(p_handle->p_sda_gpio, p_handle->sda_pin, true); /* SDA high → STOP */
            d = 200U;
            while (d--)
            {
            }
        }

        /* Configure SCL pin: set AFR BEFORE switching MODER to ALT to
         * avoid glitching the pin through the wrong alternate function. */
        gpio_hal_set_alt_function(p_handle->p_scl_gpio, p_handle->scl_pin, 4U);
        gpio_hal_pin_mode(p_handle->p_scl_gpio, p_handle->scl_pin, OPENDRAIN);
        gpio_hal_pin_mode(p_handle->p_scl_gpio, p_handle->scl_pin, PULLUP);
        gpio_hal_pin_speed(p_handle->p_scl_gpio, p_handle->scl_pin, FASTSPEED);
        gpio_hal_pin_direction(p_handle->p_scl_gpio, p_handle->scl_pin, PIN_DIRECTION_ALT);

        /* Configure SDA pin: same order — AFR first, then MODER */
        gpio_hal_set_alt_function(p_handle->p_sda_gpio, p_handle->sda_pin, 4U);
        gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, OPENDRAIN);
        gpio_hal_pin_mode(p_handle->p_sda_gpio, p_handle->sda_pin, PULLUP);
        gpio_hal_pin_speed(p_handle->p_sda_gpio, p_handle->sda_pin, FASTSPEED);
        gpio_hal_pin_direction(p_handle->p_sda_gpio, p_handle->sda_pin, PIN_DIRECTION_ALT);

        /* --- I2C peripheral configuration for STM32L4 --- */

        /* 1. Ensure PE is cleared (it should be after the RCC reset,
         *    but be explicit).  Write 0 to clear all of CR1.         */
        p_i2c->cr1 = 0U;

        /* 2. Calculate and set the TIMINGR register (PE must be 0) */
        uint32_t pclk1       = i2c_get_pclk1_freq();
        uint32_t timingr_val = i2c_calculate_timingr(pclk1, I2C_DEFAULT_SPEED);
        p_i2c->timingr       = timingr_val;

        /* 3. Clear CR2 to ensure no stale address / NBYTES / flags */
        p_i2c->cr2 = 0U;

        /* 4. Clear any pending flags */
        p_i2c->icr = I2C_ICR_NACKCF | I2C_ICR_STOPCF;

        /* 5. Enable the peripheral */
        p_i2c->cr1 = I2C_CR1_PE;

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

    reg_i2c_t *p_i2c         = p_handle->p_i2c_regs;
    uint32_t   start_address = (last_found_address < 0x08U) ? 0x08U : (last_found_address + 1u);

    /* Clear any stale flags before scanning */
    p_i2c->icr = I2C_ICR_NACKCF | I2C_ICR_STOPCF;

    for (uint32_t i = start_address; i <= 0x77U; i++)
    {
        /* Wait until the bus is free */
        if (!i2c_wait_flag_clear(&p_i2c->isr, I2C_ISR_BUSY, I2C_TIMEOUT_FLAG))
        {
            i2c_hal_flush_errors(p_i2c);
            continue;
        }

        /* 1-byte read probe: START + address(R), NBYTES=1, AUTOEND.
         *   ACK  → slave sends 1 byte → auto-STOP → STOPF
         *   NACK → auto-STOP → NACKF + STOPF
         * (NBYTES=0 has known errata on some STM32L4 revisions, so we use 1.)
         */
        p_i2c->cr2 = ((i & 0x7FU) << 1U) | /* address */
                     (1U << 16U) |         /* NBYTES = 1 */
                     I2C_CR2_RD_WRN |      /* read direction */
                     I2C_CR2_AUTOEND |     /* auto-STOP */
                     I2C_CR2_START;        /* generate START */

        /* Wait for STOPF (arrives for both ACK and NACK paths) */
        if (!i2c_wait_flag(&p_i2c->isr, I2C_ISR_STOPF, I2C_TIMEOUT_FLAG))
        {
            /* Timeout – flush and move on */
            i2c_hal_flush_errors(p_i2c);
            continue;
        }

        /* Check if device ACK'd the address (NACKF clear means ACK) */
        bool found = !(p_i2c->isr & I2C_ISR_NACKF);

        /* If a byte was received, flush RXDR to clear RXNE */
        if (p_i2c->isr & I2C_ISR_RXNE)
        {
            (void)p_i2c->rxdr;
        }

        /* Clear flags – ICR is write-to-clear; always use direct write */
        p_i2c->icr = I2C_ICR_NACKCF | I2C_ICR_STOPCF;

        if (found)
        {
            i2c_hal_remove_device(p_handle);
            return i; // Found a device at this address
        }
    }
    i2c_hal_remove_device(p_handle); // Clean up when no device found
    return 0;
}

#endif /* USE_I2C */
