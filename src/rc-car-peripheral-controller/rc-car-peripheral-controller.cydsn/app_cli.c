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

#include <stdlib.h>
#include "app_cli.h"

#include "project.h"

#include "RCUtils.h"
#include "rc_car.h"

#include "rc_car.h"
#include "motor_driver.h"

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


static uint8 is_init = FALSE;
static EmbeddedCli *CLI;

static uint32 ticks = 0;
static uint32 timer = 0;
static uint32 tick_count = 0;


/* Function prototypes */
#if ( USE_INTERRUPT == 1 )
static void Rx_ISR_InterruptCallback( void );
#endif

static void write_char( EmbeddedCli *cli, char c );
static void cmdReadReg( EmbeddedCli *cli, char *args, void *context );
static void cmdWriteReg( EmbeddedCli *cli, char *args, void *context );
static void cmdSetMotorState( EmbeddedCli *cli, char *args, void *context );
static void cmdSetMotorSpeed( EmbeddedCli *cli, char *args, void *context );
static void cmdSetMotorOnOffState( EmbeddedCli *cli, char *args, void *context );
static void cmdReset( EmbeddedCli *cli, char *args, void *context );


/* For storing result from function calls */
static uint8 ret;

/* CLI bindings */
static const CliCommandBinding cmd_bindings[] = {
     /* CliCommandBinding:
     *     char *name:
     *          Name of command to bind. Should not be NULL.
     *
     *     char *help:
     *          Help string that will be displayed when "help <cmd>" is executed
     *
     *     bool tokenizeArgs:
     *          Flag to perform tokenization before calling binding function
     *
     *     void *context:
     *          Pointer to any specific app context that is required for this binding
     *
     *     uint8 enabled:
     *          Whether this command is enabled for use
     *
     *     uint8 hidden:
     *          Whether this command this command is hidden from help and autocompete
     *
     *     void (*binding)(...):
     *          Binding function for when command is received.
     */
    (CliCommandBinding){
        "read-cmd-reg",
        
        "Reads from the register\r\n"
            "\t\tread-cmd <register>\r\n",
        
        FALSE, NULL, TRUE, FALSE, cmdReadReg
    },
    (CliCommandBinding){
        "write-cmd-reg",
        
        "Write to the register\r\n"
            "\t\twrite-cmd <register> <value>\r\n",
        
        TRUE, NULL, TRUE, FALSE, cmdWriteReg
    },
    (CliCommandBinding){
        "set_motor_state",
        
        "Configures the motor state\r\n"
        "0: Manual\r\n"
        "1: Automatic\r\n"
            "\t\tset_motor_state <state>\r\n",
        
        TRUE, NULL, TRUE, FALSE, cmdSetMotorState
    },
    (CliCommandBinding){
        "set_motor_on_off_state",
        
        "Configures the motor ON/OFF state\r\n"
        "0: Manual\r\n"
        "1: Automatic\r\n"
            "\t\tset_motor_on_off_state <state>\r\n",
        
        TRUE, NULL, TRUE, FALSE, cmdSetMotorOnOffState
    },
    (CliCommandBinding){
        "set_motor_speed_sp",
        
        "Configures motor speed setpoint\r\n"
            "\t\tset_motor_speed_sp <speed>\r\n",
        
        TRUE, NULL, TRUE, FALSE, cmdSetMotorSpeed
    },
    (CliCommandBinding){
        "reset",
        
        "Resets the unit",
        
        TRUE, NULL, TRUE, FALSE, cmdReset
    },
};


