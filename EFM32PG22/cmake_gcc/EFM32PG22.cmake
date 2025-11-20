####################################################################
# Automatically-generated file. Do not edit!                       #
####################################################################

set(SDK_PATH "/home/jack/.silabs/slt/installs/conan/p/simpleb526998f4a4d/p")
set(COPIED_SDK_PATH "simplicity_sdk_2025.6.2")
set(PKG_PATH "/home/jack/.silabs/slt/installs")

add_library(slc OBJECT
    "../${COPIED_SDK_PATH}/hardware/board/src/sl_board_control_gpio.c"
    "../${COPIED_SDK_PATH}/hardware/board/src/sl_board_init.c"
    "../${COPIED_SDK_PATH}/hardware/driver/mx25_flash_shutdown/src/sl_mx25_flash_shutdown_usart/sl_mx25_flash_shutdown.c"
    "../${COPIED_SDK_PATH}/platform/common/src/sl_assert.c"
    "../${COPIED_SDK_PATH}/platform/common/src/sl_core_cortexm.c"
    "../${COPIED_SDK_PATH}/platform/common/src/sl_syscalls.c"
    "../${COPIED_SDK_PATH}/platform/Device/SiliconLabs/EFM32PG22/Source/startup_efm32pg22.c"
    "../${COPIED_SDK_PATH}/platform/Device/SiliconLabs/EFM32PG22/Source/system_efm32pg22.c"
    "../${COPIED_SDK_PATH}/platform/driver/gpio/src/sl_gpio.c"
    "../${COPIED_SDK_PATH}/platform/emlib/src/em_cmu.c"
    "../${COPIED_SDK_PATH}/platform/emlib/src/em_emu.c"
    "../${COPIED_SDK_PATH}/platform/emlib/src/em_gpio.c"
    "../${COPIED_SDK_PATH}/platform/emlib/src/em_msc.c"
    "../${COPIED_SDK_PATH}/platform/emlib/src/em_system.c"
    "../${COPIED_SDK_PATH}/platform/emlib/src/em_usart.c"
    "../${COPIED_SDK_PATH}/platform/peripheral/src/sl_hal_gpio.c"
    "../${COPIED_SDK_PATH}/platform/service/clock_manager/src/sl_clock_manager.c"
    "../${COPIED_SDK_PATH}/platform/service/clock_manager/src/sl_clock_manager_hal_s2.c"
    "../${COPIED_SDK_PATH}/platform/service/clock_manager/src/sl_clock_manager_init.c"
    "../${COPIED_SDK_PATH}/platform/service/clock_manager/src/sl_clock_manager_init_hal_s2.c"
    "../${COPIED_SDK_PATH}/platform/service/device_init/src/sl_device_init_dcdc_s2.c"
    "../${COPIED_SDK_PATH}/platform/service/device_init/src/sl_device_init_emu_s2.c"
    "../${COPIED_SDK_PATH}/platform/service/device_manager/clocks/sl_device_clock_efr32xg22.c"
    "../${COPIED_SDK_PATH}/platform/service/device_manager/src/sl_device_clock.c"
    "../${COPIED_SDK_PATH}/platform/service/device_manager/src/sl_device_gpio.c"
    "../${COPIED_SDK_PATH}/platform/service/interrupt_manager/src/sl_interrupt_manager_cortexm.c"
    "../${COPIED_SDK_PATH}/platform/service/memory_manager/src/sl_memory_manager_region.c"
    "../${COPIED_SDK_PATH}/platform/service/sl_main/src/sl_main_init.c"
    "../${COPIED_SDK_PATH}/platform/service/sl_main/src/sl_main_init_memory.c"
    "../${COPIED_SDK_PATH}/platform/service/sl_main/src/sl_main_process_action.c"
    "../${COPIED_SDK_PATH}/platform/service/udelay/src/sl_udelay.c"
    "../${COPIED_SDK_PATH}/platform/service/udelay/src/sl_udelay_armv6m_gcc.S"
    "../app.c"
    "../autogen/sl_board_default_init.c"
    "../autogen/sl_event_handler.c"
    "../main.c"
)

