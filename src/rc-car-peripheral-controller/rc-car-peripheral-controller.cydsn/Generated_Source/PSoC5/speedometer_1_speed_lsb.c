/*******************************************************************************
* File Name: speedometer_1_speed_lsb.c  
* Version 1.90
*
* Description:
*  This file contains API to enable firmware to read the value of a Status 
*  Register.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "speedometer_1_speed_lsb.h"

#if !defined(speedometer_1_speed_lsb_sts_sts_reg__REMOVED) /* Check for removal by optimization */


/*******************************************************************************
* Function Name: speedometer_1_speed_lsb_Read
********************************************************************************
*
* Summary:
*  Reads the current value assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The current value in the Status Register.
*
*******************************************************************************/
uint8 speedometer_1_speed_lsb_Read(void) 
{ 
    return speedometer_1_speed_lsb_Status;
}


/*******************************************************************************
* Function Name: speedometer_1_speed_lsb_InterruptEnable
********************************************************************************
*
* Summary:
*  Enables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void speedometer_1_speed_lsb_InterruptEnable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    speedometer_1_speed_lsb_Status_Aux_Ctrl |= speedometer_1_speed_lsb_STATUS_INTR_ENBL;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: speedometer_1_speed_lsb_InterruptDisable
********************************************************************************
*
* Summary:
*  Disables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void speedometer_1_speed_lsb_InterruptDisable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    speedometer_1_speed_lsb_Status_Aux_Ctrl &= (uint8)(~speedometer_1_speed_lsb_STATUS_INTR_ENBL);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: speedometer_1_speed_lsb_WriteMask
********************************************************************************
*
* Summary:
*  Writes the current mask value assigned to the Status Register.
*
* Parameters:
*  mask:  Value to write into the mask register.
*
* Return:
*  None.
*
*******************************************************************************/
void speedometer_1_speed_lsb_WriteMask(uint8 mask) 
{
    #if(speedometer_1_speed_lsb_INPUTS < 8u)
    	mask &= ((uint8)(1u << speedometer_1_speed_lsb_INPUTS) - 1u);
	#endif /* End speedometer_1_speed_lsb_INPUTS < 8u */
    speedometer_1_speed_lsb_Status_Mask = mask;
}


/*******************************************************************************
* Function Name: speedometer_1_speed_lsb_ReadMask
********************************************************************************
*
* Summary:
*  Reads the current interrupt mask assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The value of the interrupt mask of the Status Register.
*
*******************************************************************************/
uint8 speedometer_1_speed_lsb_ReadMask(void) 
{
    return speedometer_1_speed_lsb_Status_Mask;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
