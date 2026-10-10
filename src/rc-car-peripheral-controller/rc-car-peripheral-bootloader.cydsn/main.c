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
#include "project.h"
#include "logging.h"
#include "bootloader.h"
#include "vers.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    uint8 staticBits = (pwm_out_DR & (uint8)(~pwm_out_MASK));
    pwm_out_DR = staticBits | ((uint8)(0 << pwm_out_SHIFT) & pwm_out_MASK);

    staticBits = (steer_out_DR & (uint8)(~steer_out_MASK));
    steer_out_DR = staticBits | ((uint8)(0 << steer_out_SHIFT) & steer_out_MASK);

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    SPIS_Start();
    SPIS_ClearFIFO();
    SPIS_ClearRxBuffer();
    SPIS_ClearTxBuffer();

    UART_Debug_Start();
    uint8_t major, minor, build;
    getVers(&major, &minor, &build);
    vLoggingPrintf(DEBUG_INFO, LOG_PSOC, " ===============================\r\n\r\n");
    vLoggingPrintf(DEBUG_INFO, LOG_PSOC,"RC Car Bootloader version: V%d.%d.%d\r\n",
            major, minor, build);
    Bootloader_Start();
        
    // We should never reach this point
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
