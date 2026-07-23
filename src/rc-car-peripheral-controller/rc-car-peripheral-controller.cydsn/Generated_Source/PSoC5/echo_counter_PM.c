/*******************************************************************************
* File Name: echo_counter_PM.c  
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

#include "echo_counter.h"

static echo_counter_backupStruct echo_counter_backup;


/*******************************************************************************
* Function Name: echo_counter_SaveConfig
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
*  echo_counter_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void echo_counter_SaveConfig(void) 
{
    #if (!echo_counter_UsingFixedFunction)

        echo_counter_backup.CounterUdb = echo_counter_ReadCounter();

        #if(!echo_counter_ControlRegRemoved)
            echo_counter_backup.CounterControlRegister = echo_counter_ReadControlRegister();
        #endif /* (!echo_counter_ControlRegRemoved) */

    #endif /* (!echo_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: echo_counter_RestoreConfig
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
*  echo_counter_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void echo_counter_RestoreConfig(void) 
{      
    #if (!echo_counter_UsingFixedFunction)

       echo_counter_WriteCounter(echo_counter_backup.CounterUdb);

        #if(!echo_counter_ControlRegRemoved)
            echo_counter_WriteControlRegister(echo_counter_backup.CounterControlRegister);
        #endif /* (!echo_counter_ControlRegRemoved) */

    #endif /* (!echo_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: echo_counter_Sleep
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
*  echo_counter_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void echo_counter_Sleep(void) 
{
    #if(!echo_counter_ControlRegRemoved)
        /* Save Counter's enable state */
        if(echo_counter_CTRL_ENABLE == (echo_counter_CONTROL & echo_counter_CTRL_ENABLE))
        {
            /* Counter is enabled */
            echo_counter_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            echo_counter_backup.CounterEnableState = 0u;
        }
    #else
        echo_counter_backup.CounterEnableState = 1u;
        if(echo_counter_backup.CounterEnableState != 0u)
        {
            echo_counter_backup.CounterEnableState = 0u;
        }
    #endif /* (!echo_counter_ControlRegRemoved) */
    
    echo_counter_Stop();
    echo_counter_SaveConfig();
}


/*******************************************************************************
* Function Name: echo_counter_Wakeup
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
*  echo_counter_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void echo_counter_Wakeup(void) 
{
    echo_counter_RestoreConfig();
    #if(!echo_counter_ControlRegRemoved)
        if(echo_counter_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            echo_counter_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!echo_counter_ControlRegRemoved) */
    
}


/* [] END OF FILE */
