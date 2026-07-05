if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
set(TOP_DIRECTORY
   C:/projects/MCUs
)
else ()
set(TOP_DIRECTORY
   /home/jack/git/MCUs
)
endif()


set( MCU_DIRECTORY ${TOP_DIRECTORY}/StickS3 )

set(PROJECT_INCLUDE_DIRECTORIES
   ${MCU_DIRECTORY}/include
   ${MCU_DIRECTORY}/hal
   ${MCU_DIRECTORY}/device
   ${TOP_DIRECTORY}/hal_include
   ${TOP_DIRECTORY}/drivers
)

#sources
file(GLOB_RECURSE SRC_FILES
   ${MCU_DIRECTORY}/main/*.c
   ${MCU_DIRECTORY}/hal/*.c
   ${MCU_DIRECTORY}/device/*.c
   ${TOP_DIRECTORY}/drivers/*.c
   
)

idf_component_register(SRCS "main.c" ${SRC_FILES}
                    PRIV_REQUIRES esp_driver_spi esp_driver_gpio esp_driver_i2c esp_driver_ledc esp_driver_gptimer esp_timer 
                    REQUIRES esp_psram
                    INCLUDE_DIRS "." ${PROJECT_INCLUDE_DIRECTORIES}
                    ) 

