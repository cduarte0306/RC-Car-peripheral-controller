#include "bootloader.h"
#include "RCUtils.h"

volatile uint8_t commsStarted = FALSE;

CY_ISR(txHandler)
{
    
}

CY_ISR(end_of_message_handler)
{
    commsStarted = TRUE;
    end_of_message_ClearPending();
}

void Bootloader_Start()
{
    
}