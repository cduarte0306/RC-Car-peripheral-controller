cmake_minimum_required(VERSION 3.20)

# --- Cross-compilation setup (must be set before project()) ---
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# There is no host OS/libc to link a full test executable against; only ask
# CMake to confirm the compiler can produce object files.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(rc-car-peripheral-controller C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(COMMON_DIR ${PROJECT_ROOT}/src/common)

# --- Flash layout shared between the bootloader and the application ---
# The bootloader owns [0, CY_BOOTLOADER_SIZE); the application is linked to
# start right after it (CY_APPL_ORIGIN in its cm3gcc.ld). Must stay row-aligned
# (CY_FLASH_ROW_SIZE = 256 in cm3gcc.ld) and is the single source of truth for
# both targets below, so the two images can never silently overlap in flash.
set(CY_BOOTLOADER_SIZE 0x8000)

add_subdirectory(src/rc-car-peripheral-controller/rc-car-peripheral-controller.cydsn)
add_subdirectory(src/rc-car-peripheral-controller/rc-car-peripheral-bootloader.cydsn)
