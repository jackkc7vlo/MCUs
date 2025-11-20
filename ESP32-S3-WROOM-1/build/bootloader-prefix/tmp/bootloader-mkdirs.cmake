# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/jack/esp/v6.0/esp-idf/components/bootloader/subproject"
  "/home/jack/git/MCUs/ESP32-S3-WROOM-1/build/bootloader"
  "/home/jack/git/MCUs/ESP32-S3-WROOM-1/build/bootloader-prefix"
  "/home/jack/git/MCUs/ESP32-S3-WROOM-1/build/bootloader-prefix/tmp"
  "/home/jack/git/MCUs/ESP32-S3-WROOM-1/build/bootloader-prefix/src/bootloader-stamp"
  "/home/jack/git/MCUs/ESP32-S3-WROOM-1/build/bootloader-prefix/src"
  "/home/jack/git/MCUs/ESP32-S3-WROOM-1/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/jack/git/MCUs/ESP32-S3-WROOM-1/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/jack/git/MCUs/ESP32-S3-WROOM-1/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
