
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
 * @file spi_hal.c
 * @brief Generic SPI interface implementation.
 * @details Outlines the portable SPI abstraction used to configure and
 * interact with SPI peripherals on any supported platform.
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

#include "spi_hal.h"
#include "clock_hal.h"
#include "device_gpio.h"
#include "device_rcc.h"
#include "device_spi.h"
#include "gpio_hal.h"
#include <hw_config.h>
#include <stdbool.h> /*lint -e129*/
#include <stddef.h>
#include <stdint.h>

#if HW_CONFIG_SPI == 1u
/********************************************************************************
 * Defines
 ********************************************************************************/
#define SPI_HAL_MAX_BUS 4u

#define SPI_BUS_1 1u
#define SPI_BUS_2 2u
#define SPI_BUS_3 3u
#define SPI_BUS_4 4u

#define SPI_HAL_BAUD_1MHz 1000000u
#define SPI_HAL_BAUD_2MHz 2000000u
#define SPI_HAL_BAUD_30MHz 30000000u

/********************************************************************************
 * Typedefs & Enums
 ********************************************************************************/
static bool spi_initialized = false;

// support for SPI callbacks
typedef struct spi_callback_context_s
{                                       /**< The GPIO pin number */
    p_spi_callback_t callback;          /**< Registered interrupt callback */
    void            *p_callback_handle; /**< Pointer to callback context (i.e. Button handle)*/
    void            *callback_context;  /**< User callback context */
} spi_callback_context_t;

struct spi_hal
{
    uint32_t         in_use_count; /*!< Indicates if this SPI instance is in use */
    uint32_t         device_id;    /*!< bus id */
    bool             is_initialized;
    p_spi_callback_t p_callback;      /*!< SPI IRQ callback */
    reg_spi_t       *p_spi_bus;       /*!< Pointer to the SPI peripheral */
    void            *p_device_handle; /*!< SPI device configuration */
};

static spi_hal_t spi_hal_devices[SPI_HAL_MAX_BUS] = {0};

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
/********************************************************************************
 * @brief          Initialization function for the SPI driver
 * @retval         None
 ********************************************************************************/
void spi_hal_initialize(void)
{
    if (!spi_initialized)
    {
        // perform any necessary hardware initialization here
        spi_initialized = true;
    }
}

/********************************************************************************
 * @brief          Check if the SPI driver is initialized
 * @retval         true if initialized, false otherwise
 ********************************************************************************/
bool spi_hal_is_bus_initialized(void)
{
    return spi_initialized;
}

/********************************************************************************
 * @brief          Create an SPI device
 * @param[in]      spi_device_id - Which SPI device to use
 * @param[in]      spi_bus - Pointer to the SPI bus
 * @param[in]      p_callback - Pointer to the callback function
 * @retval         Pointer to the I2C device
 ********************************************************************************/
