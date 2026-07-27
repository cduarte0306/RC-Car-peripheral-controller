/*******************************************************************************
* File Name: sim_sel.h  
* Version 1.80
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CONTROL_REG_sim_sel_H) /* CY_CONTROL_REG_sim_sel_H */
#define CY_CONTROL_REG_sim_sel_H

#include "cyfitter.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))
    #include "cytypes.h"
#else
    #include "syslib/cy_syslib.h"
#endif

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} sim_sel_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    sim_sel_Write(uint8 control) ;
uint8   sim_sel_Read(void) ;

void sim_sel_SaveConfig(void) ;
void sim_sel_RestoreConfig(void) ;
void sim_sel_Sleep(void) ; 
void sim_sel_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define sim_sel_Control        (* (reg8 *) sim_sel_Sync_ctrl_reg__CONTROL_REG )
#define sim_sel_Control_PTR    (  (reg8 *) sim_sel_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_sim_sel_H */


/* [] END OF FILE */
