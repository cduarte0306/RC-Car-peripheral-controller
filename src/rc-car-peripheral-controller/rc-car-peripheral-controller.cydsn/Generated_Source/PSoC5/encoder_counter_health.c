/*******************************************************************************
* File Name: encoder_counter_health.c  
* Version 3.0
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "encoder_counter_health.h"

uint8 encoder_counter_health_initVar = 0u;


/*******************************************************************************
* Function Name: encoder_counter_health_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void encoder_counter_health_Init(void) 
{
        #if (!encoder_counter_health_UsingFixedFunction && !encoder_counter_health_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!encoder_counter_health_UsingFixedFunction && !encoder_counter_health_ControlRegRemoved) */
        
        #if(!encoder_counter_health_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 encoder_counter_health_interruptState;
        #endif /* (!encoder_counter_health_UsingFixedFunction) */
        
        #if (encoder_counter_health_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            encoder_counter_health_CONTROL &= encoder_counter_health_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                encoder_counter_health_CONTROL2 &= ((uint8)(~encoder_counter_health_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                encoder_counter_health_CONTROL3 &= ((uint8)(~encoder_counter_health_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (encoder_counter_health_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                encoder_counter_health_CONTROL |= encoder_counter_health_ONESHOT;
            #endif /* (encoder_counter_health_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            encoder_counter_health_CONTROL2 |= encoder_counter_health_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            encoder_counter_health_RT1 &= ((uint8)(~encoder_counter_health_RT1_MASK));
            encoder_counter_health_RT1 |= encoder_counter_health_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            encoder_counter_health_RT1 &= ((uint8)(~encoder_counter_health_SYNCDSI_MASK));
            encoder_counter_health_RT1 |= encoder_counter_health_SYNCDSI_EN;

        #else
            #if(!encoder_counter_health_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = encoder_counter_health_CONTROL & ((uint8)(~encoder_counter_health_CTRL_CMPMODE_MASK));
            encoder_counter_health_CONTROL = ctrl | encoder_counter_health_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = encoder_counter_health_CONTROL & ((uint8)(~encoder_counter_health_CTRL_CAPMODE_MASK));
            
            #if( 0 != encoder_counter_health_CAPTURE_MODE_CONF)
                encoder_counter_health_CONTROL = ctrl | encoder_counter_health_DEFAULT_CAPTURE_MODE;
            #else
                encoder_counter_health_CONTROL = ctrl;
            #endif /* 0 != encoder_counter_health_CAPTURE_MODE */ 
            
            #endif /* (!encoder_counter_health_ControlRegRemoved) */
        #endif /* (encoder_counter_health_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!encoder_counter_health_UsingFixedFunction)
            encoder_counter_health_ClearFIFO();
        #endif /* (!encoder_counter_health_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        encoder_counter_health_WritePeriod(encoder_counter_health_INIT_PERIOD_VALUE);
        #if (!(encoder_counter_health_UsingFixedFunction && (CY_PSOC5A)))
            encoder_counter_health_WriteCounter(encoder_counter_health_INIT_COUNTER_VALUE);
        #endif /* (!(encoder_counter_health_UsingFixedFunction && (CY_PSOC5A))) */
        encoder_counter_health_SetInterruptMode(encoder_counter_health_INIT_INTERRUPTS_MASK);
        
        #if (!encoder_counter_health_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)encoder_counter_health_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            encoder_counter_health_WriteCompare(encoder_counter_health_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            encoder_counter_health_interruptState = CyEnterCriticalSection();
            
            encoder_counter_health_STATUS_AUX_CTRL |= encoder_counter_health_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(encoder_counter_health_interruptState);
            
        #endif /* (!encoder_counter_health_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: encoder_counter_health_Enable
********************************************************************************
* Summary:
*     Enable the Counter
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: 
*   If the Enable mode is set to Hardware only then this function has no effect 
*   on the operation of the counter.
*
*******************************************************************************/
void encoder_counter_health_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (encoder_counter_health_UsingFixedFunction)
        encoder_counter_health_GLOBAL_ENABLE |= encoder_counter_health_BLOCK_EN_MASK;
        encoder_counter_health_GLOBAL_STBY_ENABLE |= encoder_counter_health_BLOCK_STBY_EN_MASK;
    #endif /* (encoder_counter_health_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!encoder_counter_health_ControlRegRemoved || encoder_counter_health_UsingFixedFunction)
        encoder_counter_health_CONTROL |= encoder_counter_health_CTRL_ENABLE;                
    #endif /* (!encoder_counter_health_ControlRegRemoved || encoder_counter_health_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: encoder_counter_health_Start