p_spi_hal_t spi_hal_create_device(const uint32_t spi_device_id, p_spi_callback_t p_callback)
{
    if (spi_device_id >= SPI_HAL_MAX_BUS)
    {
        return NULL; // Invalid device ID
    }

    spi_hal_t *p_device = &spi_hal_devices[spi_device_id];

    if (p_device->in_use_count)
    {
        return p_device; // Device already in use
    }

    p_device->in_use_count++;
    p_device->device_id      = spi_device_id;
    p_device->is_initialized = false;
    // p_device->p_spi_bus       = spi_bus;
    p_device->p_callback      = p_callback;
    p_device->p_device_handle = NULL; // Set as needed

    switch (spi_device_id)
    {
    case SPI_BUS_1: // PA5 - SCK, PA6 - MISO, PA7 - MOSI
        p_device->p_spi_bus = p_device_spi1;
        p_device_rcc->ahb1enr |= (RCC_AHB1ENR_GPIOAEN_BIT); // Enable GPIO A Clock

        // reset the PA5, PA6, PA7 Mode registers
        p_device_gpio_a->moder &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
        p_device_gpio_a->moder |= (GPIO_MODER_MODER5_1) | (GPIO_MODER_MODER6_1) |
                                  (GPIO_MODER_MODER7_1); // Alternate functions for PA5, PA6, PA7 and Output for PA9

        p_device_gpio_a->ospeedr |= (GPIO_OSPEEDR_OSPEED5) | (GPIO_OSPEEDR_OSPEED6) |
                                    (GPIO_OSPEEDR_OSPEED7) /*| (1u << 18u) */; // High Speed for PA5, PA6, PA7, PA9

        p_device_gpio_a->afr[0] |= (5u << 20u) | (5u << 24u) | (5u << 28u); // AF5(SPI1) for PA5, PA6, PA7

        p_device_rcc->apb2enr |= RCC_APB2ENR_SPI1EN_BIT; // Enable SPI1 CLock
        break;
    case SPI_BUS_2: // PB13 - SCK, PB14 - MISO, PB15 - MOSI
        p_device->p_spi_bus = p_device_spi2;
        p_device_rcc->ahb1enr |= (RCC_AHB1ENR_GPIOBEN_BIT); // Enable GPIO B Clock

        // reset the PB13, PB14, PB15 Mode registers
        p_device_gpio_b->moder &= ~(GPIO_MODER_MODER13 | GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
        p_device_gpio_b->moder |= (GPIO_MODER_MODER13_1) | (GPIO_MODER_MODER14_1) |
                                  (GPIO_MODER_MODER15_1); // Alternate functions for PB13, PB14, PB15 and Output for PA9

        p_device_gpio_b->ospeedr |= (GPIO_OSPEEDR_OSPEED13) | (GPIO_OSPEEDR_OSPEED14) |
                                    (GPIO_OSPEEDR_OSPEED15) /*| (1u << 18u) */; // High Speed for PB13, PB14, PB15

        p_device_gpio_b->afr[1] |= (5u << 20u) | (5u << 24u) | (5u << 28u); // AF5(SPI2) for PB13, PB14, PB15

        p_device_rcc->apb1enr |= RCC_APB1ENR_SPI2EN_BIT; // Enable SPI2 CLock
        break;
    case SPI_BUS_3: // PB3 - SCK, PB4 - MISO, PB5 - MOSI
        p_device->p_spi_bus = p_device_spi3;
        p_device_rcc->ahb1enr |= (RCC_AHB1ENR_GPIOBEN_BIT); // Enable GPIO B Clock

        // reset the PB3, PB4, PB5 Mode registers
        p_device_gpio_b->moder &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5);
        p_device_gpio_b->moder |= (GPIO_MODER_MODER3_1) | (GPIO_MODER_MODER4_1) |
                                  (GPIO_MODER_MODER5_1); // Alternate functions for PB3, PB4, PB5 and Output for PA9

        p_device_gpio_b->ospeedr |= (GPIO_OSPEEDR_OSPEED3) | (GPIO_OSPEEDR_OSPEED4) |
                                    (GPIO_OSPEEDR_OSPEED5) /*| (1u << 18u) */; // High Speed for PB3, PB4, PB5

        p_device_gpio_b->afr[0] |= (6u << 12u) | (6u << 16u) | (6u << 20u); // AF6(SPI3) for PB3, PB4, PB5

        p_device_rcc->apb1enr |= RCC_APB1ENR_SPI3EN_BIT; // Enable SPI3 CLock
        break;
    default:
        //        assert(false); /*lint !e506*/
        break;
    }

    /* Configure SPI as master with software slave management.
     * MSTR  = 1 : Master mode (generates SCK)
     * SSM   = 1 : Software slave management enabled
     * SSI   = 1 : Internal slave select high (prevents MODF fault)
     */
    p_device->p_spi_bus->cr1 |= (SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI);

    return p_device;
}

