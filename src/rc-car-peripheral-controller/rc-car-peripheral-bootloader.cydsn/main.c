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
#include "vers.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    SPIS_Start();
    SPIS_ClearFIFO();
    SPIS_ClearRxBuffer();
    SPIS_ClearTxBuffer();
    PWM_Start();

    UART_Debug_Start();
    uint8_t major, minor, build;
    getVers(&major, &minor, &build);
    vPrintf("RC Car Bootloader version: %d.%d.%d\r\n",
                    major, minor, build);

    // We should never reach this point
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
