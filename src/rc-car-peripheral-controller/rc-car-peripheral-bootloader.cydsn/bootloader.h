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
 * @brief Set the payload and transfer structure
 * 
 * @param payload Pointer to the payload data
 * @param xfer Pointer to the transfer structure containing command and CRC
 */
void SetMessage(const uint8_t* payload, const tBlXfer* xfer);

#endif