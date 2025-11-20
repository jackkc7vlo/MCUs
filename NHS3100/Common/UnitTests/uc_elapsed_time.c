/*
 ********************************************************************************
 *                       MODULE TO MEASURE EXECUTION TIME
 ********************************************************************************
 */

#include <stdint.h>
#include <uc_elapsed_time.h>
// #include  <chip.h>
#if UNIT_TESTS == 1u

#ifdef CORE_M0PLUS
#include "cmsis.h"
#include "core_cm0plus.h"
#include "reg_hal.h"
#include <clock_hal.h>
#endif
#if defined(CORE_M33) || defined(CORE_M4)

// #include "em_device.h"
#endif

#if defined(__ESP32__)
#include <timer_u32.h>
#endif

#if defined(__ESP32S3__)
#include "esp_timer.h"
#endif

#include "clock_hal.h"

#if USE_PRINTF == 1u
#include <printf_hal.h>
#endif
extern uint32_t SystemCoreClock;
/*
 ********************************************************************************
 *                           CORTEX-M - DWT TIMER
 ********************************************************************************
 */

#define ARM_CM_DEMCR (*(uint32_t *)0xE000EDFC)
#define ARM_CM_DWT_CTRL (*(uint32_t *)0xE0001000)
#define ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004)
#define ARM_CM_DWT_LAR (*(uint32_t *)0xE0001FB0)

/*
 ********************************************************************************
 *                             Data Structure
 ********************************************************************************
 */

typedef struct elapsed_time
{
	uint32_t start;
	uint32_t current;
	uint32_t max;
	uint32_t min;
	uint32_t ticks_per_microsecond;
} UC_ELAPSED_TIME;

/*
 ********************************************************************************
 *                      STORAGE FOR ELAPSED TIME MEASUREMENTS
 ********************************************************************************
 */

static UC_ELAPSED_TIME uc_elapsed_time_tbl;

/*
 ********************************************************************************
 *                              MODULE INITIALIZATION
 *
 * Note(s): Must be called before any of the other functions in this module
 ********************************************************************************
 */

void uc_elapsed_time_init(void)
{

#if defined __ESP32__ || defined __ESP32S3__
#if USE_PRINTF == 1u
	printf_hal("Ticks %lu\n", clock_hal_get_freq());
#endif
	uc_elapsed_time_tbl.ticks_per_microsecond = clock_hal_get_freq();
#else
#ifndef CORE_M0PLUS
	/* no DWT on the ARM Cortex M0 */

	if (ARM_CM_DWT_CTRL != 0)
	{								 // See if DWT is available
		ARM_CM_DEMCR |= 1 << 24;	 // Set bit 24
		ARM_CM_DWT_LAR = 0xC5ACCE55; // Unlock access to DWT (ITM, etc.)registers
		ARM_CM_DWT_CYCCNT = 0;
		ARM_CM_DWT_CTRL |= 1 << 0;													 // Set bit 0
		uc_elapsed_time_tbl.ticks_per_microsecond = clock_hal_get_freq() / 1000000u; // 1000000 = microseconds/sec
																					 // clock_hal_get_freq() / 1000000; // 1000 / clock_hal_get_ns_per_tick();
	}

#else

	SysTick->CTRL = 0;
	// we can us systick instead

	uc_elapsed_time_tbl.ticks_per_microsecond = clock_hal_get_freq() / 1000000;

#endif
#endif
	uc_elapsed_time_clr();
}

/*
 ********************************************************************************
 *                  START THE MEASUREMENT OF A CODE SECTION
 ********************************************************************************
 */

void uc_elapsed_time_start()
{

#ifdef __ESP32__
	uc_elapsed_time_tbl.start = timer_u32();
#elif defined __ESP32S3__
	uc_elapsed_time_tbl.start = esp_timer_get_time();
#else
#ifndef CORE_M0PLUS
	uc_elapsed_time_tbl.start = ARM_CM_DWT_CYCCNT;
	/*SysTick->LOAD = 0xFFFFFF;
	SysTick->VAL = 0;
	SysTick->CTRL = 0x05;
	while (SysTick->VAL == 0)
		;
	uc_elapsed_time_tbl.start = SysTick->VAL; */
#else
	SysTick->LOAD = 0xFFFFFF;
	SysTick->VAL = 0;
	SysTick->CTRL = 0x05;
	while (SysTick->VAL == 0)
		;
	uc_elapsed_time_tbl.start = SysTick->VAL;
#endif
#endif /* __ESP32__ */
}

/*
 ********************************************************************************
 *           STOP THE MEASUREMENT OF A CODE SECTION AND COMPUTE STATS
 ********************************************************************************
 */

void uc_elapsed_time_stop()
{
	uint32_t stop;
	UC_ELAPSED_TIME *p_tbl;
#ifdef __ESP32__
	p_tbl = &uc_elapsed_time_tbl;
	stop = timer_u32();
	p_tbl->current = (stop - p_tbl->start);
#elif defined __ESP32S3__
	p_tbl = &uc_elapsed_time_tbl;
	stop = esp_timer_get_time();
	p_tbl->current = (stop - p_tbl->start);

#else
#ifndef CORE_M0PLUS
	stop = ARM_CM_DWT_CYCCNT;
	/*
	stop = SysTick->VAL;
	SysTick->CTRL = 0; */
#else
	stop = SysTick->VAL;
	SysTick->CTRL = 0;
#endif
	p_tbl = &uc_elapsed_time_tbl;
#ifndef CORE_M0PLUS
	p_tbl->current = (stop - p_tbl->start) / p_tbl->ticks_per_microsecond;
#else
	p_tbl->current = (p_tbl->start - stop) / p_tbl->ticks_per_microsecond;
#endif
#endif /* __ESP32__ */
	if (p_tbl->max < p_tbl->current)
	{
		p_tbl->max = p_tbl->current;
	}
	if (p_tbl->min > p_tbl->current)
	{
		p_tbl->min = p_tbl->current;
	}
}

/*
 ********************************************************************************
 *                      CLEAR THE MEASUREMENTS STATS
 ********************************************************************************
 */

void uc_elapsed_time_clr()
{
	UC_ELAPSED_TIME *p_tbl;

	p_tbl = &uc_elapsed_time_tbl;
	p_tbl->start = 0;
	p_tbl->current = 0;
	p_tbl->min = 0xFFFFFFFF;
	p_tbl->max = 0;
}

uint32_t uc_elapsed_time_value()
{
	UC_ELAPSED_TIME *p_tbl;
	p_tbl = &uc_elapsed_time_tbl;
	return (p_tbl->current);
}

#endif /* UNIT_TESTS */