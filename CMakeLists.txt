cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(ARM_TOOLCHAIN "/opt/gcc-arm-none-eabi-6-2017-q2-update/bin/") # Path to folder containing arm-none-eabi-gcc
set(QEMU_DIR "/home/jonass/dev/bachelor/qemu_edi/build/") # Path to folder containing qemu-system-arm
set(OPENOCD_DIR "/usr/bin/") # Path to folder containing openocd

include(cmake/toolchain.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

project(EdiTests C CXX ASM${DIALECT})

set(CMAKE_CONFIGURATION_TYPES Debug;Release)

add_subdirectory(app)
add_subdirectory(libs)
