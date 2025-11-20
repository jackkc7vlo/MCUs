/**
 * ****************************(C) COPYRIGHT 2024 Old Man Software *****************
 * @file       memory_hal.h
 * @author     Jack Wilson
 * @brief      Wrapper for OS memory functions, including a minimalistic OS
 *
 * @details    This file contains the prototypes for the OS memory functions and
 *             can be used to create a minimalistic OS.  It also can allow for indivudual
 *             chips or projects to use memory functions from other RTOSes or OSes.
 *
 * @note       It might seem reduntant to have a memory wrapper for an OS, but it allows
 *             for the OS to be easily ported to other chips or projects, epecially for memory
 *             such as flash or EEPROM.  By having this wrapper, you can debug code using RAM
 *             and then switch to EEPROM or Flash later after the calling code is debugged.
 *
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
#ifndef os_memory_h_
#define os_memory_h_

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
    typedef enum memory_types
    {
        RAM_MEMORY = 1,    ///< Ordinary RAM
        EEPROM_MEMORY = 2, ///< EEPROM
        FLASH_MEMORY = 3,  ///< Flash Memory
        RAM_POOL = 4,      ///< SPECIAL pool of ram for buffers
        MEMORY_NONE = 0xFF ///< No memory type
    } memory_types_t;

    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/
    //<! Pointer to an incomplete type (hides implementation)
    typedef struct memory_hal_def *p_memory_hal_def_t;

    /// *****************************************************************************
    /// @brief Initialize the variuse memory type
    /// @return bool true if successful, false if error
    /// @warning This is a one time allocation at initialization time and should
    ///          not be called as a general alloc/free function
    /// *****************************************************************************
    bool memory_hal_initialize(void);

    /// *****************************************************************************
    /// @brief create a memory type
    /// @param type The memory type (RAM, EEPROM, etc)
    /// @param size The size of the memory in bytes
    /// @param number_of_buffers The number of buffers to create (RAM_POOL only)
    /// @return Pointer to the memory type, NULL if error
    /// @warning This is a one time allocation at initialization time and should
    ///          not be called as a general alloc/free function
    /// *****************************************************************************
    p_memory_hal_def_t memory_hal_create(const memory_types_t memory_type, const uint32_t size, const uint32_t number_of_buffers, const memory_owner_t owner);
    /// *****************************************************************************
    /// @brief write to memory defined by p_memory_def
    /// @param p_memory_def The memory definition
    /// @param p_data Data to write
    /// @param offset Offset into memory to write to
    /// @param bytes_to_write Number of bytes to write
    /// @return true if successful, false if error
    /// *****************************************************************************
    bool memory_hal_write(const p_memory_hal_def_t p_memory_def, const uint8_t *p_data, const uint32_t offset, const uint32_t bytes_to_write);
    /// *****************************************************************************
    /// @brief read from memory defined by p_memory_def
    /// @param p_memory_def The memory definition
    /// @param p_data Data to read into
    /// @param offset Offset into memory to read into
    /// @param bytes_to_write Number of bytes to read
    /// @return true if successful, false if error
    /// *****************************************************************************
    bool memory_hal_read(const p_memory_hal_def_t p_memory_def, const uint8_t *p_data, const uint32_t offset, const uint32_t bytes_to_read);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* os_memory.h_ */