/**
 * @brief Configures Cli (called by main)
 *      This function handles cli configuration, and
 *      prints a welcome message to the user
 *
 *      This function could fail if:
 *          There is not enough heap space for cli
**/
uint8 APP_CLI_init( void )
{
    const uint8 num_cmds = sizeof( cmd_bindings )/sizeof( cmd_bindings[0] );
    uint8 ret;
    uint16 req_size;

    vLoggingPrintf(DEBUG_TRACE, LOG_CLI, "cli: init | enter\r\n");

    EmbeddedCliConfig *config = embeddedCliDefaultConfig();
    if ( config == NULL )
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_CLI, "cli: init | `config` is nullptr\r\n");
        return RET_FAIL;
    }

    config->cliBuffer           = NULL;
    config->cliBufferSize       = CLI_BUFFER_SIZE * sizeof( CLI_UINT );
    config->rxBufferSize        = RX_BUFFER_SIZE;
    config->cmdBufferSize       = CMD_BUFFER_SIZE;
    config->historyBufferSize   = HISTORY_BUFFER_SIZE;
    config->maxBindingCount     = num_cmds + 1;
    config->enableAutoComplete  = true;

    vLoggingPrintf(DEBUG_INFO, LOG_CLI, "cli: CLI_init | Initilizing CLI...\r\n");

    req_size = embeddedCliRequiredSize(config);
    vLoggingPrintf(DEBUG_INFO, LOG_CLI, "    required memory: %i bytes\r\n", req_size);
    vLoggingPrintf(DEBUG_INFO, LOG_CLI, "    available memory: %i bytes\r\n", CYDEV_HEAP_SIZE);

    if (CYDEV_HEAP_SIZE < req_size)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_CLI, "cli: CLI_init | err: unsufficient mem - Try alloc more heap mem\r\n");
        return RET_FAIL;
    }

    #if ( USE_INTERRUPT == 1 )
    Rx_ISR_StartEx( Rx_ISR_InterruptCallback );
    #endif

    CLI = embeddedCliNew( config );
    if ( CLI == NULL )
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_CLI, "cli: CLI_init | err: embeddedCliNew - Try alloc more heap mem\r\n");
        return RET_FAIL;
    }

    vLoggingPrintf(DEBUG_INFO, LOG_CLI, "cli: CLI_init | Num cmds to register: %i\r\n", num_cmds);

    for ( uint8 i = 0; i<num_cmds; i++ )
    {
        ret = embeddedCliAddBinding( CLI, cmd_bindings[i] );
        if (ret == FALSE)
        {
            vLoggingPrintf(DEBUG_ERROR, LOG_CLI, "cli: CLI_init | err: embeddedCliAddBinding - Try increasing binding count\r\n");
            return RET_FAIL;
        }

        if (CLI == NULL)
        {
            vLoggingPrintf(DEBUG_ERROR, LOG_CLI, "cli: CLI_init | err: cli=null - Possible mem corruption\r\n");
            return RET_FAIL;
        }
    }

    CLI->writeChar = write_char;

    is_init = TRUE;

    vLoggingPrintf(DEBUG_TRACE, LOG_CLI, "cli: init | exit\r\n");
    return RET_PASS;
}


/**
 * @brief Processes user input
 *      If interrupts aren't set it will check if there is any
 *      input on RX pin.
 *      And it processes cli instance
**/
void APP_CLI_update( void )
{        
    if (is_init == FALSE)
    {
        return;
    }

    if (CLI == NULL)
    {
        return;
    }
    
    ticks++;
    if (xGetElapsed(timer) > ONE_SEC)
    {
        timer = xGetTimestamp();

        tick_count = ticks;
        ticks = 0;
    }

    #if ( USE_INTERRUPT == 0 )
    while ( (UART_Debug_RXSTATUS_REG & UART_Debug_RX_STS_FIFO_NOTEMPTY) > 0 )
    {
        embeddedCliReceiveChar( CLI, UART_Debug_RXDATA_REG );
    }
    #endif

    embeddedCliProcess( CLI );
}


/**
 * @brief Processes write commands by informing cli to use UART_PutChar
 *      Writes a character to the UART Tx (Transmit) buffer
 *
 * @param[in] cli Handle to embedded CLI
 * @param[in] c Character to be sent
**/
static void write_char( EmbeddedCli *cli, char c )
{
    (void) cli;

    UART_Debug_PutChar( c );
}


#if ( USE_INTERRUPT == 1 )
/**
 * @brief Processes UART Rx interrupt call
 *      This function will run when an interrupt is called
 *      on data recieved and read data until UART Recieve
 *      FIFO is empty
**/
static void Rx_ISR_InterruptCallback( void )
{
    while ( (Cli_UART_RXSTATUS_REG & Cli_UART_RX_STS_FIFO_NOTEMPTY) > 0 )
    {
        embeddedCliReceiveChar( CLI, UART_RXDATA_REG );
    }

    Cli_Rx_ISR_ClearPending();
}
#endif


