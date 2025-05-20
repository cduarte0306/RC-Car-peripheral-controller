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

#include "app_cli.h"

#include "project.h"

#include "RCUtils.h"
#include "rc_car.h"

#include "FreeRTOS.h"
#include "task.h"


#define EMBEDDED_CLI_IMPL
#include "embedded_cli/embedded_cli.h"


#if not defined(LOG_CLI)
    #define LOG_CLI  (uint8[2]) {1u, DEBUG_INFO}
#endif

 /**
 * @brief Macro to define UART reading method
 *      This macro allows the user to choose between using
 *      interrupts and polling to read cli user input
 *
 *      Values:
 *          0: Use polling
 *          1: Use interrupts
**/
#define USE_INTERRUPT   0u

/* Definations for CLI configurations */
#define CLI_BUFFER_SIZE     (256u)
#define RX_BUFFER_SIZE      (32u)                   // To store chars untill they're proccessed
#define CMD_BUFFER_SIZE     (32u)                   // To store current input that is not yet sent as command
#define HISTORY_BUFFER_SIZE (4 * CMD_BUFFER_SIZE)   // To store previous commands




/* [] END OF FILE */
