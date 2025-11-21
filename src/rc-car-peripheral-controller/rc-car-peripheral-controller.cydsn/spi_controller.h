/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef SPI_CONTROLLER_H
#define SPI_CONTROLLER_H

#include <stdint.h>

#define SPI_CONNECTION_TIMEOUT  5
    

uint8_t SPI_controller_start(void);
uint8_t SPIGetConnectionStatus(void);
void SPI_controller_process(void);
    
#endif

/* [] END OF FILE */
