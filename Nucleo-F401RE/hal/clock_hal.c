/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       clock_hal.c
 * @author     Jack Wilson
 * @brief
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-15-2023     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 ******************************************************************************
 * @attention
 *
 * <h2>Portions of this software <center>&copy; Copyright (c) 2017 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
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

/*lint -e528 -e534*/

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
// #include <stm32f4xx.h>
#include <assert.h>
#include <clock_hal.h>
// #include <device_dwt.h>
#include <device_rcc.h>
#include <device_regs.h>
#include <device_systick.h>
#include <stdint.h>
#include <stdlib.h>
    /*lint -esym(793,__*)*/
    /*lint -esym(793,HAL_FLASH*)*/
    /*lint -esym(793,SEGGER*)*/
    /*lint -e952 -e9117 -e835 -e717 -e845 -e818 -e838*/
    // HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
    // HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);
    /********************************************************************************
     * Defines
     ********************************************************************************/

    /*lint -e506 -e641*/
    static volatile uint32_t ms_counter = 0u;
    static volatile uint32_t delay_ms   = 0u;

    static volatile uint32_t SystemCoreClock = 16000000;

    /********************************************************************************
     * Functions forward declarations
     ********************************************************************************/

    /********************************************************************************
     * Functions
     ********************************************************************************/

    /**
     * @brief  Initialize the Systick clock.
     *
     * @retval none
     */
    static inline void SysTick_Init()
    {
        // Set SysTick to interrupt every ms
        SysTick->LOAD = 2000u - 1u; /* reload register - assuming running at 2MHz */
        SysTick->VAL  = 0u;         /* Load the SysTick Counter Value */
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk |
                        SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
    }

    void SysTick_Handler(void)
    {
        ms_counter++;
        if (delay_ms > 0)
        {
            delay_ms--;
        }
        // HAL_IncTick();
        // HAL_SYSTICK_IRQHandler();
    }

    /**
     * @brief  Initialize the clock before doing almost anything else.
     *
     * This function initializes the clock.  It should be called before any other
     * clock functions are called.
     *
     * @retval none
     */

    /*lint -e9059*/

    void clock_hal_init(void)
    {

        // 1. Enable MSI (already on, but safe)
        p_device_rcc->cr |= RCC_CR_MSION;
        while ((p_device_rcc->cr & RCC_CR_MSIRDY) == 0)
            ;
        // 2. Select MSIRANGE from RCC->CR (THIS IS THE KEY)
        p_device_rcc->cr |= RCC_CR_MSIRGSEL;

        // 3. Set MSI range to 16 MHz (range = 8)
        p_device_rcc->cr &= ~RCC_CR_MSIRANGE;
        p_device_rcc->cr |= RCC_CR_MSIRANGE_8; // 16 MHz

        // 4. Select MSI as system clock
        p_device_rcc->cfgr &= ~RCC_CFGR_SW;
        p_device_rcc->cfgr |= RCC_CFGR_SW_MSI;

        // Wait until MSI is system clock
        while ((p_device_rcc->cfgr & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI)
            ;

        // 6. Update CMSIS variable
        // SystemCoreClockUpdate();
        const uint32_t msec_ticks = SystemCoreClock / 1000u; // SystemCoreClock is in Hz / 1000 (msec) = ticks per msec

        // set up the systick timer tp irq every msec;
        SysTick->LOAD = msec_ticks - 1u; /* reload register - assuming running at 2MHz */
        SysTick->VAL  = 0u;              /* Load the SysTick Counter Value */
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk |
                        SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */

        /*
        RCC_OscInitTypeDef RCC_OscInitStruct;
        RCC_ClkInitTypeDef RCC_ClkInitStruct;

        // Enable Power Control clock
        rcc_power_clock_enable();

        // The voltage scaling allows optimizing the power consumption when the
        //   device is clocked below the maximum system frequency (see datasheet).
        //__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
        pwr_voltage_scaling_config(PWR_REG_VOLT_SCALE1);

        // Enable HSE Oscillator and activate PLL with HSE as source

        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;

        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        RCC_OscInitStruct.PLL.PLLM = 8U;
        RCC_OscInitStruct.PLL.PLLN = 336U;
        RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
        RCC_OscInitStruct.PLL.PLLQ = 7U;

        RCC_OscInitStruct.PLL.PLLN = 192;
        RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
        RCC_OscInitStruct.PLL.PLLQ = 4;
        // RCC_OscInitStruct.PLL.PLLR = 2;
        HAL_RCC_OscConfig(&RCC_OscInitStruct);

        // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
        //   clocks dividers
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                      RCC_CLOCKTYPE_PCLK2;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
        HAL_RCC_ClockConfig(&RCC_ClkInitStruct, 0x5u);

        // Start the DWT timer
        // Disable TRC
        CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
        // Enable TRC
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

        // Disable clock cycle counter
        DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
        // Enable  clock cycle counter
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // 0x00000001;

        // Reset the clock cycle counter value
        DWT->CYCCNT = 0u;

        // 3 NO OPERATION instructions
        __ASM volatile("NOP");
        __ASM volatile("NOP");
        __ASM volatile("NOP");
        */
    }

    /**
     * @brief Set the clock frequency.
     * @param[in]       frequency The clock frequency in Hz
     * @return          none
     */
    void clock_hal_set_clock_frequency(const uint32_t frequency)
    {
        (void)frequency;
        /* Divide-by-0 NOT allowed */
        assert(frequency > 0u);
    }

    /**
     * @brief Get the clock frequency.
     * @return          uint32_t The clock frequency in Hz
     */
    uint32_t clock_hal_get_freq(void)
    {
        return SystemCoreClock;
    }

    /**
     * @brief Get the timer input clock for APB1 timers (e.g. TIM2).
     * @note  This computes PCLK1 from CFGR PPRE1 and returns the timer input
     *        clock. On STM32 the timer clock is PCLK1 when APB prescaler == 1
     *        or PCLK1 * 2 when the APB prescaler is >= 2.
     */
    uint32_t clock_hal_get_timer_freq(void)
    {
        /* Decode PPRE1 (bits 10:8 of RCC_CFGR) */
        const uint32_t ppre1_bits = (p_device_rcc->cfgr & 0x00001C00U) >> 10U;
        uint32_t       ppre1_div  = 1U;

        /* encoding: 0..3 => HCLK not divided (div=1)
           4 => div2, 5 => div4, 6 => div8, 7 => div16 */
        if (ppre1_bits < 4U)
        {
            ppre1_div = 1U;
        }
        else
        {
            switch (ppre1_bits)
            {
            case 4U:
                ppre1_div = 2U;
                break;
            case 5U:
                ppre1_div = 4U;
                break;
            case 6U:
                ppre1_div = 8U;
                break;
            case 7U:
                ppre1_div = 16U;
                break;
            default:
                ppre1_div = 1U;
                break;
            }
        }

        uint32_t pclk1 = SystemCoreClock / ppre1_div;

        /* For APB prescalers >= 2 the timer input clock is PCLK1 * 2 */
        if (ppre1_div == 1U)
        {
            return pclk1;
        }
        else
        {
            return pclk1 * 2U;
        }
    }

    /*
    uint32_t clock_hal_get_pclk1_freq(void)
    {
        uint32_t tmp       = 0;
        uint32_t presc     = 0;
        uint32_t pllvco    = 0;
        uint32_t pllp      = 2;
        uint32_t pllsource = 0;
        uint32_t pllm      = 2;
        uint32_t sysclk;
        uint32_t hclk;

        // PLL_VCO = (HSE_VALUE or HSI_VALUE /PLLM)*PLLN //SYSCLK = PLL_VCO/PLLP
        pllsource = (p_device_rcc->pllcfgr & RCC_PLLCFGR_PLLSRC) >> 22;
        pllm      = p_device_rcc->pllcfgr & RCC_PLLCFGR_PLLM;
        if (pllsource != 0)
        {
            // HSE used as PLL clock source
            pllvco = (16000000 / pllm) * ((p_device_rcc->pllcfgr & RCC_PLLCFGR_PLLN) >> 6);
        }
        else
        {
            // HSI used as PLL clock source
            pllvco = (16000000 / pllm) * ((p_device_rcc->pllcfgr & RCC_PLLCFGR_PLLN) >> 6);
        }
        pllp   = (((p_device_rcc->pllcfgr & RCC_PLLCFGR_PLLP) >> 16) + 1) * 2;
        sysclk = pllvco / pllp;
        // get HCLK prescaler
        tmp   = p_device_rcc->cfgr & RCC_CFGR_HPRE;
        tmp   = tmp >> 4;
        presc = AHBPrescTable[tmp];
        // HCLK clock frequency
        hclk = sysclk >> presc;
        // get PCLK1 prescaler
        tmp   = RCC->CFGR & RCC_CFGR_PPRE1;
        tmp   = tmp >> 10;
        presc = AHBPrescTable[tmp];
        // PCLK1 clock frequency
        return (hclk >> presc);
    }
*/
    /**
     * @brief Delay for the specified number of microseconds.
     * @param[in]       ms The number of microseconds to delay
     * @return          none
     */
    void clock_hal_delay(const uint32_t ms)
    {
        delay_ms = ms;
        while (delay_ms > 0)
        {
        }

    } /*lint !e438 !e550*/

    uint32_t clock_hal_get_milliseconds(void)
    {
        return ms_counter;
    }

/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 *         The SystemCoreClock variable contains the core clock (HCLK), it can
 *         be used by the user application to setup the SysTick timer or configure
 *         other parameters.
 *
 * @note   Each time the core clock (HCLK) changes, this function must be called
 *         to update SystemCoreClock variable value. Otherwise, any configuration
 *         based on this variable will be incorrect.
 *
 * @note   - The system frequency computed by this function is not the real
 *           frequency in the chip. It is calculated based on the predefined
 *           constant and the selected clock source:
 *
 *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
 *
 *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
 *
 *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
 *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
 *
 *         (*) HSI_VALUE is a constant defined in stm32f4xx_hal_conf.h file (default value
 *             16 MHz) but the real value may vary depending on the variations
 *             in voltage and temperature.
 *
 *         (**) HSE_VALUE is a constant defined in stm32f4xx_hal_conf.h file (its value
 *              depends on the application requirements), user has to ensure that HSE_VALUE
 *              is same as the real frequency of the crystal used. Otherwise, this function
 *              may have wrong result.
 *
 *         - The result of this function could be not correct when using fractional
 *           value for HSE crystal.
 *
 * @param  None
 * @retval None
 */
#if 0
   void SystemCoreClockUpdate(void)
   {
       uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

       /  Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFGR & RCC_CFGR_SWS;

    switch (tmp)
    {
    case 0x00: /* HSI used as system clock source */
        SystemCoreClock = HSI_VALUE;
        break;
    case 0x04: /* HSE used as system clock source */
        SystemCoreClock = HSE_VALUE;
        break;
    case 0x08: /* PLL used as system clock source */

        /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
           SYSCLK = PLL_VCO / PLL_P
           */
        pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
        pllm      = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;

        if (pllsource != 0)
        {
            /* HSE used as PLL clock source */
            pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
        }
        else
        {
            /* HSI used as PLL clock source */
            pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
        }

        pllp            = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> 16) + 1) * 2;
        SystemCoreClock = pllvco / pllp;
        break;
    default:
        SystemCoreClock = HSI_VALUE;
        break;
    }
    /* Compute HCLK frequency --------------------------------------------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    /* HCLK frequency */
    SystemCoreClock >>= tmp;
}
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
