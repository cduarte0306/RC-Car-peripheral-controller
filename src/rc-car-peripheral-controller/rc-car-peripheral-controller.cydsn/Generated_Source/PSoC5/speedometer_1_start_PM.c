/*******************************************************************************
* File Name: speedometer_1_start_PM.c
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

#include "speedometer_1_start.h"

/* Check for removal by optimization */
#if !defined(speedometer_1_start_Sync_ctrl_reg__REMOVED)

static speedometer_1_start_BACKUP_STRUCT  speedometer_1_start_backup = {0u};

    
/*******************************************************************************
* Function Name: speedometer_1_start_SaveConfig
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
void speedometer_1_start_SaveConfig(void) 
{
    speedometer_1_start_backup.controlState = speedometer_1_start_Control;
}


/*******************************************************************************
* Function Name: speedometer_1_start_RestoreConfig
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
void speedometer_1_start_RestoreConfig(void) 
{
     speedometer_1_start_Control = speedometer_1_start_backup.controlState;
}


/*******************************************************************************
* Function Name: speedometer_1_start_Sleep
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
void speedometer_1_start_Sleep(void) 
{
    speedometer_1_start_SaveConfig();
}


/*******************************************************************************
* Function Name: speedometer_1_start_Wakeup
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
void speedometer_1_start_Wakeup(void)  
{
    speedometer_1_start_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
