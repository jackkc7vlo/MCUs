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
 * @file device_irq.h
 * @brief  Defines registers used for Interrupts and NVIC on Arm devices
 * @details Outlines the portable device register abstraction used to configure and interact with
 * hardware peripherals on Arm devices
 *
 *
 * @author  Jack Wilson
 * @date    January 26, 2026
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2026 Jack Wilson
 * @license MIT License
 */

#ifndef DEVICE_IRQ_H
#define DEVICE_IRQ_H

#include <stdint.h>

// lint -esym(528,*) There are definitions they may not be used
// lint -esym(750,*) There are definitions they may not be used
// lint -esym(751,*) There are definitions they may not be used
// lint -esym(754,*) There are definitions they may not be used
// lint -esym(749,*) There are definitions they may not be used

// NOTE:  This does not define every possible register or bit
// More can be added from the datasheet

typedef enum
{

    /******  Cortex-M0 Processor Exceptions Numbers
     *************************************************/
    Reset_IRQn          = -15, /*!< 1 Reset Vector, invoked on Power up and warm reset */
    NonMaskableInt_IRQn = -14, /*!< 2 Non Maskable Interrupt */
    HardFault_IRQn      = -13, /*!< 3 Cortex-M0+ Hard Fault Interrupt */
    SVCall_IRQn         = -5,  /*!< 11 Cortex-M0+ SV Call Interrupt */
    PendSV_IRQn         = -2,  /*!< 14 Cortex-M0+ Pend SV Interrupt */
    SysTick_IRQn        = -1,  /*!< 15 Cortex-M0+ System Tick Interrupt */
    /******  Interrupt Numbers
     **********************************************************************/
    GPIOA_IRQn      = 0,   /*!< GPIO Port A                    */
    GPIOB_IRQn      = 1,   /*!< GPIO Port B                    */
    GPIOC_IRQn      = 2,   /*!< GPIO Port C                    */
    GPIOD_IRQn      = 3,   /*!< GPIO Port D                    */
    GPIOE_IRQn      = 4,   /*!< GPIO Port E                    */
    UART0_IRQn      = 5,   /*!< UART0 Rx and Tx                */
    UART1_IRQn      = 6,   /*!< UART1 Rx and Tx                */
    SSI0_IRQn       = 7,   /*!< SSI0 Rx and Tx                 */
    I2C0_IRQn       = 8,   /*!< I2C0 Master and Slave          */
    PWMFAULT_IRQn   = 9,   /*!< PWM Fault                      */
    PWMGEN0_IRQn    = 10,  /*!< PWM Generator 0                */
    PWMGEN1_IRQn    = 11,  /*!< PWM Generator 1                */
    PWMGEN2_IRQn    = 12,  /*!< PWM Generator 2                */
    QUAD_IRQn       = 13,  /*!< Quadrature Encoder 0           */
    ADC_SEQ0_IRQn   = 14,  /*!< ADC Sequence 0                 */
    ADC_SEQ1_IRQn   = 15,  /*!< ADC Sequence 1                 */
    ADC_SEQ2_IRQn   = 16,  /*!< ADC Sequence 2                 */
    ADC_SEQ3_IRQn   = 17,  /*!< ADC Sequence 3                 */
    WDT_IRQn        = 18,  /*!< Watchdog timer                 */
    TIM0_A_IRQn     = 19,  /*!< Timer 0 subtimer A             */
    TIM0_B_IRQn     = 20,  /*!< Timer 0 subtimer B             */
    TIM1_A_IRQn     = 21,  /*!< Timer 1 subtimer A             */
    TIM1_B_IRQn     = 22,  /*!< Timer 1 subtimer B             */
    TIM2_A_IRQn     = 23,  /*!< Timer 2 subtimer A             */
    TIM2_B_IRQn     = 24,  /*!< Timer 2 subtimer B             */
    ADC0_IRQn       = 25,  /*!< Analog Comparator 0            */
    ADC1_IRQn       = 26,  /*!< Analog Comparator 1            */
    ADC2_IRQn       = 27,  /*!< Analog Comparator 2            */
    SYSCTL_IRQn     = 28,  /*!< System Control (PLL, OSC, BO)  */
    FLASH_IRQn      = 29,  /*!< FLASH Control                  */
    GPIOF_IRQn      = 30,  /*!< GPIO Port F                    */
    GPIOG_IRQn      = 31,  /*!< GPIO Port G                    */
    GPIOH_IRQn      = 32,  /*!< GPIO Port H                    */
    UART2_IRQn      = 33,  /*!< UART2 Rx and Tx                */
    SSI1_IRQn       = 34,  /*!< SSI1 Rx and Tx                 */
    TIM3_A_IRQn     = 35,  /*!< Timer 3 subtimer A             */
    TIM3_B_IRQn     = 36,  /*!< Timer 3 subtimer B             */
    I2C1_IRQn       = 37,  /*!< I2C1 Master and Slave          */
    QUAD1_IRQn      = 38,  /*!< Quadrature Encoder 1           */
    CAN0_IRQn       = 39,  /*!< CAN0                           */
    CAN1_IRQn       = 40,  /*!< CAN1                           */
    Reserved1_IRQn  = 41,  /*!< Reserved                       */
    Reserved2_IRQn  = 42,  /*!< Reserved                       */
    Hibernate_IRQn  = 43,  /*!< Hibernate                      */
    USB0_IRQn       = 44,  /*!< USB0                           */
    PWMGEN3_IRQn    = 45,  /*!< PWM Generator 3                */
    UDMA_SW_IRQn    = 46,  /*!< uDMA Software Transfer         */
    UDMA_ERR_IRQn   = 47,  /*!< uDMA Error                     */
    ADC1_SEQ0_IRQn  = 48,  /*!< ADC1 Sequence 0                */
    ADC1_SEQ1_IRQn  = 49,  /*!< ADC1 Sequence 1                */
    ADC1_SEQ2_IRQn  = 50,  /*!< ADC1 Sequence 2                */
    ADC1_SEQ3_IRQn  = 51,  /*!< ADC1 Sequence 3                */
    Reserved3_IRQn  = 52,  /*!< Reserved                       */
    Reserved4_IRQn  = 53,  /*!< Reserved                       */
    GPIOJ_IRQn      = 54,  /*!< GPIO Port J                    */
    GPIOK_IRQn      = 55,  /*!< GPIO Port K                    */
    GPIOL_IRQn      = 56,  /*!< GPIO Port L                    */
    SSI2_IRQn       = 57,  /*!< SSI2 Rx and Tx                 */
    SSI3_IRQn       = 58,  /*!< SSI3 Rx and Tx                 */
    UART3_IRQn      = 59,  /*!< UART3 Rx and Tx                */
    UART4_IRQn      = 60,  /*!< UART4 Rx and Tx                */
    UART5_IRQn      = 61,  /*!< UART5 Rx and Tx                */
    UART6_IRQn      = 62,  /*!< UART6 Rx and Tx                */
    UART7_IRQn      = 63,  /*!< UART7 Rx and Tx                */
    Reserved5_IRQn  = 64,  /*!< Reserved                       */
    Reserved6_IRQn  = 65,  /*!< Reserved                       */
    Reserved7_IRQn  = 66,  /*!< Reserved                       */
    Reserved8_IRQn  = 67,  /*!< Reserved                       */
    I2C2_IRQn       = 68,  /*!< I2C2 Master and Slave          */
    I2C3_IRQn       = 69,  /*!< I2C3 Master and Slave          */
    TIM4_A_IRQn     = 70,  /*!< Timer 4 subtimer A             */
    TIM4_B_IRQn     = 71,  /*!< Timer 4 subtimer B             */
    Reserved10_IRQn = 72,  /*!< Reserved                       */
    Reserved11_IRQn = 73,  /*!< Reserved                       */
    Reserved12_IRQn = 74,  /*!< Reserved                       */
    Reserved13_IRQn = 75,  /*!< Reserved                       */
    Reserved14_IRQn = 76,  /*!< Reserved                       */
    Reserved15_IRQn = 77,  /*!< Reserved                       */
    Reserved16_IRQn = 78,  /*!< Reserved                       */
    Reserved17_IRQn = 79,  /*!< Reserved                       */
    Reserved18_IRQn = 80,  /*!< Reserved                       */
    Reserved19_IRQn = 81,  /*!< Reserved                       */
    Reserved20_IRQn = 82,  /*!< Reserved                       */
    Reserved21_IRQn = 83,  /*!< Reserved                       */
    Reserved22_IRQn = 84,  /*!< Reserved                       */
    Reserved23_IRQn = 85,  /*!< Reserved                       */
    Reserved24_IRQn = 86,  /*!< Reserved                       */
    Reserved25_IRQn = 87,  /*!< Reserved                       */
    Reserved26_IRQn = 88,  /*!< Reserved                       */
    Reserved27_IRQn = 89,  /*!< Reserved                       */
    Reserved28_IRQn = 90,  /*!< Reserved                       */
    Reserved29_IRQn = 91,  /*!< Reserved                       */
    TIM5_A_IRQn     = 92,  /*!< Timer 5 subtimer A             */
    TIM5_B_IRQn     = 93,  /*!< Timer 5 subtimer B             */
    WTIM0_A_IRQn    = 94,  /*!< Wide Timer 0 subtimer A        */
    WTIM0_B_IRQn    = 95,  /*!< Wide Timer 0 subtimer B        */
    WTIM1_A_IRQn    = 96,  /*!< Wide Timer 1 subtimer A        */
    WTIM1_B_IRQn    = 97,  /*!< Wide Timer 1 subtimer B        */
    WTIM2_A_IRQn    = 98,  /*!< Wide Timer 2 subtimer A        */
    WTIM2_B_IRQn    = 99,  /*!< Wide Timer 2 subtimer B        */
    WTIM3_A_IRQn    = 100, /*!< Wide Timer 3 subtimer A        */
    WTIM3_B_IRQn    = 101, /*!< Wide Timer 3 subtimer B        */
    WTIM4_A_IRQn    = 102, /*!< Wide Timer 4 subtimer A        */
    WTIM4_B_IRQn    = 103, /*!< Wide Timer 4 subtimer B        */
    WTIM5_A_IRQn    = 104, /*!< Wide Timer 5 subtimer A        */
    WTIM5_B_IRQn    = 105, /*!< Wide Timer 5 subtimer B        */
    FPU_IRQn        = 106, /*!< FPU                            */
    Reserved32_IRQn = 107, /*!< Reserved                       */
    Reserved33_IRQn = 108, /*!< Reserved                       */
    I2C4_IRQn       = 109, /*!< I2C4 Master and Slave          */
    I2C5_IRQn       = 110, /*!< I2C5 Master and Slave          */
    GPIO_M_IRQn     = 111, /*!< GPIO Port M                    */
    GPIO_N_IRQn     = 112, /*!< GPIO Port N                    */
    QEI2_IRQn       = 113, /*!< Quadrature Encoder 2           */
    Reserved34_IRQn = 114, /*!< Reserved                       */
    Reserved35_IRQn = 115, /*!< Reserved                       */
    GPIO_P_IRQn     = 116, /*!< GPIO Port P (Summary or P0)    */
    GPIO_P1_IRQn    = 117, /*!< GPIO Port P1                   */
    GPIO_P2_IRQn    = 118, /*!< GPIO Port P2                   */
    GPIO_P3_IRQn    = 119, /*!< GPIO Port P3                   */
    GPIO_P4_IRQn    = 120, /*!< GPIO Port P                   */
    GPIO_P5_IRQn    = 121, /*!< GPIO Port P                   */
    GPIO_P6_IRQn    = 122, /*!< GPIO Port P6                   */
    GPIO_P7_IRQn    = 123, /*!< GPIO Port P7                   */
    GPIO_Q_IRQn     = 124, /*!< GPIO Port Q (Summary or Q0)    */
    GPIO_Q1_IRQn    = 125, /*!< GPIO Port Q1                   */
    GPIO_Q2_IRQn    = 126, /*!< GPIO Port Q2                   */
    GPIO_Q3_IRQn    = 127, /*!< GPIO Port Q3                   */
    GPIO_Q4_IRQn    = 128, /*!< GPIO Port Q4                   */
    GPIO_Q5_IRQn    = 129, /*!< GPIO Port Q5                   */
    GPIO_Q6_IRQn    = 130, /*!< GPIO Port Q6                   */
    GPIO_Q7_IRQn    = 131, /*!< GPIO Port Q7                   */
    GPIO_R_IRQn     = 132, /*!< GPIO Port R                    */
    GPIO_S_IRQn     = 133, /*!< GPIO Port S                    */
    PWM1_0_IRQn     = 134, /*!< PWM 1 Generator 0              */
    PWM1_1_IRQn     = 135, /*!< PWM 1 Generator 1              */
    PWM1_2_IRQn     = 136, /*!< PWM 1 Generator 2              */
    PWM1_3_IRQn     = 137, /*!< PWM 1 Generator 3              */
    PWM1_FAULT_IRQn = 138  /*!< PWM 1 Fault                    */
} interrupt_id_t;

