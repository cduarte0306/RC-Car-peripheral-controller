/*******************************************************************************
* File Name: IO_REF_PM.c  
* Version 1.90
*
* Description:
*  This file provides the power management source code to API for the
*  VDAC8.  
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "IO_REF.h"

static IO_REF_backupStruct IO_REF_backup;


/*******************************************************************************
* Function Name: IO_REF_SaveConfig
********************************************************************************
* Summary:
*  Save the current user configuration
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void IO_REF_SaveConfig(void) 
{
    if (!((IO_REF_CR1 & IO_REF_SRC_MASK) == IO_REF_SRC_UDB))
    {
        IO_REF_backup.data_value = IO_REF_Data;
    }
}


/*******************************************************************************
* Function Name: IO_REF_RestoreConfig
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
*******************************************************************************/
void IO_REF_RestoreConfig(void) 
{
    if (!((IO_REF_CR1 & IO_REF_SRC_MASK) == IO_REF_SRC_UDB))
    {
        if((IO_REF_Strobe & IO_REF_STRB_MASK) == IO_REF_STRB_EN)
        {
            IO_REF_Strobe &= (uint8)(~IO_REF_STRB_MASK);
            IO_REF_Data = IO_REF_backup.data_value;
            IO_REF_Strobe |= IO_REF_STRB_EN;
        }
        else
        {
            IO_REF_Data = IO_REF_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: IO_REF_Sleep
********************************************************************************
* Summary:
*  Stop and Save the user configuration
*
* Parameters:  
*  void:  
*
* Return: 
*  void
*
* Global variables:
*  IO_REF_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void IO_REF_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(IO_REF_ACT_PWR_EN == (IO_REF_PWRMGR & IO_REF_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        IO_REF_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        IO_REF_backup.enableState = 0u;
    }
    
    IO_REF_Stop();
    IO_REF_SaveConfig();
}


/*******************************************************************************
* Function Name: IO_REF_Wakeup
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
*  IO_REF_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void IO_REF_Wakeup(void) 
{
    IO_REF_RestoreConfig();
    
    if(IO_REF_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        IO_REF_Enable();

        /* Restore the data register */
        IO_REF_SetValue(IO_REF_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