********************************************************************************
* Summary:
*  Enables the counter for operation 
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Global variables:
*  encoder_counter_health_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void encoder_counter_health_Start(void) 
{
    if(encoder_counter_health_initVar == 0u)
    {
        encoder_counter_health_Init();
        
        encoder_counter_health_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    encoder_counter_health_Enable();        
}


/*******************************************************************************
* Function Name: encoder_counter_health_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void encoder_counter_health_Stop(void) 
{
    /* Disable Counter */
    #if(!encoder_counter_health_ControlRegRemoved || encoder_counter_health_UsingFixedFunction)
        encoder_counter_health_CONTROL &= ((uint8)(~encoder_counter_health_CTRL_ENABLE));        
    #endif /* (!encoder_counter_health_ControlRegRemoved || encoder_counter_health_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (encoder_counter_health_UsingFixedFunction)
        encoder_counter_health_GLOBAL_ENABLE &= ((uint8)(~encoder_counter_health_BLOCK_EN_MASK));
        encoder_counter_health_GLOBAL_STBY_ENABLE &= ((uint8)(~encoder_counter_health_BLOCK_STBY_EN_MASK));
    #endif /* (encoder_counter_health_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: encoder_counter_health_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:  
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.   
*
* Return: 
*  void
*
*******************************************************************************/
void encoder_counter_health_SetInterruptMode(uint8 interruptsMask) 
{
    encoder_counter_health_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: encoder_counter_health_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read. 
*
*******************************************************************************/
uint8   encoder_counter_health_ReadStatusRegister(void) 
{
    return encoder_counter_health_STATUS;
}


#if(!encoder_counter_health_ControlRegRemoved)
/*******************************************************************************
* Function Name: encoder_counter_health_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   encoder_counter_health_ReadControlRegister(void) 
{
    return encoder_counter_health_CONTROL;
}


/*******************************************************************************
* Function Name: encoder_counter_health_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    encoder_counter_health_WriteControlRegister(uint8 control) 
{
    encoder_counter_health_CONTROL = control;
}

#endif  /* (!encoder_counter_health_ControlRegRemoved) */


#if (!(encoder_counter_health_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: encoder_counter_health_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:  
*  counter:  New counter value. 
*
* Return: 
*  void 
*
*******************************************************************************/
void encoder_counter_health_WriteCounter(uint32 counter) \
                                   
{
    #if(encoder_counter_health_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (encoder_counter_health_GLOBAL_ENABLE & encoder_counter_health_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        encoder_counter_health_GLOBAL_ENABLE |= encoder_counter_health_BLOCK_EN_MASK;
        CY_SET_REG16(encoder_counter_health_COUNTER_LSB_PTR, (uint16)counter);
        encoder_counter_health_GLOBAL_ENABLE &= ((uint8)(~encoder_counter_health_BLOCK_EN_MASK));
    #else
        CY_SET_REG24(encoder_counter_health_COUNTER_LSB_PTR, counter);
    #endif /* (encoder_counter_health_UsingFixedFunction) */
}
#endif /* (!(encoder_counter_health_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: encoder_counter_health_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint32) The present value of the counter.
*
*******************************************************************************/
uint32 encoder_counter_health_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(encoder_counter_health_UsingFixedFunction)
		(void)CY_GET_REG16(encoder_counter_health_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(encoder_counter_health_COUNTER_LSB_PTR_8BIT);
	#endif/* (encoder_counter_health_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(encoder_counter_health_UsingFixedFunction)
        return ((uint32)CY_GET_REG16(encoder_counter_health_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG24(encoder_counter_health_STATICCOUNT_LSB_PTR));
    #endif /* (encoder_counter_health_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: encoder_counter_health_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint32) Present Capture value.
*
*******************************************************************************/
uint32 encoder_counter_health_ReadCapture(void) 
{
    #if(encoder_counter_health_UsingFixedFunction)
        return ((uint32)CY_GET_REG16(encoder_counter_health_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG24(encoder_counter_health_STATICCOUNT_LSB_PTR));
    #endif /* (encoder_counter_health_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: encoder_counter_health_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint32) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void encoder_counter_health_WritePeriod(uint32 period) 
{
    #if(encoder_counter_health_UsingFixedFunction)
        CY_SET_REG16(encoder_counter_health_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG24(encoder_counter_health_PERIOD_LSB_PTR, period);
    #endif /* (encoder_counter_health_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: encoder_counter_health_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint32) Present period value.
*
*******************************************************************************/
uint32 encoder_counter_health_ReadPeriod(void) 
{
    #if(encoder_counter_health_UsingFixedFunction)
        return ((uint32)CY_GET_REG16(encoder_counter_health_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG24(encoder_counter_health_PERIOD_LSB_PTR));
    #endif /* (encoder_counter_health_UsingFixedFunction) */
}


#if (!encoder_counter_health_UsingFixedFunction)
/*******************************************************************************
* Function Name: encoder_counter_health_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will 
* reflect the new value on the next UDB clock.  The compare output will be 
* driven high when the present counter value compares true based on the 
* configured compare mode setting. 
*
* Parameters:  
*  Compare:  New compare value. 
*
* Return: 
*  void
*
*******************************************************************************/
void encoder_counter_health_WriteCompare(uint32 compare) \
                                   
{
    #if(encoder_counter_health_UsingFixedFunction)
        CY_SET_REG16(encoder_counter_health_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG24(encoder_counter_health_COMPARE_LSB_PTR, compare);
    #endif /* (encoder_counter_health_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: encoder_counter_health_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint32) Present compare value.
*
*******************************************************************************/
uint32 encoder_counter_health_ReadCompare(void) 
{
    return (CY_GET_REG24(encoder_counter_health_COMPARE_LSB_PTR));
}


#if (encoder_counter_health_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: encoder_counter_health_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void encoder_counter_health_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    encoder_counter_health_CONTROL &= ((uint8)(~encoder_counter_health_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    encoder_counter_health_CONTROL |= compareMode;
}
#endif  /* (encoder_counter_health_COMPARE_MODE_SOFTWARE) */


#if (encoder_counter_health_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: encoder_counter_health_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void encoder_counter_health_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    encoder_counter_health_CONTROL &= ((uint8)(~encoder_counter_health_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    encoder_counter_health_CONTROL |= ((uint8)((uint8)captureMode << encoder_counter_health_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (encoder_counter_health_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: encoder_counter_health_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:  
*  void:
*
* Return: 
*  None
*
*******************************************************************************/
void encoder_counter_health_ClearFIFO(void) 
{

    while(0u != (encoder_counter_health_ReadStatusRegister() & encoder_counter_health_STATUS_FIFONEMP))
    {
        (void)encoder_counter_health_ReadCapture();
    }

}
#endif  /* (!encoder_counter_health_UsingFixedFunction) */


/* [] END OF FILE */

