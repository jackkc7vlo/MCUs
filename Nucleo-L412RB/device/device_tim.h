/**
 * Minimal TIM register definitions for STM32L412 (TIM2)
 */
#ifndef DEVICE_TIM_H
#define DEVICE_TIM_H

#include "device_regs.h"
#include <stdint.h>

/* TIM2 base (from SVD) */
#define REG_TIM2_BASE (REG_PERIPH_BASE + 0x00000000U)

typedef struct
{
    volatile uint32_t CR1;   /* 0x00 */
    volatile uint32_t CR2;   /* 0x04 */
    volatile uint32_t SMCR;  /* 0x08 */
    volatile uint32_t DIER;  /* 0x0C */
    volatile uint32_t SR;    /* 0x10 */
    volatile uint32_t EGR;   /* 0x14 */
    volatile uint32_t CCMR1; /* 0x18 */
    volatile uint32_t CCMR2; /* 0x1C */
    volatile uint32_t CCER;  /* 0x20 */
    volatile uint32_t CNT;   /* 0x24 */
    volatile uint32_t PSC;   /* 0x28 */
    volatile uint32_t ARR;   /* 0x2C */
    volatile uint32_t RCR;   /* 0x30 */
    volatile uint32_t CCR1;  /* 0x34 */
    volatile uint32_t CCR2;  /* 0x38 */
    volatile uint32_t CCR3;  /* 0x3C */
    volatile uint32_t CCR4;  /* 0x40 */
    volatile uint32_t BDTR;  /* 0x44 */
    volatile uint32_t DCR;   /* 0x48 */
    volatile uint32_t DMAR;  /* 0x4C */
} reg_tim_t;

static reg_tim_t *const TIM2 = (reg_tim_t *const)(REG_TIM2_BASE);

/* Useful bit masks */
#define TIM_CR1_CEN (1U << 0U)
#define TIM_DIER_UIE (1U << 0U)
#define TIM_SR_UIF (1U << 0U)

#endif /* DEVICE_TIM_H */