target_include_directories(slc PUBLIC
   "../config"
   "../autogen"
   "../."
    "../${COPIED_SDK_PATH}/platform/Device/SiliconLabs/EFM32PG22/Include"
    "../${COPIED_SDK_PATH}/hardware/board/inc"
    "../${COPIED_SDK_PATH}/platform/service/clock_manager/inc"
    "../${COPIED_SDK_PATH}/platform/service/clock_manager/src"
    "../${COPIED_SDK_PATH}/platform/CMSIS/Core/Include"
    "../${COPIED_SDK_PATH}/platform/common/inc"
    "../${COPIED_SDK_PATH}/platform/service/device_manager/inc"
    "../${COPIED_SDK_PATH}/platform/service/device_init/inc"
    "../${COPIED_SDK_PATH}/platform/emlib/inc"
    "../${COPIED_SDK_PATH}/platform/driver/gpio/inc"
    "../${COPIED_SDK_PATH}/platform/peripheral/inc"
    "../${COPIED_SDK_PATH}/platform/service/interrupt_manager/inc"
    "../${COPIED_SDK_PATH}/platform/service/interrupt_manager/src"
    "../${COPIED_SDK_PATH}/platform/service/interrupt_manager/inc/arm"
    "../${COPIED_SDK_PATH}/platform/service/memory_manager/inc"
    "../${COPIED_SDK_PATH}/hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart"
    "../${COPIED_SDK_PATH}/platform/service/sl_main/inc"
    "../${COPIED_SDK_PATH}/platform/service/sl_main/src"
    "../${COPIED_SDK_PATH}/platform/service/udelay/inc"
)

target_compile_definitions(slc PUBLIC
    "DEBUG_EFM=1"
    "EFM32PG22C200F512IM40=1"
    "SL_CODE_COMPONENT_SYSTEM=system"
    "HFXO_FREQ=38400000"
    "SL_BOARD_NAME=\"BRD2503A\""
    "SL_BOARD_REV=\"A02\""
    "SL_CODE_COMPONENT_CLOCK_MANAGER=clock_manager"
    "SL_COMPONENT_CATALOG_PRESENT=1"
    "SL_CODE_COMPONENT_GPIO=gpio"
    "SL_CODE_COMPONENT_HAL_COMMON=hal_common"
    "SL_CODE_COMPONENT_HAL_GPIO=hal_gpio"
    "SL_CODE_COMPONENT_INTERRUPT_MANAGER=interrupt_manager"
    "CMSIS_NVIC_VIRTUAL=1"
    "CMSIS_NVIC_VIRTUAL_HEADER_FILE=\"cmsis_nvic_virtual.h\""
    "SL_CODE_COMPONENT_CORE=core"
)

target_link_libraries(slc PUBLIC
    "-Wl,--start-group"
    "gcc"
    "c"
    "m"
    "nosys"
    "-Wl,--end-group"
)
target_compile_options(slc PUBLIC
    $<$<COMPILE_LANGUAGE:C>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:C>:-mthumb>
    $<$<COMPILE_LANGUAGE:C>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:C>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:C>:-mcmse>
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
    $<$<COMPILE_LANGUAGE:C>:-Os>
    $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:C>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:C>:-g>
    $<$<COMPILE_LANGUAGE:C>:-fno-lto>
    $<$<COMPILE_LANGUAGE:C>:--specs=nano.specs>
    $<$<COMPILE_LANGUAGE:CXX>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:CXX>:-mthumb>
    $<$<COMPILE_LANGUAGE:CXX>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:CXX>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-mcmse>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
    $<$<COMPILE_LANGUAGE:CXX>:-Os>
    $<$<COMPILE_LANGUAGE:CXX>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:CXX>:-g>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-lto>
    $<$<COMPILE_LANGUAGE:CXX>:--specs=nano.specs>
    $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:ASM>:-mthumb>
    $<$<COMPILE_LANGUAGE:ASM>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:ASM>:-mfloat-abi=hard>
    "$<$<COMPILE_LANGUAGE:ASM>:SHELL:-x assembler-with-cpp>"
)

set(post_build_command )
set_property(TARGET slc PROPERTY C_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_EXTENSIONS OFF)

target_link_options(slc INTERFACE
    -mcpu=cortex-m33
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -T${CMAKE_CURRENT_LIST_DIR}/../autogen/linkerfile.ld
    --specs=nano.specs
    "SHELL:-Xlinker -Map=$<TARGET_FILE_DIR:EFM32PG22>/EFM32PG22.map"
    -fno-lto
    -Wl,--gc-sections
)

# BEGIN_SIMPLICITY_STUDIO_METADATA=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END_SIMPLICITY_STUDIO_METADATA