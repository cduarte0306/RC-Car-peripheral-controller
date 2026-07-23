/*******************************************************************************
* File Name: isr_right_echo.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_isr_right_echo_H)
#define CY_ISR_isr_right_echo_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void isr_right_echo_Start(void);
void isr_right_echo_StartEx(cyisraddress address);
void isr_right_echo_Stop(void);

CY_ISR_PROTO(isr_right_echo_Interrupt);

void isr_right_echo_SetVector(cyisraddress address);
cyisraddress isr_right_echo_GetVector(void);

void isr_right_echo_SetPriority(uint8 priority);
uint8 isr_right_echo_GetPriority(void);

void isr_right_echo_Enable(void);
uint8 isr_right_echo_GetState(void);
void isr_right_echo_Disable(void);

void isr_right_echo_SetPending(void);
void isr_right_echo_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the isr_right_echo ISR. */
#define isr_right_echo_INTC_VECTOR            ((reg32 *) isr_right_echo__INTC_VECT)

/* Address of the isr_right_echo ISR priority. */
#define isr_right_echo_INTC_PRIOR             ((reg8 *) isr_right_echo__INTC_PRIOR_REG)

/* Priority of the isr_right_echo interrupt. */
#define isr_right_echo_INTC_PRIOR_NUMBER      isr_right_echo__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable isr_right_echo interrupt. */
#define isr_right_echo_INTC_SET_EN            ((reg32 *) isr_right_echo__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the isr_right_echo interrupt. */
#define isr_right_echo_INTC_CLR_EN            ((reg32 *) isr_right_echo__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the isr_right_echo interrupt state to pending. */
#define isr_right_echo_INTC_SET_PD            ((reg32 *) isr_right_echo__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the isr_right_echo interrupt. */
#define isr_right_echo_INTC_CLR_PD            ((reg32 *) isr_right_echo__INTC_CLR_PD_REG)


#endif /* CY_ISR_isr_right_echo_H */


/* [] END OF FILE */
