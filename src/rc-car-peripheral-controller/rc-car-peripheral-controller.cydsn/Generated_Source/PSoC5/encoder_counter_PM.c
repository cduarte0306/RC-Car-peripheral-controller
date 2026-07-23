/*******************************************************************************
* File Name: encoder_counter_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "encoder_counter.h"

static encoder_counter_backupStruct encoder_counter_backup;


/*******************************************************************************
* Function Name: encoder_counter_SaveConfig
********************************************************************************
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
*  encoder_counter_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void encoder_counter_SaveConfig(void) 
{
    #if (!encoder_counter_UsingFixedFunction)

        encoder_counter_backup.CounterUdb = encoder_counter_ReadCounter();

        #if(!encoder_counter_ControlRegRemoved)
            encoder_counter_backup.CounterControlRegister = encoder_counter_ReadControlRegister();
        #endif /* (!encoder_counter_ControlRegRemoved) */

    #endif /* (!encoder_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: encoder_counter_RestoreConfig
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
*  encoder_counter_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void encoder_counter_RestoreConfig(void) 
{      
    #if (!encoder_counter_UsingFixedFunction)

       encoder_counter_WriteCounter(encoder_counter_backup.CounterUdb);

        #if(!encoder_counter_ControlRegRemoved)
            encoder_counter_WriteControlRegister(encoder_counter_backup.CounterControlRegister);
        #endif /* (!encoder_counter_ControlRegRemoved) */

    #endif /* (!encoder_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: encoder_counter_Sleep
********************************************************************************
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
*  encoder_counter_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void encoder_counter_Sleep(void) 
{
    #if(!encoder_counter_ControlRegRemoved)
        /* Save Counter's enable state */
        if(encoder_counter_CTRL_ENABLE == (encoder_counter_CONTROL & encoder_counter_CTRL_ENABLE))
        {
            /* Counter is enabled */
            encoder_counter_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            encoder_counter_backup.CounterEnableState = 0u;
        }
    #else
        encoder_counter_backup.CounterEnableState = 1u;
        if(encoder_counter_backup.CounterEnableState != 0u)
        {
            encoder_counter_backup.CounterEnableState = 0u;
        }
    #endif /* (!encoder_counter_ControlRegRemoved) */
    
    encoder_counter_Stop();
    encoder_counter_SaveConfig();
}


/*******************************************************************************
* Function Name: encoder_counter_Wakeup
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
*  encoder_counter_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void encoder_counter_Wakeup(void) 
{
    encoder_counter_RestoreConfig();
    #if(!encoder_counter_ControlRegRemoved)
        if(encoder_counter_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            encoder_counter_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!encoder_counter_ControlRegRemoved) */
    
}


/* [] END OF FILE */
