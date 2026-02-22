/*lint -esym(793,__*)*/
/*lint -esym(793,SEGGER*)*/
// #include "device_irq.h"
#include "clock_hal.h"
#include "drv_button.h"
#include "drv_led.h"
#include "gpio_hal.h"
#include "timer_hal.h"
#include "i2c_hal.h"
#include <efm32pg22c200f512im40.h>
#include <stddef.h>
#include <stdint.h>
// #include "device_gpio.h"
// #include "device_iocon.h"
// #include "device_syscon.h"

#ifndef DEBUG_FAULT_TRACE
#define DEBUG_FAULT_TRACE 1
#endif

static p_led_handle_t led_handle   = NULL;
static p_timer_hal_t  timer_handle = NULL;
static p_i2c_hal_t   i2c_handle   = NULL;

#if DEBUG_FAULT_TRACE
volatile uint32_t smu_secure_if     = 0U;
volatile uint32_t smu_secure_ppufs  = 0U;
volatile uint32_t smu_secure_bmpufs = 0U;
volatile uint32_t smu_secure_count  = 0U;

volatile uint32_t hardfault_cfsr        = 0U;
volatile uint32_t hardfault_hfsr        = 0U;
volatile uint32_t hardfault_dfsr        = 0U;
volatile uint32_t hardfault_afsr        = 0U;
volatile uint32_t hardfault_bfar        = 0U;
volatile uint32_t hardfault_mmfar       = 0U;
volatile uint32_t hardfault_shcsr       = 0U;
volatile uint32_t hardfault_stacked_r0  = 0U;
volatile uint32_t hardfault_stacked_r1  = 0U;
volatile uint32_t hardfault_stacked_r2  = 0U;
volatile uint32_t hardfault_stacked_r3  = 0U;
volatile uint32_t hardfault_stacked_r12 = 0U;
volatile uint32_t hardfault_stacked_lr  = 0U;
volatile uint32_t hardfault_stacked_pc  = 0U;
volatile uint32_t hardfault_stacked_psr = 0U;
volatile uint32_t thread_control_before = 0U;
volatile uint32_t thread_control_after  = 0U;

void hardfault_c_handler(uint32_t *stacked_regs)
{
    hardfault_cfsr  = SCB->CFSR;
    hardfault_hfsr  = SCB->HFSR;
    hardfault_dfsr  = SCB->DFSR;
    hardfault_afsr  = SCB->AFSR;
    hardfault_bfar  = SCB->BFAR;
    hardfault_mmfar = SCB->MMFAR;
    hardfault_shcsr = SCB->SHCSR;

    hardfault_stacked_r0  = stacked_regs[0];
    hardfault_stacked_r1  = stacked_regs[1];
    hardfault_stacked_r2  = stacked_regs[2];
    hardfault_stacked_r3  = stacked_regs[3];
    hardfault_stacked_r12 = stacked_regs[4];
    hardfault_stacked_lr  = stacked_regs[5];
    hardfault_stacked_pc  = stacked_regs[6];
    hardfault_stacked_psr = stacked_regs[7];

    while (1)
    {
    }
}

__attribute__((naked)) void HardFault_Handler(void)
{
    __asm volatile("tst lr, #4\n"
                   "ite eq\n"
                   "mrseq r0, msp\n"
                   "mrsne r0, psp\n"
                   "b hardfault_c_handler\n");
}

__attribute__((naked)) static void request_privileged_mode(void)
{
    __asm volatile("svc 0\n"
                   "bx lr\n");
}

void SVC_Handler(void)
{
    uint32_t control = __get_CONTROL();
    __set_CONTROL(control & ~1UL);
    __ISB();
}

void SMU_SECURE_IRQHandler(void)
{
    uint32_t iflags = SMU_NS->IF & _SMU_IF_MASK;

    smu_secure_if     = iflags;
    smu_secure_ppufs  = SMU_NS->PPUFS;
    smu_secure_bmpufs = SMU_NS->BMPUFS;
    smu_secure_count++;

    SMU_NS->IF_CLR = iflags;
    NVIC_ClearPendingIRQ(SMU_SECURE_IRQn);
}
#endif

#if USE_BUTTON_GPIO == 1
void button_callback(button_state_t button_state)
{

    if (button_state == BUTTON_PRESSED)
    {
        drv_led_toggle(led_handle);
    }
}
#endif // USE_BUTTON_GPIO
/*
 * @brief
 *
 * @return int
 */
int main(void) /*lint !e970*/
{
#if DEBUG_FAULT_TRACE
    thread_control_before = __get_CONTROL();
    if ((thread_control_before & 1UL) != 0UL)
    {
        request_privileged_mode();
    }
    thread_control_after = __get_CONTROL();
#endif

    // Enable GPIO and IOCON clock
    clock_hal_init();
    timer_handle = timer_hal_create(1000U, false, NULL);
    i2c_handle   = i2c_hal_create_device(1u, I2C1_SDA_PORT, I2C1_SDA_PIN, I2C1_SCL_PORT, I2C1_SCL_PIN, NULL);
#if HW_CONFIG_GPIO == 1 && USE_LED_GPIO == 1
    led_handle = drv_led_create(LED_PORT, LED_PIN);
    if (led_handle != NULL)
    {
        drv_led_init(led_handle);
    }
#endif // HW_CONFIG_GPIO AND USE_LED_GPIO

#if HW_CONFIG_GPIO == 1 && USE_BUTTON_GPIO == 1

    p_button_handle_t p_button_handle = drv_button_create(BUTTON_PORT, BUTTON_PIN, button_callback);
    if (p_button_handle != NULL)
    {
        drv_button_init(p_button_handle);
    }
#endif // HW_CONFIG_GPIO AND USE_BUTTON_GPIO

    /* Enable global interrupts after all peripherals are initialized */
    __enable_irq();

    for (;;)
    {
        // clock_hal_delay(1000);
        // drv_led_toggle(led_handle);
#if HW_CONFIG_GPIO == 1
#if 0
        gpio_hal_toggle_state(gpio_handle, LED_PIN);

        bool button_state = drv_button_is_pressed(p_button_handle);
        if (button_state)

        {
            gpio_hal_set_state(gpio_handle, LED_PIN, true);
        }
        else
        {
            gpio_hal_set_state(gpio_handle, LED_PIN, false);
        }
#endif

#endif // HW_CONFIG_GPIO AND USE_LED_GPIO AND USE_BUTTON_GPIO
        timer_hal_enable(timer_handle, true);
        timer_hal_start(timer_handle);
        while (!timer_hal_get_overflow(timer_handle))
        {
            // Debug: Check if UF flag is being set (timer hardware working)
            volatile uint32_t timer_if  = TIMER0_NS->IF;
            volatile uint32_t timer_cnt = TIMER0_NS->CNT;
            volatile uint32_t timer_ien = TIMER0_NS->IEN;

            // Set breakpoint here and check:
            // - Is letimer_cnt counting down from 32768?
            // - Is letimer_if showing UF bit (0x01) set after 1 second?
            // - Is letimer_ien showing UF enabled (0x01)?

            (void)timer_if;
            (void)timer_cnt;
            (void)timer_ien; // wait for timer to expire
        }
        timer_hal_reset_count(timer_handle);
        // clock_hal_delay(1000U);
        drv_led_toggle(led_handle);
    }
}
