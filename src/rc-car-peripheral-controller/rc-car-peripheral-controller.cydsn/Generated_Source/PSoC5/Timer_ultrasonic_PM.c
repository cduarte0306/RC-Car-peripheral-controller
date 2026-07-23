/*******************************************************************************
* File Name: Timer_ultrasonic_PM.c
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

#include "Timer_ultrasonic.h"

static Timer_ultrasonic_backupStruct Timer_ultrasonic_backup;


/*******************************************************************************
* Function Name: Timer_ultrasonic_SaveConfig
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
*  Timer_ultrasonic_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_ultrasonic_SaveConfig(void) 
{
    #if (!Timer_ultrasonic_UsingFixedFunction)
        Timer_ultrasonic_backup.TimerUdb = Timer_ultrasonic_ReadCounter();
        Timer_ultrasonic_backup.InterruptMaskValue = Timer_ultrasonic_STATUS_MASK;
        #if (Timer_ultrasonic_UsingHWCaptureCounter)
            Timer_ultrasonic_backup.TimerCaptureCounter = Timer_ultrasonic_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_ultrasonic_UDB_CONTROL_REG_REMOVED)
            Timer_ultrasonic_backup.TimerControlRegister = Timer_ultrasonic_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_ultrasonic_RestoreConfig
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
*  Timer_ultrasonic_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_ultrasonic_RestoreConfig(void) 
{   
    #if (!Timer_ultrasonic_UsingFixedFunction)

        Timer_ultrasonic_WriteCounter(Timer_ultrasonic_backup.TimerUdb);
        Timer_ultrasonic_STATUS_MASK =Timer_ultrasonic_backup.InterruptMaskValue;
        #if (Timer_ultrasonic_UsingHWCaptureCounter)
            Timer_ultrasonic_SetCaptureCount(Timer_ultrasonic_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_ultrasonic_UDB_CONTROL_REG_REMOVED)
            Timer_ultrasonic_WriteControlRegister(Timer_ultrasonic_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_ultrasonic_Sleep
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
*  Timer_ultrasonic_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_ultrasonic_Sleep(void) 
{
    #if(!Timer_ultrasonic_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_ultrasonic_CTRL_ENABLE == (Timer_ultrasonic_CONTROL & Timer_ultrasonic_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_ultrasonic_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_ultrasonic_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_ultrasonic_Stop();
    Timer_ultrasonic_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_ultrasonic_Wakeup
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
*  Timer_ultrasonic_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_ultrasonic_Wakeup(void) 
{
    Timer_ultrasonic_RestoreConfig();
    #if(!Timer_ultrasonic_UDB_CONTROL_REG_REMOVED)
        if(Timer_ultrasonic_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_ultrasonic_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
