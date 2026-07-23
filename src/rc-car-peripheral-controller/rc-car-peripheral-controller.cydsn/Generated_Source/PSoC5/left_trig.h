/*******************************************************************************
* File Name: left_trig.h  
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

#if !defined(CY_PINS_left_trig_H) /* Pins left_trig_H */
#define CY_PINS_left_trig_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "left_trig_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 left_trig__PORT == 15 && ((left_trig__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    left_trig_Write(uint8 value);
void    left_trig_SetDriveMode(uint8 mode);
uint8   left_trig_ReadDataReg(void);
uint8   left_trig_Read(void);
void    left_trig_SetInterruptMode(uint16 position, uint16 mode);
uint8   left_trig_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the left_trig_SetDriveMode() function.
     *  @{
     */
        #define left_trig_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define left_trig_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define left_trig_DM_RES_UP          PIN_DM_RES_UP
        #define left_trig_DM_RES_DWN         PIN_DM_RES_DWN
        #define left_trig_DM_OD_LO           PIN_DM_OD_LO
        #define left_trig_DM_OD_HI           PIN_DM_OD_HI
        #define left_trig_DM_STRONG          PIN_DM_STRONG
        #define left_trig_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define left_trig_MASK               left_trig__MASK
#define left_trig_SHIFT              left_trig__SHIFT
#define left_trig_WIDTH              1u

/* Interrupt constants */
#if defined(left_trig__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in left_trig_SetInterruptMode() function.
     *  @{
     */
        #define left_trig_INTR_NONE      (uint16)(0x0000u)
        #define left_trig_INTR_RISING    (uint16)(0x0001u)
        #define left_trig_INTR_FALLING   (uint16)(0x0002u)
        #define left_trig_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define left_trig_INTR_MASK      (0x01u) 
#endif /* (left_trig__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define left_trig_PS                     (* (reg8 *) left_trig__PS)
/* Data Register */
#define left_trig_DR                     (* (reg8 *) left_trig__DR)
/* Port Number */
#define left_trig_PRT_NUM                (* (reg8 *) left_trig__PRT) 
/* Connect to Analog Globals */                                                  
#define left_trig_AG                     (* (reg8 *) left_trig__AG)                       
/* Analog MUX bux enable */
#define left_trig_AMUX                   (* (reg8 *) left_trig__AMUX) 
/* Bidirectional Enable */                                                        
#define left_trig_BIE                    (* (reg8 *) left_trig__BIE)
/* Bit-mask for Aliased Register Access */
#define left_trig_BIT_MASK               (* (reg8 *) left_trig__BIT_MASK)
/* Bypass Enable */
#define left_trig_BYP                    (* (reg8 *) left_trig__BYP)
/* Port wide control signals */                                                   
#define left_trig_CTL                    (* (reg8 *) left_trig__CTL)
/* Drive Modes */
#define left_trig_DM0                    (* (reg8 *) left_trig__DM0) 
#define left_trig_DM1                    (* (reg8 *) left_trig__DM1)
#define left_trig_DM2                    (* (reg8 *) left_trig__DM2) 
/* Input Buffer Disable Override */
#define left_trig_INP_DIS                (* (reg8 *) left_trig__INP_DIS)
/* LCD Common or Segment Drive */
#define left_trig_LCD_COM_SEG            (* (reg8 *) left_trig__LCD_COM_SEG)
/* Enable Segment LCD */
#define left_trig_LCD_EN                 (* (reg8 *) left_trig__LCD_EN)
/* Slew Rate Control */
#define left_trig_SLW                    (* (reg8 *) left_trig__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define left_trig_PRTDSI__CAPS_SEL       (* (reg8 *) left_trig__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define left_trig_PRTDSI__DBL_SYNC_IN    (* (reg8 *) left_trig__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define left_trig_PRTDSI__OE_SEL0        (* (reg8 *) left_trig__PRTDSI__OE_SEL0) 
#define left_trig_PRTDSI__OE_SEL1        (* (reg8 *) left_trig__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define left_trig_PRTDSI__OUT_SEL0       (* (reg8 *) left_trig__PRTDSI__OUT_SEL0) 
#define left_trig_PRTDSI__OUT_SEL1       (* (reg8 *) left_trig__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define left_trig_PRTDSI__SYNC_OUT       (* (reg8 *) left_trig__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(left_trig__SIO_CFG)
    #define left_trig_SIO_HYST_EN        (* (reg8 *) left_trig__SIO_HYST_EN)
    #define left_trig_SIO_REG_HIFREQ     (* (reg8 *) left_trig__SIO_REG_HIFREQ)
    #define left_trig_SIO_CFG            (* (reg8 *) left_trig__SIO_CFG)
    #define left_trig_SIO_DIFF           (* (reg8 *) left_trig__SIO_DIFF)
#endif /* (left_trig__SIO_CFG) */

/* Interrupt Registers */
#if defined(left_trig__INTSTAT)
    #define left_trig_INTSTAT            (* (reg8 *) left_trig__INTSTAT)
    #define left_trig_SNAP               (* (reg8 *) left_trig__SNAP)
    
	#define left_trig_0_INTTYPE_REG 		(* (reg8 *) left_trig__0__INTTYPE)
#endif /* (left_trig__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_left_trig_H */


/* [] END OF FILE */
