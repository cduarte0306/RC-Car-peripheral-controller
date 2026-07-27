/*******************************************************************************
* File Name: in2.h  
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

#if !defined(CY_PINS_in2_H) /* Pins in2_H */
#define CY_PINS_in2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "in2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 in2__PORT == 15 && ((in2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    in2_Write(uint8 value);
void    in2_SetDriveMode(uint8 mode);
uint8   in2_ReadDataReg(void);
uint8   in2_Read(void);
void    in2_SetInterruptMode(uint16 position, uint16 mode);
uint8   in2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the in2_SetDriveMode() function.
     *  @{
     */
        #define in2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define in2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define in2_DM_RES_UP          PIN_DM_RES_UP
        #define in2_DM_RES_DWN         PIN_DM_RES_DWN
        #define in2_DM_OD_LO           PIN_DM_OD_LO
        #define in2_DM_OD_HI           PIN_DM_OD_HI
        #define in2_DM_STRONG          PIN_DM_STRONG
        #define in2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define in2_MASK               in2__MASK
#define in2_SHIFT              in2__SHIFT
#define in2_WIDTH              1u

/* Interrupt constants */
#if defined(in2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in in2_SetInterruptMode() function.
     *  @{
     */
        #define in2_INTR_NONE      (uint16)(0x0000u)
        #define in2_INTR_RISING    (uint16)(0x0001u)
        #define in2_INTR_FALLING   (uint16)(0x0002u)
        #define in2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define in2_INTR_MASK      (0x01u) 
#endif /* (in2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define in2_PS                     (* (reg8 *) in2__PS)
/* Data Register */
#define in2_DR                     (* (reg8 *) in2__DR)
/* Port Number */
#define in2_PRT_NUM                (* (reg8 *) in2__PRT) 
/* Connect to Analog Globals */                                                  
#define in2_AG                     (* (reg8 *) in2__AG)                       
/* Analog MUX bux enable */
#define in2_AMUX                   (* (reg8 *) in2__AMUX) 
/* Bidirectional Enable */                                                        
#define in2_BIE                    (* (reg8 *) in2__BIE)
/* Bit-mask for Aliased Register Access */
#define in2_BIT_MASK               (* (reg8 *) in2__BIT_MASK)
/* Bypass Enable */
#define in2_BYP                    (* (reg8 *) in2__BYP)
/* Port wide control signals */                                                   
#define in2_CTL                    (* (reg8 *) in2__CTL)
/* Drive Modes */
#define in2_DM0                    (* (reg8 *) in2__DM0) 
#define in2_DM1                    (* (reg8 *) in2__DM1)
#define in2_DM2                    (* (reg8 *) in2__DM2) 
/* Input Buffer Disable Override */
#define in2_INP_DIS                (* (reg8 *) in2__INP_DIS)
/* LCD Common or Segment Drive */
#define in2_LCD_COM_SEG            (* (reg8 *) in2__LCD_COM_SEG)
/* Enable Segment LCD */
#define in2_LCD_EN                 (* (reg8 *) in2__LCD_EN)
/* Slew Rate Control */
#define in2_SLW                    (* (reg8 *) in2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define in2_PRTDSI__CAPS_SEL       (* (reg8 *) in2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define in2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) in2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define in2_PRTDSI__OE_SEL0        (* (reg8 *) in2__PRTDSI__OE_SEL0) 
#define in2_PRTDSI__OE_SEL1        (* (reg8 *) in2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define in2_PRTDSI__OUT_SEL0       (* (reg8 *) in2__PRTDSI__OUT_SEL0) 
#define in2_PRTDSI__OUT_SEL1       (* (reg8 *) in2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define in2_PRTDSI__SYNC_OUT       (* (reg8 *) in2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(in2__SIO_CFG)
    #define in2_SIO_HYST_EN        (* (reg8 *) in2__SIO_HYST_EN)
    #define in2_SIO_REG_HIFREQ     (* (reg8 *) in2__SIO_REG_HIFREQ)
    #define in2_SIO_CFG            (* (reg8 *) in2__SIO_CFG)
    #define in2_SIO_DIFF           (* (reg8 *) in2__SIO_DIFF)
#endif /* (in2__SIO_CFG) */

/* Interrupt Registers */
#if defined(in2__INTSTAT)
    #define in2_INTSTAT            (* (reg8 *) in2__INTSTAT)
    #define in2_SNAP               (* (reg8 *) in2__SNAP)
    
	#define in2_0_INTTYPE_REG 		(* (reg8 *) in2__0__INTTYPE)
#endif /* (in2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_in2_H */


/* [] END OF FILE */