static void cmdReadReg( EmbeddedCli *cli, char *args, void *context )
{
    ( void ) cli;
    ( void ) context;
    
    const char *arg1 = embeddedCliGetToken(args, 1);
    
    if (arg1 == NULL)
    {
        vPrintf("Error: No argument provided\r\n");
        return;
    }
    
    char *endPtr = NULL;
    uint8_t reg = strtoul(arg1, &endPtr, 10);
    if (*endPtr != '\0')
    {
        vPrintf("Error: Invalid numeric argument '%s'\n", arg1);
        return;
    }
    
    const regMapType* regMap = getRegRef();
    if (regMap == NULL)
    {
        return;
    }
    
    vPrintf("Reg value: %lu\r\n", regMap[reg].data.u32);
}


static void cmdWriteReg( EmbeddedCli *cli, char *args, void *context )
{
    ( void ) cli;
    ( void ) context;
    
    const char *reg_ = embeddedCliGetToken(args, 1);
    
    if (reg_ == NULL)
    {
        vPrintf("Error: No argument provided\r\n");
        return;
    }
    
    const char *val_ = embeddedCliGetToken(args, 2);
    
    if (val_ == NULL)
    {
        vPrintf("Error: No argument provided\r\n");
        return;
    }
    
    char *endPtr = NULL;
    uint8_t reg = strtoul(reg_, &endPtr, 10);
    if (*endPtr != '\0')
    {
        vPrintf("Error: Invalid numeric argument '%s'\n", reg_);
        return;
    }
    
    uint32_t val = strtoul(val_, &endPtr, 10);
    if (*endPtr != '\0')
    {
        vPrintf("Error: Invalid register value '%s'\n", reg_);
        return;
    }
    
    regMapType* regMap = getRegRef();
    if (regMap == NULL)
    {
        return;
    }
    
    regMap[reg].data.u32 = val;
}


static void cmdSetMotorState( EmbeddedCli *cli, char *args, void *context )
{
    ( void ) cli;
    ( void ) context;
    
    const char *arg1 = embeddedCliGetToken(args, 1);
    
    if (arg1 == NULL)
    {
        vPrintf("Error: No argument provided");
        return;
    }
    
    char *endPtr = NULL;
    uint8_t state = strtoul(arg1, &endPtr, 10);
    if (*endPtr != '\0')
    {
        vPrintf("Error: Invalid numeric argument '%s'\n", arg1);
        return;
    }
    
    regMapType* regMap = getRegRef();
    if (regMap == NULL)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_CLI, "app: cmdSetMotorState | err: Could not read register map\r\n");
        return;
    }
    
    regMap[REG_SET_MOTOR_CTRL_STATUS].data.u32 = state;
}


static void cmdSetMotorSpeed( EmbeddedCli *cli, char *args, void *context )
{
    ( void ) cli;
    ( void ) context;
    
    const char *arg1 = embeddedCliGetToken(args, 1);
    
    if (arg1 == NULL)
    {
        vPrintf("Error: No argument provided");
        return;
    }
    
    char *endPtr = NULL;
    uint32_t speed = strtoul(arg1, &endPtr, 10);
    if (*endPtr != '\0')
    {
        vPrintf("Error: Invalid numeric argument '%s'\n", arg1);
        return;
    }

    regMapType* regMap = getRegRef();
    if (regMap == NULL)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_CLI, "app: cmdSetMotorSpeed | err: Could not read register map\r\n");
        return;
    }
    
    regMap[REG_SPEED_SETPOINT].data.u32 = speed;
}


static void cmdSetMotorOnOffState( EmbeddedCli *cli, char *args, void *context )
{
    ( void ) cli;
    ( void ) context;
    
    const char *arg1 = embeddedCliGetToken(args, 1);
    
    if (arg1 == NULL)
    {
        vPrintf("Error: No argument provided");
        return;
    }
    
    char *endPtr = NULL;
    uint8_t state = strtoul(arg1, &endPtr, 10);
    if (*endPtr != '\0')
    {
        vPrintf("Error: Invalid numeric argument '%s'\n", arg1);
        return;
    }

    regMapType* regMap = getRegRef();
    if (regMap == NULL)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_CLI, "app: cmdSetMotorSpeed | err: Could not read register map\r\n");
        return;
    }
    
    regMap[REG_MOTOR_ONOFF_STATE].data.u32 = state;
}


static void cmdReset( EmbeddedCli *cli, char *args, void *context )
{
    ( void ) cli;
    ( void ) context;
    ( void ) args;
    
    vLoggingPrintf(DEBUG_INFO, LOG_CLI, "app: cmdReset | Resetting... \r\n");
    CY_LIB_RESET_CR2_REG |= CY_LIB_RESET_CR2_RESET;
}

/* [] END OF FILE */
