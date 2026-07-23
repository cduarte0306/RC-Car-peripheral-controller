/*******************************************************************************
* File Name: speed_msb.c  
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

#include "speed_msb.h"

#if !defined(speed_msb_sts_sts_reg__REMOVED) /* Check for removal by optimization */


/*******************************************************************************
* Function Name: speed_msb_Read
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
uint8 speed_msb_Read(void) 
{ 
    return speed_msb_Status;
}


/*******************************************************************************
* Function Name: speed_msb_InterruptEnable
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
void speed_msb_InterruptEnable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    speed_msb_Status_Aux_Ctrl |= speed_msb_STATUS_INTR_ENBL;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: speed_msb_InterruptDisable
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
void speed_msb_InterruptDisable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    speed_msb_Status_Aux_Ctrl &= (uint8)(~speed_msb_STATUS_INTR_ENBL);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: speed_msb_WriteMask
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
void speed_msb_WriteMask(uint8 mask) 
{
    #if(speed_msb_INPUTS < 8u)
    	mask &= ((uint8)(1u << speed_msb_INPUTS) - 1u);
	#endif /* End speed_msb_INPUTS < 8u */
    speed_msb_Status_Mask = mask;
}


/*******************************************************************************
* Function Name: speed_msb_ReadMask
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
uint8 speed_msb_ReadMask(void) 
{
    return speed_msb_Status_Mask;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
