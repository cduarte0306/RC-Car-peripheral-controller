/*******************************************************************************
* File Name: Timer_encoder_PM.c
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

#include "Timer_encoder.h"

static Timer_encoder_backupStruct Timer_encoder_backup;


/*******************************************************************************
* Function Name: Timer_encoder_SaveConfig
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
*  Timer_encoder_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_encoder_SaveConfig(void) 
{
    #if (!Timer_encoder_UsingFixedFunction)
        Timer_encoder_backup.TimerUdb = Timer_encoder_ReadCounter();
        Timer_encoder_backup.InterruptMaskValue = Timer_encoder_STATUS_MASK;
        #if (Timer_encoder_UsingHWCaptureCounter)
            Timer_encoder_backup.TimerCaptureCounter = Timer_encoder_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_encoder_UDB_CONTROL_REG_REMOVED)
            Timer_encoder_backup.TimerControlRegister = Timer_encoder_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_encoder_RestoreConfig
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
*  Timer_encoder_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_encoder_RestoreConfig(void) 
{   
    #if (!Timer_encoder_UsingFixedFunction)

        Timer_encoder_WriteCounter(Timer_encoder_backup.TimerUdb);
        Timer_encoder_STATUS_MASK =Timer_encoder_backup.InterruptMaskValue;
        #if (Timer_encoder_UsingHWCaptureCounter)
            Timer_encoder_SetCaptureCount(Timer_encoder_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_encoder_UDB_CONTROL_REG_REMOVED)
            Timer_encoder_WriteControlRegister(Timer_encoder_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_encoder_Sleep
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
*  Timer_encoder_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_encoder_Sleep(void) 
{
    #if(!Timer_encoder_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_encoder_CTRL_ENABLE == (Timer_encoder_CONTROL & Timer_encoder_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_encoder_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_encoder_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_encoder_Stop();
    Timer_encoder_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_encoder_Wakeup
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
*  Timer_encoder_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_encoder_Wakeup(void) 
{
    Timer_encoder_RestoreConfig();
    #if(!Timer_encoder_UDB_CONTROL_REG_REMOVED)
        if(Timer_encoder_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_encoder_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
