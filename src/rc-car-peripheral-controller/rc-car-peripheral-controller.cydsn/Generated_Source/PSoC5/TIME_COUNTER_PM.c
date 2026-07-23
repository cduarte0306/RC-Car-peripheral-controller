/*******************************************************************************
* File Name: TIME_COUNTER_PM.c
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

#include "TIME_COUNTER.h"

static TIME_COUNTER_backupStruct TIME_COUNTER_backup;


/*******************************************************************************
* Function Name: TIME_COUNTER_SaveConfig
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
*  TIME_COUNTER_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void TIME_COUNTER_SaveConfig(void) 
{
    #if (!TIME_COUNTER_UsingFixedFunction)
        TIME_COUNTER_backup.TimerUdb = TIME_COUNTER_ReadCounter();
        TIME_COUNTER_backup.InterruptMaskValue = TIME_COUNTER_STATUS_MASK;
        #if (TIME_COUNTER_UsingHWCaptureCounter)
            TIME_COUNTER_backup.TimerCaptureCounter = TIME_COUNTER_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!TIME_COUNTER_UDB_CONTROL_REG_REMOVED)
            TIME_COUNTER_backup.TimerControlRegister = TIME_COUNTER_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: TIME_COUNTER_RestoreConfig
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
*  TIME_COUNTER_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void TIME_COUNTER_RestoreConfig(void) 
{   
    #if (!TIME_COUNTER_UsingFixedFunction)

        TIME_COUNTER_WriteCounter(TIME_COUNTER_backup.TimerUdb);
        TIME_COUNTER_STATUS_MASK =TIME_COUNTER_backup.InterruptMaskValue;
        #if (TIME_COUNTER_UsingHWCaptureCounter)
            TIME_COUNTER_SetCaptureCount(TIME_COUNTER_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!TIME_COUNTER_UDB_CONTROL_REG_REMOVED)
            TIME_COUNTER_WriteControlRegister(TIME_COUNTER_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: TIME_COUNTER_Sleep
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
*  TIME_COUNTER_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void TIME_COUNTER_Sleep(void) 
{
    #if(!TIME_COUNTER_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(TIME_COUNTER_CTRL_ENABLE == (TIME_COUNTER_CONTROL & TIME_COUNTER_CTRL_ENABLE))
        {
            /* Timer is enabled */
            TIME_COUNTER_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            TIME_COUNTER_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    TIME_COUNTER_Stop();
    TIME_COUNTER_SaveConfig();
}


/*******************************************************************************
* Function Name: TIME_COUNTER_Wakeup
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
*  TIME_COUNTER_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void TIME_COUNTER_Wakeup(void) 
{
    TIME_COUNTER_RestoreConfig();
    #if(!TIME_COUNTER_UDB_CONTROL_REG_REMOVED)
        if(TIME_COUNTER_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                TIME_COUNTER_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
