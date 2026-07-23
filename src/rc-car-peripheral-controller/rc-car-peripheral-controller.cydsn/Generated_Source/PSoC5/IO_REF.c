/*******************************************************************************
* File Name: IO_REF.c  
* Version 1.90
*
* Description:
*  This file provides the source code to the API for the 8-bit Voltage DAC 
*  (VDAC8) User Module.
*
* Note:
*  Any unusual or non-standard behavior should be noted here. Other-
*  wise, this section should remain blank.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "IO_REF.h"

#if (CY_PSOC5A)
#include <CyLib.h>
#endif /* CY_PSOC5A */

uint8 IO_REF_initVar = 0u;

#if (CY_PSOC5A)
    static uint8 IO_REF_restoreVal = 0u;
#endif /* CY_PSOC5A */

#if (CY_PSOC5A)
    static IO_REF_backupStruct IO_REF_backup;
#endif /* CY_PSOC5A */


/*******************************************************************************
* Function Name: IO_REF_Init
********************************************************************************
* Summary:
*  Initialize to the schematic state.
* 
* Parameters:
*  void:
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void IO_REF_Init(void) 
{
    IO_REF_CR0 = (IO_REF_MODE_V );

    /* Set default data source */
    #if(IO_REF_DEFAULT_DATA_SRC != 0 )
        IO_REF_CR1 = (IO_REF_DEFAULT_CNTL | IO_REF_DACBUS_ENABLE) ;
    #else
        IO_REF_CR1 = (IO_REF_DEFAULT_CNTL | IO_REF_DACBUS_DISABLE) ;
    #endif /* (IO_REF_DEFAULT_DATA_SRC != 0 ) */

    /* Set default strobe mode */
    #if(IO_REF_DEFAULT_STRB != 0)
        IO_REF_Strobe |= IO_REF_STRB_EN ;
    #endif/* (IO_REF_DEFAULT_STRB != 0) */

    /* Set default range */
    IO_REF_SetRange(IO_REF_DEFAULT_RANGE); 

    /* Set default speed */
    IO_REF_SetSpeed(IO_REF_DEFAULT_SPEED);
}


/*******************************************************************************
* Function Name: IO_REF_Enable
********************************************************************************
* Summary:
*  Enable the VDAC8
* 
* Parameters:
*  void
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void IO_REF_Enable(void) 
{
    IO_REF_PWRMGR |= IO_REF_ACT_PWR_EN;
    IO_REF_STBY_PWRMGR |= IO_REF_STBY_PWR_EN;

    /*This is to restore the value of register CR0 ,
    which is modified  in Stop API , this prevents misbehaviour of VDAC */
    #if (CY_PSOC5A)
        if(IO_REF_restoreVal == 1u) 
        {
             IO_REF_CR0 = IO_REF_backup.data_value;
             IO_REF_restoreVal = 0u;
        }
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: IO_REF_Start
********************************************************************************
*
* Summary:
*  The start function initializes the voltage DAC with the default values, 
*  and sets the power to the given level.  A power level of 0, is the same as
*  executing the stop function.
*
* Parameters:
*  Power: Sets power level between off (0) and (3) high power
*
* Return:
*  void 
*
* Global variables:
*  IO_REF_initVar: Is modified when this function is called for the 
*  first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void IO_REF_Start(void)  
{
    /* Hardware initiazation only needs to occure the first time */
    if(IO_REF_initVar == 0u)
    { 
        IO_REF_Init();
        IO_REF_initVar = 1u;
    }

    /* Enable power to DAC */
    IO_REF_Enable();

    /* Set default value */
    IO_REF_SetValue(IO_REF_DEFAULT_DATA); 
}


/*******************************************************************************
* Function Name: IO_REF_Stop
********************************************************************************
*
* Summary:
*  Powers down DAC to lowest power state.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void IO_REF_Stop(void) 
{
    /* Disble power to DAC */
    IO_REF_PWRMGR &= (uint8)(~IO_REF_ACT_PWR_EN);
    IO_REF_STBY_PWRMGR &= (uint8)(~IO_REF_STBY_PWR_EN);

    /* This is a work around for PSoC5A  ,
    this sets VDAC to current mode with output off */
    #if (CY_PSOC5A)
        IO_REF_backup.data_value = IO_REF_CR0;
        IO_REF_CR0 = IO_REF_CUR_MODE_OUT_OFF;
        IO_REF_restoreVal = 1u;
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: IO_REF_SetSpeed
********************************************************************************
*
* Summary:
*  Set DAC speed
*
* Parameters:
*  power: Sets speed value
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void IO_REF_SetSpeed(uint8 speed) 
{
    /* Clear power mask then write in new value */
    IO_REF_CR0 &= (uint8)(~IO_REF_HS_MASK);
    IO_REF_CR0 |=  (speed & IO_REF_HS_MASK);
}


/*******************************************************************************
* Function Name: IO_REF_SetRange
********************************************************************************
*
* Summary:
*  Set one of three current ranges.
*
* Parameters:
*  Range: Sets one of Three valid ranges.
*
* Return:
*  void 
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void IO_REF_SetRange(uint8 range) 
{
    IO_REF_CR0 &= (uint8)(~IO_REF_RANGE_MASK);      /* Clear existing mode */
    IO_REF_CR0 |= (range & IO_REF_RANGE_MASK);      /*  Set Range  */
    IO_REF_DacTrim();
}


/*******************************************************************************
* Function Name: IO_REF_SetValue
********************************************************************************
*
* Summary:
*  Set 8-bit DAC value
*
* Parameters:  
*  value:  Sets DAC value between 0 and 255.
*
* Return: 
*  void 
*
* Theory: 
*
* Side Effects:
*
*******************************************************************************/
void IO_REF_SetValue(uint8 value) 
{
    #if (CY_PSOC5A)
        uint8 IO_REF_intrStatus = CyEnterCriticalSection();
    #endif /* CY_PSOC5A */

    IO_REF_Data = value;                /*  Set Value  */

    /* PSOC5A requires a double write */
    /* Exit Critical Section */
    #if (CY_PSOC5A)
        IO_REF_Data = value;
        CyExitCriticalSection(IO_REF_intrStatus);
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: IO_REF_DacTrim
********************************************************************************
*
* Summary:
*  Set the trim value for the given range.
*
* Parameters:
*  range:  1V or 4V range.  See constants.
*
* Return:
*  void
*
* Theory: 
*
* Side Effects:
*
*******************************************************************************/
void IO_REF_DacTrim(void) 
{
    uint8 mode;

    mode = (uint8)((IO_REF_CR0 & IO_REF_RANGE_MASK) >> 2) + IO_REF_TRIM_M7_1V_RNG_OFFSET;
    IO_REF_TR = CY_GET_XTND_REG8((uint8 *)(IO_REF_DAC_TRIM_BASE + mode));
}


/* [] END OF FILE */
