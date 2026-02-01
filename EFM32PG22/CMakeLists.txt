cmake_minimum_required(VERSION 3.20)

# Optional: print out extra messages to see what is going on. Comment it to have less verbose messages
set(CMAKE_VERBOSE_MAKEFILE ON)

# Path to toolchain file. This one has to be before 'project()' below
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-none-eabi-gcc.cmake)

# The place we all the projects files including common core files are
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
set(TOP_DIRECTORY
   C:/projects/MCUs
)
else ()
set(TOP_DIRECTORY
   /home/jack/git/MCUs
)
endif()

option (WITH_UNITS "Use reach logging." OFF)

set(MCU_MODEL EFM32PG22C200F512IM40=1)
set( MCU_DIRECTORY ${TOP_DIRECTORY}/EFM32PG22)

# Setup project, output and linker file
project("EFM32PG22")
set(EXECUTABLE ${PROJECT_NAME}.elf)
message(STATUS "CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")

set(LINKER_FILE ${MCU_DIRECTORY}/device/efm32pg22.ld)

enable_language(C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Optional: issue a message to be sure it uses the correct toolchain file.
message(STATUS "Executable is: ${EXECUTABLE}")

#headers
set(PROJECT_INCLUDE_DIRECTORIES
${MCU_DIRECTORY}/include
${TOP_DIRECTORY}/hal_include
${MCU_DIRECTORY}/device
${TOP_DIRECTORY}/CMSIS
${TOP_DIRECTORY}/CMSIS/Core/Include
${TOP_DIRECTORY}/drivers
)

#sources
file(GLOB_RECURSE SRC_FILES
   ${MCU_DIRECTORY}/source/*.c
   ${MCU_DIRECTORY}/hal/*.c
   ${MCU_DIRECTORY}/device/*.c
   ${MCU_DIRECTORY}/device/*.S
   ${TOP_DIRECTORY}/drivers/*.c
)


# Build the executable based on the source files
add_executable(${EXECUTABLE} ${SRC_FILES})

message(STATUS "Executable is: ${EXECUTABLE}")

# Add header directories (AFTER add_executable !!)
target_include_directories(${EXECUTABLE} PRIVATE
    ${PROJECT_INCLUDE_DIRECTORIES})



# Compiler options
target_compile_options(${EXECUTABLE} PRIVATE
-DDEBUG
-DCORE_M4
-D${MCU_MODEL}
        -mcpu=cortex-m4
        -mthumb
	-fno-common
        -fdata-sections
        -ffunction-sections
	-fmessage-length=0
	-fno-builtin
	-fmerge-constants
	-fstack-usage
        -fdiagnostics-color=auto
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
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
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -specs=nano.specs
        -specs=nosys.specs
        -Wl,--start-group
        -Wl,--end-group
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
	-Xlinker --gc-sections -Xlinker
        -Wl,--print-memory-usage
        -lnosys
        -lc
        -lm
        )

# Optional: Print executable size as part of the post build process
#add_custom_command(TARGET ${CMAKE_PROJECT_NAME}
#        POST_BUILD
#        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE})
message(STATUS "Executable is: ${EXECUTABLE}")
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE_UTIL} $<TARGET_FILE:${EXECUTABLE}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${PROJECT_ORIGINAL_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${PROJECT_ORIGINAL_NAME}.bin
)


# Optional: Create hex, bin and S-Record files after the build
#add_custom_command(TARGET ${CMAKE_PROJECT_NAME}
#        POST_BUILD
#        COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 ${EXECUTABLE} ${PROJECT_NAME}.s19
#        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
#        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)
