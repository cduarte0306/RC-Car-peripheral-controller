/*******************************************************************************
* File Name: dir_sel.h  
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

#if !defined(CY_PINS_dir_sel_H) /* Pins dir_sel_H */
#define CY_PINS_dir_sel_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "dir_sel_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 dir_sel__PORT == 15 && ((dir_sel__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    dir_sel_Write(uint8 value);
void    dir_sel_SetDriveMode(uint8 mode);
uint8   dir_sel_ReadDataReg(void);
uint8   dir_sel_Read(void);
void    dir_sel_SetInterruptMode(uint16 position, uint16 mode);
uint8   dir_sel_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the dir_sel_SetDriveMode() function.
     *  @{
     */
        #define dir_sel_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define dir_sel_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define dir_sel_DM_RES_UP          PIN_DM_RES_UP
        #define dir_sel_DM_RES_DWN         PIN_DM_RES_DWN
        #define dir_sel_DM_OD_LO           PIN_DM_OD_LO
        #define dir_sel_DM_OD_HI           PIN_DM_OD_HI
        #define dir_sel_DM_STRONG          PIN_DM_STRONG
        #define dir_sel_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define dir_sel_MASK               dir_sel__MASK
#define dir_sel_SHIFT              dir_sel__SHIFT
#define dir_sel_WIDTH              1u

/* Interrupt constants */
#if defined(dir_sel__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in dir_sel_SetInterruptMode() function.
     *  @{
     */
        #define dir_sel_INTR_NONE      (uint16)(0x0000u)
        #define dir_sel_INTR_RISING    (uint16)(0x0001u)
        #define dir_sel_INTR_FALLING   (uint16)(0x0002u)
        #define dir_sel_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define dir_sel_INTR_MASK      (0x01u) 
#endif /* (dir_sel__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define dir_sel_PS                     (* (reg8 *) dir_sel__PS)
/* Data Register */
#define dir_sel_DR                     (* (reg8 *) dir_sel__DR)
/* Port Number */
#define dir_sel_PRT_NUM                (* (reg8 *) dir_sel__PRT) 
/* Connect to Analog Globals */                                                  
#define dir_sel_AG                     (* (reg8 *) dir_sel__AG)                       
/* Analog MUX bux enable */
#define dir_sel_AMUX                   (* (reg8 *) dir_sel__AMUX) 
/* Bidirectional Enable */                                                        
#define dir_sel_BIE                    (* (reg8 *) dir_sel__BIE)
/* Bit-mask for Aliased Register Access */
#define dir_sel_BIT_MASK               (* (reg8 *) dir_sel__BIT_MASK)
/* Bypass Enable */
#define dir_sel_BYP                    (* (reg8 *) dir_sel__BYP)
/* Port wide control signals */                                                   
#define dir_sel_CTL                    (* (reg8 *) dir_sel__CTL)
/* Drive Modes */
#define dir_sel_DM0                    (* (reg8 *) dir_sel__DM0) 
#define dir_sel_DM1                    (* (reg8 *) dir_sel__DM1)
#define dir_sel_DM2                    (* (reg8 *) dir_sel__DM2) 
/* Input Buffer Disable Override */
#define dir_sel_INP_DIS                (* (reg8 *) dir_sel__INP_DIS)
/* LCD Common or Segment Drive */
#define dir_sel_LCD_COM_SEG            (* (reg8 *) dir_sel__LCD_COM_SEG)
/* Enable Segment LCD */
#define dir_sel_LCD_EN                 (* (reg8 *) dir_sel__LCD_EN)
/* Slew Rate Control */
#define dir_sel_SLW                    (* (reg8 *) dir_sel__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define dir_sel_PRTDSI__CAPS_SEL       (* (reg8 *) dir_sel__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define dir_sel_PRTDSI__DBL_SYNC_IN    (* (reg8 *) dir_sel__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define dir_sel_PRTDSI__OE_SEL0        (* (reg8 *) dir_sel__PRTDSI__OE_SEL0) 
#define dir_sel_PRTDSI__OE_SEL1        (* (reg8 *) dir_sel__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define dir_sel_PRTDSI__OUT_SEL0       (* (reg8 *) dir_sel__PRTDSI__OUT_SEL0) 
#define dir_sel_PRTDSI__OUT_SEL1       (* (reg8 *) dir_sel__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define dir_sel_PRTDSI__SYNC_OUT       (* (reg8 *) dir_sel__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(dir_sel__SIO_CFG)
    #define dir_sel_SIO_HYST_EN        (* (reg8 *) dir_sel__SIO_HYST_EN)
    #define dir_sel_SIO_REG_HIFREQ     (* (reg8 *) dir_sel__SIO_REG_HIFREQ)
    #define dir_sel_SIO_CFG            (* (reg8 *) dir_sel__SIO_CFG)
    #define dir_sel_SIO_DIFF           (* (reg8 *) dir_sel__SIO_DIFF)
#endif /* (dir_sel__SIO_CFG) */

/* Interrupt Registers */
#if defined(dir_sel__INTSTAT)
    #define dir_sel_INTSTAT            (* (reg8 *) dir_sel__INTSTAT)
    #define dir_sel_SNAP               (* (reg8 *) dir_sel__SNAP)
    
	#define dir_sel_0_INTTYPE_REG 		(* (reg8 *) dir_sel__0__INTTYPE)
#endif /* (dir_sel__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_dir_sel_H */


/* [] END OF FILE */
