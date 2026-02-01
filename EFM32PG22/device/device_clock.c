/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       device_clock.c
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
 *
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

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/

/********************************************************************************
 * Includes
 ********************************************************************************/
#include <device_clock.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*lint -esym(793,__*)*/
// #include <stdint.h>
#include <assert.h>
#include <device_reg.h>
// #include <stdbool.h>
#include <stdlib.h> /*lint -e129*/
    // #include <efm32pg22c200f512im40.h>
    /********************************************************************************
     * Defines
     ********************************************************************************/
/*lint -e750 -e9059 -e835*/
#define CMU_EN_BIT_POS 0U
#define CMU_EN_BIT_MASK 0x1FU
#define CMU_NO_EN_REG 0u

#define CMU_CLKEN0_EN_REG 1u
#define CMU_CLKEN1_EN_REG 2u
#define CMU_CRYPTOACCCLKCTRL_EN_REG 3u
#define CMU_EN_REG_POS 5u
#define CMU_EN_REG_MASK 0x3u

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    volatile uint32_t loops_per_jiffy = 98400u;
    /********************************************************************************
     * Functions
     ********************************************************************************/

    /********************************************************************************/
    static void device_clock_init_hfxo(void)
    {
        SystemHFXOClockSet(38400000u);
    }

    /********************************************************************************/
    static void device_clock_init_clocks(void)
    {
        // sli_em_cmu_HFXOSetForceEnable();
#if defined(_CMU_CLKEN0_MASK) && defined(CMU_CLKEN0_HFXO0)
        CMU->CLKEN0_SET = CMU_CLKEN0_HFXO0;
#endif
        HFXO0->CTRL_SET = HFXO_CTRL_FORCEEN;

        ((CMU_TypeDef *)((0x50008000UL)))->SYSCLKCTRL =
            (((CMU_TypeDef *)((0x50008000UL)))->SYSCLKCTRL & ~0x7UL) | (0x00000003UL << 0);
        // sli_em_cmu_SYSCLKInitPostClockSelect();
        if ((((HFXO_TypeDef *)((0x5000C000UL)))->CTRL & (0x1UL << 1u)) == 0u)
        {
            ((HFXO_TypeDef *)((0x5000C000UL)))->CTRL_CLR = (0x1UL << 0u);
        }

        ((CMU_TypeDef *)((0x50008000UL)))->EM01GRPACLKCTRL =
            (((CMU_TypeDef *)((0x50008000UL)))->EM01GRPACLKCTRL & ~0x3UL) | (0x00000002UL << 0);

        ((CMU_TypeDef *)((0x50008000UL)))->EM01GRPBCLKCTRL =
            (((CMU_TypeDef *)((0x50008000UL)))->EM01GRPBCLKCTRL & ~0x7UL) | (0x00000002UL << 0);

        ((CMU_TypeDef *)((0x50008000UL)))->EM23GRPACLKCTRL =
            (((CMU_TypeDef *)((0x50008000UL)))->EM23GRPACLKCTRL & ~0x3UL) | (0x00000002UL << 0);

        ((CMU_TypeDef *)((0x50008000UL)))->EM4GRPACLKCTRL =
            (((CMU_TypeDef *)((0x50008000UL)))->EM4GRPACLKCTRL & ~0x3UL) | (0x00000002UL << 0);

        device_clock_enable(CLOCK_GPIO, (bool)true);
    }

    /********************************************************************************/
    void device_clock_init(void)
    {

        // Enable HFXO module clock.
        device_clock_init_hfxo();
        device_clock_init_clocks();
    }

    /********************************************************************************/
    void device_clock_set_divisor(const uint32_t divisor)
    {
        (void)divisor;
        assert(false); /*lint !e506 Not implemented yet */
    }

    /********************************************************************************/
    void device_clock_set_clock_frequency(const uint32_t frequency)
    {
        (void)frequency;
        assert(false); /*lint !e506*/
        /* Divide-by-0 NOT allowed */
    }

    /********************************************************************************/
    uint32_t device_clock_get_divisor(void)
    {
        return 0u;
    }

    /********************************************************************************/
    uint32_t device_clock_get_freq(void)
    {
        const uint32_t ret = SystemCoreClockGet();

        return ret;
    }

#if defined(__GNUC__) /* GCC */

    /********************************************************************************/
    void device_clock_delay_us(const uint32_t us)
    {

        __ASM volatile(
#if (__CORTEX_M == 0x00u)
            "        .syntax unified           \n"
            "        .arch armv6-m             \n"
#endif
            "        cmp     %0, #0            \n" /* Return if 0 delay. */
            "        beq.n   2f                \n"
            "        subs    %0, #1            \n" /* Correct for off by one error. */
            "        movs    r2, #0x88         \n"
            "        lsls    r2, r2, #8        \n"
            "        adds    r2, #0x00         \n"
            "        muls    %0, r2            \n"
            "                                  \n"
            "        ldr     r2, [%1]          \n"
            "        movs    r0, %0, lsr #11   \n"
            "        movs    r2, r2, lsr #11   \n"
            "                                  \n"
            "        muls    r0, r2            \n"
            "        movs    r0, r0, lsr #6    \n"
            "                                  \n"
            "        beq.n   2f                \n"
            "                                  \n"
            "1:      subs    r0, #1            \n"
            "        bhi     1b                \n"
#if (__CORTEX_M == 0x00u)
            "2:                                \n"
            "        .syntax divided           \n"
            :
            : "r"(us), "r"(&loops_per_jiffy)
            : "r0", "r2", "cc");
#else
            "2:                                \n"
            :
            : "r"(us), "r"(&loops_per_jiffy)
            : "r0", "r2", "cc");
#endif
    } /*lint !e715*/
#endif /* defined(__GNUC__) */

    void device_clock_delay(const uint32_t ms)
    {
        for (uint32_t i = 0u; i < ms; i++)
        {
            device_clock_delay_us(1000u);
        }
    }

    /********************************************************************************/
    void device_clock_enable(const uint32_t clock, const bool enable)
    {
        volatile uint32_t *reg = NULL;
        uint32_t           bit;

        if (((uint32_t)clock >> CMU_EN_REG_POS) == CMU_CLKEN0_EN_REG)
        {
            reg = &CMU->CLKEN0;
        }
        else if (((uint32_t)clock >> CMU_EN_REG_POS) == CMU_CLKEN1_EN_REG)
        {
            reg = &CMU->CLKEN1;
        }
        else
        {
            reg = &CMU->CRYPTOACCCLKCTRL;
        }

        /* Get the bit position used to enable/disable. */
        bit = ((uint32_t)clock >> CMU_EN_BIT_POS) & CMU_EN_BIT_MASK;

        /* Set/clear bit as requested. */
        device_reg_bit_write(reg, bit, (uint32_t)enable);
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
