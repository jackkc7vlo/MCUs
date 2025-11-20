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
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h> /*lint -e129*/
#include <string.h>
#include <hw_config.h>
#include <assert.h>
#include <memory_hal.h>
#include <spin_lock_hal.h> /*lint -e793*/

#if USE_PRINTF == 1u
#include <printf_hal.h>
#endif
    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    typedef struct memory_hal_def
    {
#ifdef PROTECT_MEM
        uint32_t block_guard; // Always 0xABCDDCBA unless overwrite error */
#endif
        struct memory_hal_def *next; ///< Pointer to the next memory block
        memory_types_t type;         ///< Memory type (RAM, EEPROM, etc)
        uint32_t size;               ///< The size of the memory in bytes
        memory_owner_t owner;        ///< The owner of the memory (debug aid)
        uint8_t *location;           ///< The location of the memory for the memory type
    } memory_hal_def_t;

    static const spin_lock_def_t mem_lock = {.lock = 0};

    static volatile p_memory_hal_def_t p_general_memory = NULL; /*lint -e844*/
    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    /// *****************************************************************************
    /// @brief Initialize the variuse memory type
    /// @return bool true if successful, false if error
    /// @warning This is a one time allocation at initialization time and should
    ///          not be called as a general alloc/free function
    /// *****************************************************************************
    bool memory_hal_initialize(void)
    {
        spin_lock_hal_lock(&mem_lock);
        p_general_memory = memory_hal_create(RAM_MEMORY, GENERAL_MEM_SIZE, 1u, SYSTEM_MEMORY);
        assert(p_general_memory != NULL);
        if (p_general_memory == NULL)
        {
            return false;
        }
        spin_lock_hal_unlock(&mem_lock);
        return true;
    }

    /// *****************************************************************************
    /// @brief allocate memory for a memory type
    /// @param memory_type The memory type (RAM, EEPROM, etc)
    /// @param size The size of the memory in bytes
    /// @param number_of_buffers The number of buffers to create (RAM_POOL only)
    /// @return Pointer to the memory type, NULL if error
    /// @warning This is a one time allocation at initialization time and should
    ///          not be called as a general alloc/free function
    /// *****************************************************************************
    void *memory_hal_allocate(const memory_types_t memory_type, const uint32_t size, const uint32_t number_of_buffers)
    {
#if USE_PRINTF == 1u && PRINT_ALLOC == 1u
        printf("memomry hal malloc\n");
#endif
        void *p_memory = NULL;
        switch (memory_type)
        {
            /// for the moment we will just allocate from RAM just to get things working
        case RAM_MEMORY:
            p_memory = (void *)calloc(1u, size);
            break;
        case EEPROM_MEMORY:
            p_memory = (void *)calloc(1u, size); // temporary
            break;
        case FLASH_MEMORY:
            return NULL; // so we don't assert below
            break;       /*lint !e527 */
        case RAM_POOL:
            p_memory = (void *)calloc(number_of_buffers, size);
            break;
        case MEMORY_NONE:
        default:
            p_memory = NULL;
        }
        assert(p_memory != NULL);

        return p_memory;
    }

    /// *****************************************************************************
    /// @brief create a memory type (The overall memory allocation function)
    /// @param type The memory type (RAM, EEPROM, etc)
    /// @param size The size of the memory in bytes
    /// @param number_of_buffers The number of buffers to create (RAM_POOL only)
    /// @return Pointer to the memory type, NULL if error
    /// @warning This is a one time allocation at initialization time and should
    ///          not be called as a general alloc/free function
    /// *****************************************************************************
    p_memory_hal_def_t memory_hal_create(const memory_types_t memory_type, const uint32_t size, const uint32_t number_of_buffers, const memory_owner_t owner)
    {
        spin_lock_hal_lock(&mem_lock);
        const p_memory_hal_def_t p_memory = (void *)calloc(1u, sizeof(memory_hal_def_t));
        if (p_memory != NULL)
        {
            p_memory->type = memory_type;
            p_memory->size = size;
            p_memory->owner = owner;
            p_memory->location = memory_hal_allocate(memory_type, size, number_of_buffers);
        }
        spin_lock_hal_unlock(&mem_lock);
        return p_memory;
    }
    /// *****************************************************************************
    /// @brief write to memory defined by p_memory_def
    /// @param p_memory_def The memory definition
    /// @param p_data Data to write
    /// @param offset Offset into memory to write to
    /// @param bytes_to_write Number of bytes to write
    /// @return true if successful, false if error
    /// *****************************************************************************
    bool memory_hal_write(const p_memory_hal_def_t p_memory_def, const uint8_t *p_data, const uint32_t offset, const uint32_t bytes_to_write)
    {
        bool success = false;
        // uint8_t *p_memory;
        assert(offset + bytes_to_write <= p_memory_def->size);

        switch (p_memory_def->type)
        {
        case RAM_MEMORY:
            memcpy((uint8_t *)p_memory_def->location + offset, p_data, bytes_to_write);
            success = true;
            break;
        case EEPROM_MEMORY:
            memcpy((uint8_t *)p_memory_def->location + offset, p_data, bytes_to_write);
            success = true;
            break;
        case FLASH_MEMORY:
            memcpy((uint8_t *)p_memory_def->location + offset, p_data, bytes_to_write);
            success = true;
            break;
        case RAM_POOL:
            memcpy((uint8_t *)p_memory_def->location + offset, p_data, bytes_to_write);
            success = true;
            break;
        case MEMORY_NONE:
            success = false;
            break;
        }
        return success;
    } /*lint !e818 !e952*/
    /// *****************************************************************************
    /// @brief read from memory defined by p_memory_def
    /// @param p_memory_def The memory definition
    /// @param p_data Data to read into
    /// @param offset Offset into memory to read into
    /// @param bytes_to_write Number of bytes to read
    /// @return true if successful, false if error
    /// *****************************************************************************
    bool memory_hal_read(const p_memory_hal_def_t p_memory_def, const uint8_t *p_data, const uint32_t offset, const uint32_t bytes_to_read)
    {
        bool success = false;
#if USE_PRINTF == 1u
        (void)printf_hal("Reading from memory owner %lu type %d\n", p_memory_def->owner, p_memory_def->type);
#endif
        assert(offset + bytes_to_read <= p_memory_def->size);

        switch (p_memory_def->type)
        {
        case RAM_MEMORY:
            memcpy((uint8_t *)p_data, (uint8_t *)p_memory_def->location + offset, bytes_to_read);
            success = true;
            break;
        case EEPROM_MEMORY: // bogus for now
            memcpy((uint8_t *)p_data, (uint8_t *)p_memory_def->location + offset, bytes_to_read);
            success = true;
            break;
        case FLASH_MEMORY: // bogus for now
            memcpy((uint8_t *)p_data, (uint8_t *)p_memory_def->location + offset, bytes_to_read);
            success = true;
            break;
            // memcpy((uint8_t *)p_data, (uint8_t *)p_memory_def->location + offset, bytes_to_read);

        case RAM_POOL:
            memcpy((uint8_t *)p_data, (uint8_t *)p_memory_def->location + offset, bytes_to_read);
            success = true;
            break;
        case MEMORY_NONE:
        default:
#if USE_PRINTF == 1u
            (void)printf_hal("****memory hal read error\n");
#endif
            success = false;
        }
        return success;
        /*lint -save -e818 -e952*/
    }
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
