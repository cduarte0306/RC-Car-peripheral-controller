/*******************************************************************************
* File Name: PWM_trig_PM.c
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

#include "PWM_trig.h"

static PWM_trig_backupStruct PWM_trig_backup;


/*******************************************************************************
* Function Name: PWM_trig_SaveConfig
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
*  PWM_trig_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void PWM_trig_SaveConfig(void) 
{

    #if(!PWM_trig_UsingFixedFunction)
        #if(!PWM_trig_PWMModeIsCenterAligned)
            PWM_trig_backup.PWMPeriod = PWM_trig_ReadPeriod();
        #endif /* (!PWM_trig_PWMModeIsCenterAligned) */
        PWM_trig_backup.PWMUdb = PWM_trig_ReadCounter();
        #if (PWM_trig_UseStatus)
            PWM_trig_backup.InterruptMaskValue = PWM_trig_STATUS_MASK;
        #endif /* (PWM_trig_UseStatus) */

        #if(PWM_trig_DeadBandMode == PWM_trig__B_PWM__DBM_256_CLOCKS || \
            PWM_trig_DeadBandMode == PWM_trig__B_PWM__DBM_2_4_CLOCKS)
            PWM_trig_backup.PWMdeadBandValue = PWM_trig_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(PWM_trig_KillModeMinTime)
             PWM_trig_backup.PWMKillCounterPeriod = PWM_trig_ReadKillTime();
        #endif /* (PWM_trig_KillModeMinTime) */

        #if(PWM_trig_UseControl)
            PWM_trig_backup.PWMControlRegister = PWM_trig_ReadControlRegister();
        #endif /* (PWM_trig_UseControl) */
    #endif  /* (!PWM_trig_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PWM_trig_RestoreConfig
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
*  PWM_trig_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_trig_RestoreConfig(void) 
{
        #if(!PWM_trig_UsingFixedFunction)
            #if(!PWM_trig_PWMModeIsCenterAligned)
                PWM_trig_WritePeriod(PWM_trig_backup.PWMPeriod);
            #endif /* (!PWM_trig_PWMModeIsCenterAligned) */

            PWM_trig_WriteCounter(PWM_trig_backup.PWMUdb);

            #if (PWM_trig_UseStatus)
                PWM_trig_STATUS_MASK = PWM_trig_backup.InterruptMaskValue;
            #endif /* (PWM_trig_UseStatus) */

            #if(PWM_trig_DeadBandMode == PWM_trig__B_PWM__DBM_256_CLOCKS || \
                PWM_trig_DeadBandMode == PWM_trig__B_PWM__DBM_2_4_CLOCKS)
                PWM_trig_WriteDeadTime(PWM_trig_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(PWM_trig_KillModeMinTime)
                PWM_trig_WriteKillTime(PWM_trig_backup.PWMKillCounterPeriod);
            #endif /* (PWM_trig_KillModeMinTime) */

            #if(PWM_trig_UseControl)
                PWM_trig_WriteControlRegister(PWM_trig_backup.PWMControlRegister);
            #endif /* (PWM_trig_UseControl) */
        #endif  /* (!PWM_trig_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: PWM_trig_Sleep
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
*  PWM_trig_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void PWM_trig_Sleep(void) 
{
    #if(PWM_trig_UseControl)
        if(PWM_trig_CTRL_ENABLE == (PWM_trig_CONTROL & PWM_trig_CTRL_ENABLE))
        {
            /*Component is enabled */
            PWM_trig_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            PWM_trig_backup.PWMEnableState = 0u;
        }
    #endif /* (PWM_trig_UseControl) */

    /* Stop component */
    PWM_trig_Stop();

    /* Save registers configuration */
    PWM_trig_SaveConfig();
}


/*******************************************************************************
* Function Name: PWM_trig_Wakeup
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
*  PWM_trig_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_trig_Wakeup(void) 
{
     /* Restore registers values */
    PWM_trig_RestoreConfig();

    if(PWM_trig_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        PWM_trig_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
