/**
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************
 * @file       spin_lock.h
 * @author     Jack Wilson
 * @brief      Basic spin lock implementation
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Apr-29-2024     Jack Wilson                     jackkc7vlo@gmail.com
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
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************/

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdint.h>
#include <spin_lock_hal.h>
#include <assert.h>
#include <stdlib.h> /*lint -e129*/
                    // #include <atomic>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    void spin_lock_hal_lock(const spin_lock_def_t *s)
    {
        while (1)
        {
            int32_t zero = 0;
            int32_t one = 1;
            if (__atomic_compare_exchange(&s->lock, &zero,
                                          &one, 0,
                                          __ATOMIC_SEQ_CST,
                                          __ATOMIC_SEQ_CST))
                return;
        }
    }

    void spin_lock_hal_unlock(const spin_lock_def_t *s)
    {
        int32_t zero = 0;
        __atomic_store(&s->lock, &zero, __ATOMIC_SEQ_CST);
    }

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
