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

add_subdirectory(src)
