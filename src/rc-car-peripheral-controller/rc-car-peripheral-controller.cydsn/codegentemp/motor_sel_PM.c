/*******************************************************************************
* File Name: motor_sel_PM.c
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

#include "motor_sel.h"

/* Check for removal by optimization */
#if !defined(motor_sel_Sync_ctrl_reg__REMOVED)

static motor_sel_BACKUP_STRUCT  motor_sel_backup = {0u};

    
/*******************************************************************************
* Function Name: motor_sel_SaveConfig
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
void motor_sel_SaveConfig(void) 
{
    motor_sel_backup.controlState = motor_sel_Control;
}


/*******************************************************************************
* Function Name: motor_sel_RestoreConfig
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
void motor_sel_RestoreConfig(void) 
{
     motor_sel_Control = motor_sel_backup.controlState;
}


/*******************************************************************************
* Function Name: motor_sel_Sleep
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
void motor_sel_Sleep(void) 
{
    motor_sel_SaveConfig();
}


/*******************************************************************************
* Function Name: motor_sel_Wakeup
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
void motor_sel_Wakeup(void)  
{
    motor_sel_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
