/*******************************************************************************
* File Name: front_echo.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_front_echo_H) /* Pins front_echo_H */
#define CY_PINS_front_echo_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "front_echo_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 front_echo__PORT == 15 && ((front_echo__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    front_echo_Write(uint8 value);
void    front_echo_SetDriveMode(uint8 mode);
uint8   front_echo_ReadDataReg(void);
uint8   front_echo_Read(void);
void    front_echo_SetInterruptMode(uint16 position, uint16 mode);
uint8   front_echo_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the front_echo_SetDriveMode() function.
     *  @{
     */
        #define front_echo_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define front_echo_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define front_echo_DM_RES_UP          PIN_DM_RES_UP
        #define front_echo_DM_RES_DWN         PIN_DM_RES_DWN
        #define front_echo_DM_OD_LO           PIN_DM_OD_LO
        #define front_echo_DM_OD_HI           PIN_DM_OD_HI
        #define front_echo_DM_STRONG          PIN_DM_STRONG
        #define front_echo_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define front_echo_MASK               front_echo__MASK
#define front_echo_SHIFT              front_echo__SHIFT
#define front_echo_WIDTH              1u

/* Interrupt constants */
#if defined(front_echo__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in front_echo_SetInterruptMode() function.
     *  @{
     */
        #define front_echo_INTR_NONE      (uint16)(0x0000u)
        #define front_echo_INTR_RISING    (uint16)(0x0001u)
        #define front_echo_INTR_FALLING   (uint16)(0x0002u)
        #define front_echo_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define front_echo_INTR_MASK      (0x01u) 
#endif /* (front_echo__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define front_echo_PS                     (* (reg8 *) front_echo__PS)
/* Data Register */
#define front_echo_DR                     (* (reg8 *) front_echo__DR)
/* Port Number */
#define front_echo_PRT_NUM                (* (reg8 *) front_echo__PRT) 
/* Connect to Analog Globals */                                                  
#define front_echo_AG                     (* (reg8 *) front_echo__AG)                       
/* Analog MUX bux enable */
#define front_echo_AMUX                   (* (reg8 *) front_echo__AMUX) 
/* Bidirectional Enable */                                                        
#define front_echo_BIE                    (* (reg8 *) front_echo__BIE)
/* Bit-mask for Aliased Register Access */
#define front_echo_BIT_MASK               (* (reg8 *) front_echo__BIT_MASK)
/* Bypass Enable */
#define front_echo_BYP                    (* (reg8 *) front_echo__BYP)
/* Port wide control signals */                                                   
#define front_echo_CTL                    (* (reg8 *) front_echo__CTL)
/* Drive Modes */
#define front_echo_DM0                    (* (reg8 *) front_echo__DM0) 
#define front_echo_DM1                    (* (reg8 *) front_echo__DM1)
#define front_echo_DM2                    (* (reg8 *) front_echo__DM2) 
/* Input Buffer Disable Override */
#define front_echo_INP_DIS                (* (reg8 *) front_echo__INP_DIS)
/* LCD Common or Segment Drive */
#define front_echo_LCD_COM_SEG            (* (reg8 *) front_echo__LCD_COM_SEG)
/* Enable Segment LCD */
#define front_echo_LCD_EN                 (* (reg8 *) front_echo__LCD_EN)
/* Slew Rate Control */
#define front_echo_SLW                    (* (reg8 *) front_echo__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define front_echo_PRTDSI__CAPS_SEL       (* (reg8 *) front_echo__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define front_echo_PRTDSI__DBL_SYNC_IN    (* (reg8 *) front_echo__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define front_echo_PRTDSI__OE_SEL0        (* (reg8 *) front_echo__PRTDSI__OE_SEL0) 
#define front_echo_PRTDSI__OE_SEL1        (* (reg8 *) front_echo__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define front_echo_PRTDSI__OUT_SEL0       (* (reg8 *) front_echo__PRTDSI__OUT_SEL0) 
#define front_echo_PRTDSI__OUT_SEL1       (* (reg8 *) front_echo__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define front_echo_PRTDSI__SYNC_OUT       (* (reg8 *) front_echo__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(front_echo__SIO_CFG)
    #define front_echo_SIO_HYST_EN        (* (reg8 *) front_echo__SIO_HYST_EN)
    #define front_echo_SIO_REG_HIFREQ     (* (reg8 *) front_echo__SIO_REG_HIFREQ)
    #define front_echo_SIO_CFG            (* (reg8 *) front_echo__SIO_CFG)
    #define front_echo_SIO_DIFF           (* (reg8 *) front_echo__SIO_DIFF)
#endif /* (front_echo__SIO_CFG) */

/* Interrupt Registers */
#if defined(front_echo__INTSTAT)
    #define front_echo_INTSTAT            (* (reg8 *) front_echo__INTSTAT)
    #define front_echo_SNAP               (* (reg8 *) front_echo__SNAP)
    
	#define front_echo_0_INTTYPE_REG 		(* (reg8 *) front_echo__0__INTTYPE)
#endif /* (front_echo__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_front_echo_H */


/* [] END OF FILE */
