#ifndef BOOTLOADER_SPI_H
#define BOOTLOADER_SPI_H

#include <stdint.h>

/**
 * @brief Start bootloader application
 * 
 */
void Bootloader_SPI_Start();

/**
 * @brief Set the reply buffer to the server
 * 
 * @param status 
 */
void Bootloader_SPI_SetResponse(uint8_t status);

#endif