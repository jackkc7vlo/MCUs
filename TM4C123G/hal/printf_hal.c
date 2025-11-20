/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       printf_hal_h_
 * @author     Jack Wilson
 * @brief      printf function to allow for different hardware implementations
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Mar-11-2024     Jack Wilson                     jackkc7vlo@gmail.com
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
 * Includes
 ********************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdio.h>  /*lint -e793*/
#include <stdlib.h> /*lint -e129*/
#include <hw_config.h>
#if USE_PRINTF == 1u
#if USE_SEGGER == 1u
#include <SEGGER_RTT.h>
#endif
#endif

    int32_t printf_hal(const char *sFormat, ...)
    {
        int32_t r;

        va_list ParamList;

        va_start(ParamList, sFormat);
#if USE_PRINTF == 1u
#if USE_SEGGER == 1u
        r = SEGGER_RTT_vprintf(0u, sFormat, &ParamList);
#else
        r = vprintf(sFormat, ParamList);
#endif
#endif
        va_end(ParamList);
        return r;
    } /*lint !e952*/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