void spi_hal_remove_device(p_spi_hal_t p_handle)
{
    if (p_handle == NULL || p_handle->in_use_count == 0u)
    {
        return; // Nothing to remove
    }

    if (--p_handle->in_use_count == 0u)
    {
        /* Power down the hardware first */
        spi_hal_enable(p_handle, false);
    }
}

/********************************************************************************
 * @brief          Write function for the SPI driver
 * @param[in]      p_spi_device - pointer to the SPI device
 * @param[in]      data -  pointer to data to send (may be words depending on
 *MCU)
 * @param[in]      data_len - Length of data to write in bytes
 * @retval         Number of bytes written
 ********************************************************************************/
void spi_hal_write(const p_spi_hal_t p_spi_device, const uint8_t *const data, const uint16_t data_len)
{
    uint32_t                        i = 0u;
    uint8_t __attribute__((unused)) temp;
    /************** STEPS TO FOLLOW *****************
    1. Wait for the TXE bit to set in the Status Register
    2. Write the data to the Data Register
    3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
    4. Clear the Overrun flag by reading DR and SR
    ************************************************/

    while (i < data_len)
    {
        /*Wait until TXE is set*/
        while (!((p_spi_device->p_spi_bus->sr) & (SPI_SR_TXE)))
        {
        }; // wait for TXE bit to set -> This will indicate that the buffer is empty
        *(volatile uint8_t *)&p_spi_device->p_spi_bus->dr = data[i]; // load the data into the Data Register
        i++;
    }

    /*During discontinuous communications, there is a 2 APB clock period delay between the
    write operation to the SPI_DR register and BSY bit setting. As a consequence it is
    mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
    data.
    */
    while (!((p_spi_device->p_spi_bus->sr) & (SPI_SR_TXE)))
    {
    }; // wait for TXE bit to set -> This will indicate that the buffer is empty
    while (((p_spi_device->p_spi_bus->sr) & (SPI_SR_BSY)))
    {
    }; // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication

    //  Clear the Overrun flag by reading DR and SR
    temp = (uint8_t)p_spi_device->p_spi_bus->dr;
    temp = (uint8_t)p_spi_device->p_spi_bus->sr; /*lint !e838*/

    return; /*lint !e438*/
}

/********************************************************************************
 * @brief          Read function for the SPI driver
 * @param[in]      spi_device_id - SPI device ID
 * @param[in]      data -  pointer to data (in bytes!) to read
 * @param[in]      data_len - Length of data to read in bytes
 * @retval         Number of bytes read
 ********************************************************************************/
uint16_t spi_hal_read(const p_spi_hal_t p_spi_device, uint8_t *data, const uint16_t data_len)
{
    uint_fast16_t size = data_len;
    /************** STEPS TO FOLLOW *****************
    1. Wait for the BSY bit to reset in Status Register
    2. Send some Dummy data before reading the DATA
    3. Wait for the RXNE bit to Set in the status Register
    4. Read data from Data Register
    ************************************************/
    while (size)
    {
        while (((p_spi_device->p_spi_bus->sr) & (SPI_SR_BSY)))
        {
        }; // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
        *(volatile uint8_t *)&p_spi_device->p_spi_bus->dr = 0u; // send dummy data
        while (!((p_spi_device->p_spi_bus->sr) & (SPI_SR_RXNE)))
        {
        }; // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
        *data++ = (uint8_t)(p_spi_device->p_spi_bus->dr & 0xFFU); // read the data from the Data Register
        size--;
    }

    return data_len;
}

/********************************************************************************
 * @brief  Enable or disable SPI
 * @param[in]      turn_on - enable or disable the SPI devices
 * @retval         None
 ********************************************************************************/
void spi_hal_enable(const p_spi_hal_t p_spi_device, const bool turn_on)
{
    if (turn_on)
    {
        p_spi_device->p_spi_bus->cr1 |= (SPI_CR1_SPE); // SPE=1, Peripheral enabled
    }
    else
    {
        p_spi_device->p_spi_bus->cr1 &= ~(SPI_CR1_SPE); // SPE=0, Peripheral Disabled
    }
}

