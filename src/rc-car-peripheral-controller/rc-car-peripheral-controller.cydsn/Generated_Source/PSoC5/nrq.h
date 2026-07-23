/*******************************************************************************
* File Name: nrq.h
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
#if !defined(CY_ISR_nrq_H)
#define CY_ISR_nrq_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void nrq_Start(void);
void nrq_StartEx(cyisraddress address);
void nrq_Stop(void);

CY_ISR_PROTO(nrq_Interrupt);

void nrq_SetVector(cyisraddress address);
cyisraddress nrq_GetVector(void);

void nrq_SetPriority(uint8 priority);
uint8 nrq_GetPriority(void);

void nrq_Enable(void);
uint8 nrq_GetState(void);
void nrq_Disable(void);

void nrq_SetPending(void);
void nrq_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the nrq ISR. */
#define nrq_INTC_VECTOR            ((reg32 *) nrq__INTC_VECT)

/* Address of the nrq ISR priority. */
#define nrq_INTC_PRIOR             ((reg8 *) nrq__INTC_PRIOR_REG)

/* Priority of the nrq interrupt. */
#define nrq_INTC_PRIOR_NUMBER      nrq__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable nrq interrupt. */
#define nrq_INTC_SET_EN            ((reg32 *) nrq__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the nrq interrupt. */
#define nrq_INTC_CLR_EN            ((reg32 *) nrq__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the nrq interrupt state to pending. */
#define nrq_INTC_SET_PD            ((reg32 *) nrq__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the nrq interrupt. */
#define nrq_INTC_CLR_PD            ((reg32 *) nrq__INTC_CLR_PD_REG)


#endif /* CY_ISR_nrq_H */


/* [] END OF FILE */
