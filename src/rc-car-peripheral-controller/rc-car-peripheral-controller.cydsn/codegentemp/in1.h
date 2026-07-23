/*******************************************************************************
* File Name: in1.h  
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

#if !defined(CY_PINS_in1_H) /* Pins in1_H */
#define CY_PINS_in1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "in1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 in1__PORT == 15 && ((in1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    in1_Write(uint8 value);
void    in1_SetDriveMode(uint8 mode);
uint8   in1_ReadDataReg(void);
uint8   in1_Read(void);
void    in1_SetInterruptMode(uint16 position, uint16 mode);
uint8   in1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the in1_SetDriveMode() function.
     *  @{
     */
        #define in1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define in1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define in1_DM_RES_UP          PIN_DM_RES_UP
        #define in1_DM_RES_DWN         PIN_DM_RES_DWN
        #define in1_DM_OD_LO           PIN_DM_OD_LO
        #define in1_DM_OD_HI           PIN_DM_OD_HI
        #define in1_DM_STRONG          PIN_DM_STRONG
        #define in1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define in1_MASK               in1__MASK
#define in1_SHIFT              in1__SHIFT
#define in1_WIDTH              1u

/* Interrupt constants */
#if defined(in1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in in1_SetInterruptMode() function.
     *  @{
     */
        #define in1_INTR_NONE      (uint16)(0x0000u)
        #define in1_INTR_RISING    (uint16)(0x0001u)
        #define in1_INTR_FALLING   (uint16)(0x0002u)
        #define in1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define in1_INTR_MASK      (0x01u) 
#endif /* (in1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define in1_PS                     (* (reg8 *) in1__PS)
/* Data Register */
#define in1_DR                     (* (reg8 *) in1__DR)
/* Port Number */
#define in1_PRT_NUM                (* (reg8 *) in1__PRT) 
/* Connect to Analog Globals */                                                  
#define in1_AG                     (* (reg8 *) in1__AG)                       
/* Analog MUX bux enable */
#define in1_AMUX                   (* (reg8 *) in1__AMUX) 
/* Bidirectional Enable */                                                        
#define in1_BIE                    (* (reg8 *) in1__BIE)
/* Bit-mask for Aliased Register Access */
#define in1_BIT_MASK               (* (reg8 *) in1__BIT_MASK)
/* Bypass Enable */
#define in1_BYP                    (* (reg8 *) in1__BYP)
/* Port wide control signals */                                                   
#define in1_CTL                    (* (reg8 *) in1__CTL)
/* Drive Modes */
#define in1_DM0                    (* (reg8 *) in1__DM0) 
#define in1_DM1                    (* (reg8 *) in1__DM1)
#define in1_DM2                    (* (reg8 *) in1__DM2) 
/* Input Buffer Disable Override */
#define in1_INP_DIS                (* (reg8 *) in1__INP_DIS)
/* LCD Common or Segment Drive */
#define in1_LCD_COM_SEG            (* (reg8 *) in1__LCD_COM_SEG)
/* Enable Segment LCD */
#define in1_LCD_EN                 (* (reg8 *) in1__LCD_EN)
/* Slew Rate Control */
#define in1_SLW                    (* (reg8 *) in1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define in1_PRTDSI__CAPS_SEL       (* (reg8 *) in1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define in1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) in1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define in1_PRTDSI__OE_SEL0        (* (reg8 *) in1__PRTDSI__OE_SEL0) 
#define in1_PRTDSI__OE_SEL1        (* (reg8 *) in1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define in1_PRTDSI__OUT_SEL0       (* (reg8 *) in1__PRTDSI__OUT_SEL0) 
#define in1_PRTDSI__OUT_SEL1       (* (reg8 *) in1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define in1_PRTDSI__SYNC_OUT       (* (reg8 *) in1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(in1__SIO_CFG)
    #define in1_SIO_HYST_EN        (* (reg8 *) in1__SIO_HYST_EN)
    #define in1_SIO_REG_HIFREQ     (* (reg8 *) in1__SIO_REG_HIFREQ)
    #define in1_SIO_CFG            (* (reg8 *) in1__SIO_CFG)
    #define in1_SIO_DIFF           (* (reg8 *) in1__SIO_DIFF)
#endif /* (in1__SIO_CFG) */

/* Interrupt Registers */
#if defined(in1__INTSTAT)
    #define in1_INTSTAT            (* (reg8 *) in1__INTSTAT)
    #define in1_SNAP               (* (reg8 *) in1__SNAP)
    
	#define in1_0_INTTYPE_REG 		(* (reg8 *) in1__0__INTTYPE)
#endif /* (in1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_in1_H */


/* [] END OF FILE */