/********************************************************************************
 * @brief          Helper to get APB1 clock frequency (PCLK1)
 * @retval         PCLK1 frequency in Hz
 ********************************************************************************/
static uint32_t spi_get_pclk1_freq(void)
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

/********************************************************************************
 * @brief          Helper to get APB2 clock frequency (PCLK2)
 * @retval         PCLK2 frequency in Hz
 ********************************************************************************/
static uint32_t spi_get_pclk2_freq(void)
{
    /* PPRE2 is bits [15:13] of RCC_CFGR */
    const uint32_t ppre2_bits = (p_device_rcc->cfgr & 0x0000E000U) >> 13U;
    uint32_t       ppre2_div  = 1U;

    /* Encoding: 0xx => not divided, 100 => /2, 101 => /4, 110 => /8, 111 => /16 */
    if (ppre2_bits >= 4U)
    {
        ppre2_div = 1U << (ppre2_bits - 3U); /* 4->2, 5->4, 6->8, 7->16 */
    }

    return clock_hal_get_freq() / ppre2_div;
}

/********************************************************************************
 * @brief  Set or clear the SPI baud rate
 * @param[in]      p_spi_device - pointer to the SPI device
 * @param[in]      baud_rate - baud rate to set in Hz
 * @retval         None
 * @note           The actual baud rate will be <= the requested rate
 *                 SPI baud = f_PCLK / (2^(BR+1)), where BR is 0-7
 ********************************************************************************/
void spi_hal_set_baud(const p_spi_hal_t p_spi_device, const uint32_t baud_rate)
{
    uint32_t pclk_freq;
    uint32_t br_value = 0u;

    if (p_spi_device == NULL || baud_rate == 0u)
    {
        return;
    }

    /* Determine which APB bus this SPI is on */
    if ((p_spi_device->p_spi_bus == p_device_spi1) || (p_spi_device->p_spi_bus == p_device_spi4))
    {
        /* SPI1 and SPI4 are on APB2 */
        pclk_freq = spi_get_pclk2_freq();
    }
    else
    {
        /* SPI2 and SPI3 are on APB1 */
        pclk_freq = spi_get_pclk1_freq();
    }

    /* Calculate BR value: SPI_CLK = PCLK / (2^(BR+1))
     * Find the smallest BR such that PCLK/(2^(BR+1)) <= baud_rate
     * BR ranges from 0 to 7:
     *   0: /2, 1: /4, 2: /8, 3: /16, 4: /32, 5: /64, 6: /128, 7: /256
     */
    for (br_value = 0u; br_value < 7u; br_value++)
    {
        uint32_t divisor         = 2u << br_value; /* 2^(BR+1) */
        uint32_t actual_baudrate = pclk_freq / divisor;

        if (actual_baudrate <= baud_rate)
        {
            break;
        }
    }

    /* Disable SPI before changing baud rate */
    bool was_enabled = (p_spi_device->p_spi_bus->cr1 & SPI_CR1_SPE) != 0u;
    if (was_enabled)
    {
        p_spi_device->p_spi_bus->cr1 &= ~SPI_CR1_SPE;
    }

    /* Clear existing BR bits [5:3] and set new value */
    p_spi_device->p_spi_bus->cr1 &= ~SPI_CR1_BR;
    p_spi_device->p_spi_bus->cr1 |= (br_value << SPI_CR1_BR_Pos);

    /* Re-enable SPI if it was enabled before */
    if (was_enabled)
    {
        p_spi_device->p_spi_bus->cr1 |= SPI_CR1_SPE;
    }
}

#if USE_SPI_INTERRUPTS == 1
/**
 * @brief  Handle an I2C interrupt
 * @retval None
 */
void spi_hal_irqhandler(void);
#endif

#endif // HW_CONFIG_SPI
