/*******************************************************************************
* File Name: rx_nrq.h
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
#if !defined(CY_ISR_rx_nrq_H)
#define CY_ISR_rx_nrq_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void rx_nrq_Start(void);
void rx_nrq_StartEx(cyisraddress address);
void rx_nrq_Stop(void);

CY_ISR_PROTO(rx_nrq_Interrupt);

void rx_nrq_SetVector(cyisraddress address);
cyisraddress rx_nrq_GetVector(void);

void rx_nrq_SetPriority(uint8 priority);
uint8 rx_nrq_GetPriority(void);

void rx_nrq_Enable(void);
uint8 rx_nrq_GetState(void);
void rx_nrq_Disable(void);

void rx_nrq_SetPending(void);
void rx_nrq_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the rx_nrq ISR. */
#define rx_nrq_INTC_VECTOR            ((reg32 *) rx_nrq__INTC_VECT)

/* Address of the rx_nrq ISR priority. */
#define rx_nrq_INTC_PRIOR             ((reg8 *) rx_nrq__INTC_PRIOR_REG)

/* Priority of the rx_nrq interrupt. */
#define rx_nrq_INTC_PRIOR_NUMBER      rx_nrq__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable rx_nrq interrupt. */
#define rx_nrq_INTC_SET_EN            ((reg32 *) rx_nrq__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the rx_nrq interrupt. */
#define rx_nrq_INTC_CLR_EN            ((reg32 *) rx_nrq__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the rx_nrq interrupt state to pending. */
#define rx_nrq_INTC_SET_PD            ((reg32 *) rx_nrq__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the rx_nrq interrupt. */
#define rx_nrq_INTC_CLR_PD            ((reg32 *) rx_nrq__INTC_CLR_PD_REG)


#endif /* CY_ISR_rx_nrq_H */


/* [] END OF FILE */
