/*******************************************************************************
* File Name: start_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "start.h"

/* Check for removal by optimization */
#if !defined(start_Sync_ctrl_reg__REMOVED)

static start_BACKUP_STRUCT  start_backup = {0u};

    
/*******************************************************************************
* Function Name: start_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void start_SaveConfig(void) 
{
    start_backup.controlState = start_Control;
}


/*******************************************************************************
* Function Name: start_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void start_RestoreConfig(void) 
{
     start_Control = start_backup.controlState;
}


/*******************************************************************************
* Function Name: start_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void start_Sleep(void) 
{
    start_SaveConfig();
}


/*******************************************************************************
* Function Name: start_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void start_Wakeup(void)  
{
    start_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