#define __COMPILER_BARRIER() asm volatile("" ::: "memory")

// from core_cm4.h
/* Memory mapping of Core Hardware */
#define SCS_BASE (0xE000E000UL)            /*!< System Control Space Base Address */
#define ITM_BASE (0xE0000000UL)            /*!< ITM Base Address */
#define DWT_BASE (0xE0001000UL)            /*!< DWT Base Address */
#define TPI_BASE (0xE0040000UL)            /*!< TPI Base Address */
#define CoreDebug_BASE (0xE000EDF0UL)      /*!< Core Debug Base Address */
#define SysTick_BASE (SCS_BASE + 0x0010UL) /*!< SysTick Base Address */
#define NVIC_BASE (SCS_BASE + 0x0100UL)    /*!< NVIC Base Address */
#define SCB_BASE (SCS_BASE + 0x0D00UL)     /*!< System Control Block Base Address */

#define SCnSCB ((SCnSCB_Type *)SCS_BASE)             /*!< System control Register not in SCB */
#define SCB ((SCB_Type *)SCB_BASE)                   /*!< SCB configuration struct */
#define SysTick ((SysTick_Type *)SysTick_BASE)       /*!< SysTick configuration struct */
#define NVIC ((NVIC_Type *)NVIC_BASE)                /*!< NVIC configuration struct */
#define ITM ((ITM_Type *)ITM_BASE)                   /*!< ITM configuration struct */
#define DWT ((DWT_Type *)DWT_BASE)                   /*!< DWT configuration struct */
#define TPI ((TPI_Type *)TPI_BASE)                   /*!< TPI configuration struct */
#define CoreDebug ((CoreDebug_Type *)CoreDebug_BASE) /*!< Core Debug configuration struct */

