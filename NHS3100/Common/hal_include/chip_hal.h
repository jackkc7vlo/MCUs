/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       chip_hal.h
 * @author     Jack Wilson
 * @brief      Chip specific hardware abstraction layer
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Dec-20-2023     Jack Wilson                     jackkc7vlo@gmail.com
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
#ifndef chip_hal_h_
#define chip_hal_h_

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

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
void chip_hal_initialize_IO(void);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* chip_hal.h_ */
