/*******************************************************************************
* File Name: reset_timer_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "reset_timer.h"

static reset_timer_backupStruct reset_timer_backup;


/*******************************************************************************
* Function Name: reset_timer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  reset_timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void reset_timer_SaveConfig(void) 
{
    #if (!reset_timer_UsingFixedFunction)
        reset_timer_backup.TimerUdb = reset_timer_ReadCounter();
        reset_timer_backup.InterruptMaskValue = reset_timer_STATUS_MASK;
        #if (reset_timer_UsingHWCaptureCounter)
            reset_timer_backup.TimerCaptureCounter = reset_timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!reset_timer_UDB_CONTROL_REG_REMOVED)
            reset_timer_backup.TimerControlRegister = reset_timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: reset_timer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  reset_timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void reset_timer_RestoreConfig(void) 
{   
    #if (!reset_timer_UsingFixedFunction)

        reset_timer_WriteCounter(reset_timer_backup.TimerUdb);
        reset_timer_STATUS_MASK =reset_timer_backup.InterruptMaskValue;
        #if (reset_timer_UsingHWCaptureCounter)
            reset_timer_SetCaptureCount(reset_timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!reset_timer_UDB_CONTROL_REG_REMOVED)
            reset_timer_WriteControlRegister(reset_timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: reset_timer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  reset_timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void reset_timer_Sleep(void) 
{
    #if(!reset_timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(reset_timer_CTRL_ENABLE == (reset_timer_CONTROL & reset_timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            reset_timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            reset_timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    reset_timer_Stop();
    reset_timer_SaveConfig();
}


/*******************************************************************************
* Function Name: reset_timer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  reset_timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void reset_timer_Wakeup(void) 
{
    reset_timer_RestoreConfig();
    #if(!reset_timer_UDB_CONTROL_REG_REMOVED)
        if(reset_timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                reset_timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
