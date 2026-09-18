/*******************************************************************************
* File Name: TIMER_CLK.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_TIMER_CLK_H)
#define CY_CLOCK_TIMER_CLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void TIMER_CLK_Start(void) ;
void TIMER_CLK_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void TIMER_CLK_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void TIMER_CLK_StandbyPower(uint8 state) ;
void TIMER_CLK_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 TIMER_CLK_GetDividerRegister(void) ;
void TIMER_CLK_SetModeRegister(uint8 modeBitMask) ;
void TIMER_CLK_ClearModeRegister(uint8 modeBitMask) ;
uint8 TIMER_CLK_GetModeRegister(void) ;
void TIMER_CLK_SetSourceRegister(uint8 clkSource) ;
uint8 TIMER_CLK_GetSourceRegister(void) ;
#if defined(TIMER_CLK__CFG3)
void TIMER_CLK_SetPhaseRegister(uint8 clkPhase) ;
uint8 TIMER_CLK_GetPhaseRegister(void) ;
#endif /* defined(TIMER_CLK__CFG3) */

#define TIMER_CLK_Enable()                       TIMER_CLK_Start()
#define TIMER_CLK_Disable()                      TIMER_CLK_Stop()
#define TIMER_CLK_SetDivider(clkDivider)         TIMER_CLK_SetDividerRegister(clkDivider, 1u)
#define TIMER_CLK_SetDividerValue(clkDivider)    TIMER_CLK_SetDividerRegister((clkDivider) - 1u, 1u)
#define TIMER_CLK_SetMode(clkMode)               TIMER_CLK_SetModeRegister(clkMode)
#define TIMER_CLK_SetSource(clkSource)           TIMER_CLK_SetSourceRegister(clkSource)
#if defined(TIMER_CLK__CFG3)
#define TIMER_CLK_SetPhase(clkPhase)             TIMER_CLK_SetPhaseRegister(clkPhase)
#define TIMER_CLK_SetPhaseValue(clkPhase)        TIMER_CLK_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(TIMER_CLK__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define TIMER_CLK_CLKEN              (* (reg8 *) TIMER_CLK__PM_ACT_CFG)
#define TIMER_CLK_CLKEN_PTR          ((reg8 *) TIMER_CLK__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define TIMER_CLK_CLKSTBY            (* (reg8 *) TIMER_CLK__PM_STBY_CFG)
#define TIMER_CLK_CLKSTBY_PTR        ((reg8 *) TIMER_CLK__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define TIMER_CLK_DIV_LSB            (* (reg8 *) TIMER_CLK__CFG0)
#define TIMER_CLK_DIV_LSB_PTR        ((reg8 *) TIMER_CLK__CFG0)
#define TIMER_CLK_DIV_PTR            ((reg16 *) TIMER_CLK__CFG0)

/* Clock MSB divider configuration register. */
#define TIMER_CLK_DIV_MSB            (* (reg8 *) TIMER_CLK__CFG1)
#define TIMER_CLK_DIV_MSB_PTR        ((reg8 *) TIMER_CLK__CFG1)

/* Mode and source configuration register */
#define TIMER_CLK_MOD_SRC            (* (reg8 *) TIMER_CLK__CFG2)
#define TIMER_CLK_MOD_SRC_PTR        ((reg8 *) TIMER_CLK__CFG2)

#if defined(TIMER_CLK__CFG3)
/* Analog clock phase configuration register */
#define TIMER_CLK_PHASE              (* (reg8 *) TIMER_CLK__CFG3)
#define TIMER_CLK_PHASE_PTR          ((reg8 *) TIMER_CLK__CFG3)
#endif /* defined(TIMER_CLK__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define TIMER_CLK_CLKEN_MASK         TIMER_CLK__PM_ACT_MSK
#define TIMER_CLK_CLKSTBY_MASK       TIMER_CLK__PM_STBY_MSK

/* CFG2 field masks */
#define TIMER_CLK_SRC_SEL_MSK        TIMER_CLK__CFG2_SRC_SEL_MASK
#define TIMER_CLK_MODE_MASK          (~(TIMER_CLK_SRC_SEL_MSK))

#if defined(TIMER_CLK__CFG3)
/* CFG3 phase mask */
#define TIMER_CLK_PHASE_MASK         TIMER_CLK__CFG3_PHASE_DLY_MASK
#endif /* defined(TIMER_CLK__CFG3) */

#endif /* CY_CLOCK_TIMER_CLK_H */


/* [] END OF FILE */
