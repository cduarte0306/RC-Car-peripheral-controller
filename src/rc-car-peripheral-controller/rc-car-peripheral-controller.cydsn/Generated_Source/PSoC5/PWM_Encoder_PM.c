/*******************************************************************************
* File Name: PWM_Encoder_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "PWM_Encoder.h"

static PWM_Encoder_backupStruct PWM_Encoder_backup;


/*******************************************************************************
* Function Name: PWM_Encoder_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_Encoder_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void PWM_Encoder_SaveConfig(void) 
{

    #if(!PWM_Encoder_UsingFixedFunction)
        #if(!PWM_Encoder_PWMModeIsCenterAligned)
            PWM_Encoder_backup.PWMPeriod = PWM_Encoder_ReadPeriod();
        #endif /* (!PWM_Encoder_PWMModeIsCenterAligned) */
        PWM_Encoder_backup.PWMUdb = PWM_Encoder_ReadCounter();
        #if (PWM_Encoder_UseStatus)
            PWM_Encoder_backup.InterruptMaskValue = PWM_Encoder_STATUS_MASK;
        #endif /* (PWM_Encoder_UseStatus) */

        #if(PWM_Encoder_DeadBandMode == PWM_Encoder__B_PWM__DBM_256_CLOCKS || \
            PWM_Encoder_DeadBandMode == PWM_Encoder__B_PWM__DBM_2_4_CLOCKS)
            PWM_Encoder_backup.PWMdeadBandValue = PWM_Encoder_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(PWM_Encoder_KillModeMinTime)
             PWM_Encoder_backup.PWMKillCounterPeriod = PWM_Encoder_ReadKillTime();
        #endif /* (PWM_Encoder_KillModeMinTime) */

        #if(PWM_Encoder_UseControl)
            PWM_Encoder_backup.PWMControlRegister = PWM_Encoder_ReadControlRegister();
        #endif /* (PWM_Encoder_UseControl) */
    #endif  /* (!PWM_Encoder_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PWM_Encoder_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_Encoder_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_Encoder_RestoreConfig(void) 
{
        #if(!PWM_Encoder_UsingFixedFunction)
            #if(!PWM_Encoder_PWMModeIsCenterAligned)
                PWM_Encoder_WritePeriod(PWM_Encoder_backup.PWMPeriod);
            #endif /* (!PWM_Encoder_PWMModeIsCenterAligned) */

            PWM_Encoder_WriteCounter(PWM_Encoder_backup.PWMUdb);

            #if (PWM_Encoder_UseStatus)
                PWM_Encoder_STATUS_MASK = PWM_Encoder_backup.InterruptMaskValue;
            #endif /* (PWM_Encoder_UseStatus) */

            #if(PWM_Encoder_DeadBandMode == PWM_Encoder__B_PWM__DBM_256_CLOCKS || \
                PWM_Encoder_DeadBandMode == PWM_Encoder__B_PWM__DBM_2_4_CLOCKS)
                PWM_Encoder_WriteDeadTime(PWM_Encoder_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(PWM_Encoder_KillModeMinTime)
                PWM_Encoder_WriteKillTime(PWM_Encoder_backup.PWMKillCounterPeriod);
            #endif /* (PWM_Encoder_KillModeMinTime) */

            #if(PWM_Encoder_UseControl)
                PWM_Encoder_WriteControlRegister(PWM_Encoder_backup.PWMControlRegister);
            #endif /* (PWM_Encoder_UseControl) */
        #endif  /* (!PWM_Encoder_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: PWM_Encoder_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_Encoder_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void PWM_Encoder_Sleep(void) 
{
    #if(PWM_Encoder_UseControl)
        if(PWM_Encoder_CTRL_ENABLE == (PWM_Encoder_CONTROL & PWM_Encoder_CTRL_ENABLE))
        {
            /*Component is enabled */
            PWM_Encoder_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            PWM_Encoder_backup.PWMEnableState = 0u;
        }
    #endif /* (PWM_Encoder_UseControl) */

    /* Stop component */
    PWM_Encoder_Stop();

    /* Save registers configuration */
    PWM_Encoder_SaveConfig();
}


/*******************************************************************************
* Function Name: PWM_Encoder_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_Encoder_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_Encoder_Wakeup(void) 
{
     /* Restore registers values */
    PWM_Encoder_RestoreConfig();

    if(PWM_Encoder_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        PWM_Encoder_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
