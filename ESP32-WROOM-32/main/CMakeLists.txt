
set(TOP_DIRECTORY
   /home/jack/git/MCUs
)

set( MCU_DIRECTORY ${TOP_DIRECTORY}/ESP32-WROOM-32 )

set(PROJECT_INCLUDE_DIRECTORIES
   ${MCU_DIRECTORY}/include
   ${MCU_DIRECTORY}/hal
   ${MCU_DIRECTORY}/device
   ${TOP_DIRECTORY}/hal_include
)

#sources
file(GLOB_RECURSE SRC_FILES
   ${MCU_DIRECTORY}/main/*.c
   ${MCU_DIRECTORY}/hal/*.c
   ${MCU_DIRECTORY}/device/*.c

)

idf_component_register(SRCS "main.c" ${SRC_FILES}
                    INCLUDE_DIRS "." ${PROJECT_INCLUDE_DIRECTORIES}) 
