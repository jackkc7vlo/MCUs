cmake_minimum_required(VERSION 3.15.3)

# Optional: print out extra messages to see what is going on. Comment it to have less verbose messages
set(CMAKE_VERBOSE_MAKEFILE ON)

# Path to toolchain file. This one has to be before 'project()' below
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-none-eabi-gcc.cmake)

# The place we all the projects files including common core files are
set(TOP_DIRECTORY
   /home/jack/git/MCUs
)

option (WITH_UNITS "Use reach logging." OFF)

set( MCU_DIRECTORY ${TOP_DIRECTORY}/TM4C123G)

# Setup project, output and linker file
project(TM4C123G)
set(EXECUTABLE ${PROJECT_NAME}.elf)
set(LINKER_FILE ${MCU_DIRECTORY}/device/tm4c123.ld)

enable_language(C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Optional: issue a message to be sure it uses the correct toolchain file.
message(STATUS "CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")

#headers
set(PROJECT_INCLUDE_DIRECTORIES
   ${MCU_DIRECTORY}/include
   ${MCU_DIRECTORY}/hal
   ${MCU_DIRECTORY}/device
   ${TOP_DIRECTORY}/CMSIS
   ${TOP_DIRECTORY}/CMSIS/Core/Include
   ${TOP_DIRECTORY}/hal_include
#   ${TOP_DIRECTORY}/Common/RTT
#   ${TOP_DIRECTORY}/Common/hal_include
#   ${TOP_DIRECTORY}/Common/os_hal_include
#   ${TOP_DIRECTORY}/Common/UnitTests
#   ${TOP_DIRECTORY}/TagCode/Include
#   ${TOP_DIRECTORY}/Drivers
)

#sources
file(GLOB_RECURSE SRC_FILES
   ${MCU_DIRECTORY}/source/*.c
   ${MCU_DIRECTORY}/hal/*.c
   ${MCU_DIRECTORY}/device/*.c
#   ${MCU_DIRECTORY}/os_hal/*.c
#   ${MCU_DIRECTORY}/TagCode/Source/*.c
#   ${TOP_DIRECTORY}/Common/RTT/*.c
#   ${TOP_DIRECTORY}/Common/RTT/*.S
#   ${TOP_DIRECTORY}/Common/UnitTests/*.c
#   ${TOP_DIRECTORY}/Drivers/*.c
)


# Build the executable based on the source files
add_executable(${EXECUTABLE} ${SRC_FILES})

# Add header directories (AFTER add_executable !!)
target_include_directories(${EXECUTABLE} PRIVATE
    ${PROJECT_INCLUDE_DIRECTORIES})

# List of compiler defines, prefix with -D compiler option
target_compile_definitions(${EXECUTABLE} PRIVATE
        -DDEBUG
	-DCORE_M0PLUS
	-D_TM4C123G_
        )


# Compiler options
target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m4
        -mthumb
	-fno-common
        -fdata-sections
        -ffunction-sections
	-fmessage-length=0
	-fno-builtin
        -nostdlib
	-fmerge-constants
	-fstack-usage
        -fdiagnostics-color=auto
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wshadow
        -Wpointer-arith
        -Winit-self
        -Wstrict-overflow=2
        $<$<CONFIG:Debug>:-O0 -g3 -ggdb3>
        $<$<CONFIG:Release>:-Og -g0>
        )

# Linker options
target_link_options(${EXECUTABLE} PRIVATE
        -T${LINKER_FILE}
        -mcpu=cortex-m4
        -mthumb
        -specs=nano.specs
        -specs=nosys.specs
        -Wl,--start-group
        -lc
        -lm
        -lnosys
        -Wl,--end-group
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
	-Xlinker --gc-sections -Xlinker
        -Wl,--print-memory-usage
        -nostdlib
        )

# Optional: Print executable size as part of the post build process
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE})

# Optional: Create hex, bin and S-Record files after the build
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 ${EXECUTABLE} ${PROJECT_NAME}.s19
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)