/**
  \brief Structure type to access the Nested Vectored Interrupt Controller(NVIC).
*/

typedef struct
{
    volatile uint32_t ISER[1]; /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register           */
    uint32_t          RESERVED0[31];
    volatile uint32_t ICER[1]; /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register          */
    uint32_t          RSERVED1[31];
    volatile uint32_t ISPR[1]; /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register           */
    uint32_t          RESERVED2[31];
    volatile uint32_t ICPR[1]; /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register         */
    uint32_t          RESERVED3[31];
    uint32_t          RESERVED4[64];
    volatile uint32_t IP[8]; /*!< Offset: 0x300 (R/W)  Interrupt Priority Register              */
} NVIC_Type;

#define NVIC ((NVIC_Type *)NVIC_BASE) /*!< NVIC configuration struct */

static inline void NVIC_EnableIRQ(interrupt_id_t IRQn)
{
    NVIC->ISER[0] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

static inline void NVIC_ClearPendingIRQ(interrupt_id_t IRQn)
{
    NVIC->ICPR[0] = (1 << ((uint32_t)(IRQn) & 0x1F)); /* Clear pending interrupt */
}

static inline void NVIC_DisableIRQ(interrupt_id_t IRQn)
{
    NVIC->ICER[0] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

static inline void NVIC_SetPendingIRQ(interrupt_id_t IRQn)
{
    NVIC->ISPR[0] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

__attribute__((always_inline)) static inline void __enable_irq(void)
{
    asm volatile("cpsie i");
}
#endif /* DEVICE_IRQ_H */