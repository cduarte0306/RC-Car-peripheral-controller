/*******************************************************************************
* File Name: imu_interrupt_pin.h
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
#if !defined(CY_ISR_imu_interrupt_pin_H)
#define CY_ISR_imu_interrupt_pin_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void imu_interrupt_pin_Start(void);
void imu_interrupt_pin_StartEx(cyisraddress address);
void imu_interrupt_pin_Stop(void);

CY_ISR_PROTO(imu_interrupt_pin_Interrupt);

void imu_interrupt_pin_SetVector(cyisraddress address);
cyisraddress imu_interrupt_pin_GetVector(void);

void imu_interrupt_pin_SetPriority(uint8 priority);
uint8 imu_interrupt_pin_GetPriority(void);

void imu_interrupt_pin_Enable(void);
uint8 imu_interrupt_pin_GetState(void);
void imu_interrupt_pin_Disable(void);

void imu_interrupt_pin_SetPending(void);
void imu_interrupt_pin_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the imu_interrupt_pin ISR. */
#define imu_interrupt_pin_INTC_VECTOR            ((reg32 *) imu_interrupt_pin__INTC_VECT)

/* Address of the imu_interrupt_pin ISR priority. */
#define imu_interrupt_pin_INTC_PRIOR             ((reg8 *) imu_interrupt_pin__INTC_PRIOR_REG)

/* Priority of the imu_interrupt_pin interrupt. */
#define imu_interrupt_pin_INTC_PRIOR_NUMBER      imu_interrupt_pin__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable imu_interrupt_pin interrupt. */
#define imu_interrupt_pin_INTC_SET_EN            ((reg32 *) imu_interrupt_pin__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the imu_interrupt_pin interrupt. */
#define imu_interrupt_pin_INTC_CLR_EN            ((reg32 *) imu_interrupt_pin__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the imu_interrupt_pin interrupt state to pending. */
#define imu_interrupt_pin_INTC_SET_PD            ((reg32 *) imu_interrupt_pin__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the imu_interrupt_pin interrupt. */
#define imu_interrupt_pin_INTC_CLR_PD            ((reg32 *) imu_interrupt_pin__INTC_CLR_PD_REG)


#endif /* CY_ISR_imu_interrupt_pin_H */


/* [] END OF FILE */
