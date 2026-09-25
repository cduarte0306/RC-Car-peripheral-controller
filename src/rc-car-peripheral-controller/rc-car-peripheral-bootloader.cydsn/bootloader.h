#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint-gcc.h>
#include "MotorBootloaderDefs.h"

/**
 * @brief Enter the bootloader stage
 * 
 */
void Bootloader_Start();

/**
 * @brief Set the Comms Started flag to prevent from jumping to the app
 * 
 */
inline void SetCommsStarted();

#endif