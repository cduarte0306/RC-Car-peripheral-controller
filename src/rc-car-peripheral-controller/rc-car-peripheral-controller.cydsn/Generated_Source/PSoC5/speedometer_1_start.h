/*******************************************************************************
* File Name: speedometer_1_start.h  
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

#if !defined(CY_CONTROL_REG_speedometer_1_start_H) /* CY_CONTROL_REG_speedometer_1_start_H */
#define CY_CONTROL_REG_speedometer_1_start_H

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

} speedometer_1_start_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    speedometer_1_start_Write(uint8 control) ;
uint8   speedometer_1_start_Read(void) ;

void speedometer_1_start_SaveConfig(void) ;
void speedometer_1_start_RestoreConfig(void) ;
void speedometer_1_start_Sleep(void) ; 
void speedometer_1_start_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define speedometer_1_start_Control        (* (reg8 *) speedometer_1_start_Sync_ctrl_reg__CONTROL_REG )
#define speedometer_1_start_Control_PTR    (  (reg8 *) speedometer_1_start_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_speedometer_1_start_H */


/* [] END OF FILE */
