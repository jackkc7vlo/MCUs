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
#ifndef spin_lock_h_
#define spin_lock_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdint.h>

  /********************************************************************************
   * Defines
   ********************************************************************************/

  /********************************************************************************
   * Typedefs & Enums
   ********************************************************************************/
  typedef struct spin_lock_def_s
  {
    int32_t lock;
  } spin_lock_def_t;

  // typedef spin_lock_def_t *p_spin_lock_def_t;

  /********************************************************************************
   * Function Prototypes
   ********************************************************************************/

  void spin_lock_hal_lock(const spin_lock_def_t *s);

  void spin_lock_hal_unlock(const spin_lock_def_t *s);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* spin_lock.h_ */
