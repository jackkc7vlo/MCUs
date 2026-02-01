/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       device_reg.h
 * @author     Jack Wilson
 * @brief      Processor specific registers and register access functions
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-17-2023     Jack Wilson                     jackkc7vlo@gmail.com
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
#ifndef device_reg_h_
#define device_reg_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <efm32pg22c200f512im40.h>
#include <stdint.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /********************************************************************************/
    static inline void device_reg_masked_clear(volatile uint32_t *addr, uint32_t mask)
    {

        uint32_t aliasAddr              = (uint32_t)addr + 0x2000u; // clear offset
        *(volatile uint32_t *)aliasAddr = mask;
    }

    /********************************************************************************/
    static inline void device_reg_masked_set(volatile uint32_t *addr, uint32_t mask)
    {

        uint32_t aliasAddr              = (uint32_t)addr + 0x1000u; // set offset
        *(volatile uint32_t *)aliasAddr = mask;
    }

    /********************************************************************************/
    static inline void device_reg_masked_write(volatile uint32_t *addr, uint32_t mask, uint32_t val)
    {

        device_reg_masked_clear(addr, mask);
        device_reg_masked_set(addr, val);
    }

    /********************************************************************************/
    static inline uint32_t device_reg_masked_read(volatile const uint32_t *addr, uint32_t mask)
    {
        return *addr & mask;
    }

    /********************************************************************************/
    static inline void device_reg_bit_write(volatile uint32_t *addr, unsigned int bit,
                                            unsigned int val)
    {

        uint32_t aliasAddr;
        if (val)
        {
            aliasAddr = (uint32_t)addr + 0x1000u; // set offset
        }
        else
        {
            aliasAddr = (uint32_t)addr + 0x2000u; // clear offset
        }
        *(volatile uint32_t *)aliasAddr = 1 << bit;
    }

    /********************************************************************************/
    static inline unsigned int device_reg_bit_read(volatile const uint32_t *addr, unsigned int bit)
    {
        return ((*addr) >> bit) & 1UL;
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* reg_hal_h_ */
