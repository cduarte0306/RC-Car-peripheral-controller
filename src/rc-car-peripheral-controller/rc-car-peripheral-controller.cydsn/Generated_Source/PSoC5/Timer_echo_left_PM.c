/*******************************************************************************
* File Name: Timer_echo_left_PM.c
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

#include "Timer_echo_left.h"

static Timer_echo_left_backupStruct Timer_echo_left_backup;


/*******************************************************************************
* Function Name: Timer_echo_left_SaveConfig
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
*  Timer_echo_left_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_echo_left_SaveConfig(void) 
{
    #if (!Timer_echo_left_UsingFixedFunction)
        Timer_echo_left_backup.TimerUdb = Timer_echo_left_ReadCounter();
        Timer_echo_left_backup.InterruptMaskValue = Timer_echo_left_STATUS_MASK;
        #if (Timer_echo_left_UsingHWCaptureCounter)
            Timer_echo_left_backup.TimerCaptureCounter = Timer_echo_left_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_echo_left_UDB_CONTROL_REG_REMOVED)
            Timer_echo_left_backup.TimerControlRegister = Timer_echo_left_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_echo_left_RestoreConfig
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
*  Timer_echo_left_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_echo_left_RestoreConfig(void) 
{   
    #if (!Timer_echo_left_UsingFixedFunction)

        Timer_echo_left_WriteCounter(Timer_echo_left_backup.TimerUdb);
        Timer_echo_left_STATUS_MASK =Timer_echo_left_backup.InterruptMaskValue;
        #if (Timer_echo_left_UsingHWCaptureCounter)
            Timer_echo_left_SetCaptureCount(Timer_echo_left_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_echo_left_UDB_CONTROL_REG_REMOVED)
            Timer_echo_left_WriteControlRegister(Timer_echo_left_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_echo_left_Sleep
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
*  Timer_echo_left_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_echo_left_Sleep(void) 
{
    #if(!Timer_echo_left_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_echo_left_CTRL_ENABLE == (Timer_echo_left_CONTROL & Timer_echo_left_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_echo_left_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_echo_left_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_echo_left_Stop();
    Timer_echo_left_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_echo_left_Wakeup
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
*  Timer_echo_left_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_echo_left_Wakeup(void) 
{
    Timer_echo_left_RestoreConfig();
    #if(!Timer_echo_left_UDB_CONTROL_REG_REMOVED)
        if(Timer_echo_left_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_echo_left_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
