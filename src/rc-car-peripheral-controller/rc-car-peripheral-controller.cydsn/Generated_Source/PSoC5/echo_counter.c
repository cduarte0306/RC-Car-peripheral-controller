/*******************************************************************************
* File Name: echo_counter.c  
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

#include "echo_counter.h"

uint8 echo_counter_initVar = 0u;


/*******************************************************************************
* Function Name: echo_counter_Init
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
void echo_counter_Init(void) 
{
        #if (!echo_counter_UsingFixedFunction && !echo_counter_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!echo_counter_UsingFixedFunction && !echo_counter_ControlRegRemoved) */
        
        #if(!echo_counter_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 echo_counter_interruptState;
        #endif /* (!echo_counter_UsingFixedFunction) */
        
        #if (echo_counter_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            echo_counter_CONTROL &= echo_counter_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                echo_counter_CONTROL2 &= ((uint8)(~echo_counter_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                echo_counter_CONTROL3 &= ((uint8)(~echo_counter_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (echo_counter_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                echo_counter_CONTROL |= echo_counter_ONESHOT;
            #endif /* (echo_counter_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            echo_counter_CONTROL2 |= echo_counter_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            echo_counter_RT1 &= ((uint8)(~echo_counter_RT1_MASK));
            echo_counter_RT1 |= echo_counter_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            echo_counter_RT1 &= ((uint8)(~echo_counter_SYNCDSI_MASK));
            echo_counter_RT1 |= echo_counter_SYNCDSI_EN;

        #else
            #if(!echo_counter_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = echo_counter_CONTROL & ((uint8)(~echo_counter_CTRL_CMPMODE_MASK));
            echo_counter_CONTROL = ctrl | echo_counter_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = echo_counter_CONTROL & ((uint8)(~echo_counter_CTRL_CAPMODE_MASK));
            
            #if( 0 != echo_counter_CAPTURE_MODE_CONF)
                echo_counter_CONTROL = ctrl | echo_counter_DEFAULT_CAPTURE_MODE;
            #else
                echo_counter_CONTROL = ctrl;
            #endif /* 0 != echo_counter_CAPTURE_MODE */ 
            
            #endif /* (!echo_counter_ControlRegRemoved) */
        #endif /* (echo_counter_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!echo_counter_UsingFixedFunction)
            echo_counter_ClearFIFO();
        #endif /* (!echo_counter_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        echo_counter_WritePeriod(echo_counter_INIT_PERIOD_VALUE);
        #if (!(echo_counter_UsingFixedFunction && (CY_PSOC5A)))
            echo_counter_WriteCounter(echo_counter_INIT_COUNTER_VALUE);
        #endif /* (!(echo_counter_UsingFixedFunction && (CY_PSOC5A))) */
        echo_counter_SetInterruptMode(echo_counter_INIT_INTERRUPTS_MASK);
        
        #if (!echo_counter_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)echo_counter_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            echo_counter_WriteCompare(echo_counter_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            echo_counter_interruptState = CyEnterCriticalSection();
            
            echo_counter_STATUS_AUX_CTRL |= echo_counter_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(echo_counter_interruptState);
            
        #endif /* (!echo_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: echo_counter_Enable
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
void echo_counter_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (echo_counter_UsingFixedFunction)
        echo_counter_GLOBAL_ENABLE |= echo_counter_BLOCK_EN_MASK;
        echo_counter_GLOBAL_STBY_ENABLE |= echo_counter_BLOCK_STBY_EN_MASK;
    #endif /* (echo_counter_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!echo_counter_ControlRegRemoved || echo_counter_UsingFixedFunction)
        echo_counter_CONTROL |= echo_counter_CTRL_ENABLE;                
    #endif /* (!echo_counter_ControlRegRemoved || echo_counter_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: echo_counter_Start
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
*  echo_counter_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void echo_counter_Start(void) 
{
    if(echo_counter_initVar == 0u)
    {
        echo_counter_Init();
        
        echo_counter_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    echo_counter_Enable();        
}


/*******************************************************************************
* Function Name: echo_counter_Stop
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
void echo_counter_Stop(void) 
{
    /* Disable Counter */
    #if(!echo_counter_ControlRegRemoved || echo_counter_UsingFixedFunction)
        echo_counter_CONTROL &= ((uint8)(~echo_counter_CTRL_ENABLE));        
    #endif /* (!echo_counter_ControlRegRemoved || echo_counter_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (echo_counter_UsingFixedFunction)
        echo_counter_GLOBAL_ENABLE &= ((uint8)(~echo_counter_BLOCK_EN_MASK));
        echo_counter_GLOBAL_STBY_ENABLE &= ((uint8)(~echo_counter_BLOCK_STBY_EN_MASK));
    #endif /* (echo_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: echo_counter_SetInterruptMode
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
void echo_counter_SetInterruptMode(uint8 interruptsMask) 
{
    echo_counter_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: echo_counter_ReadStatusRegister
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
uint8   echo_counter_ReadStatusRegister(void) 
{
    return echo_counter_STATUS;
}


#if(!echo_counter_ControlRegRemoved)
/*******************************************************************************
* Function Name: echo_counter_ReadControlRegister
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
uint8   echo_counter_ReadControlRegister(void) 
{
    return echo_counter_CONTROL;
}


/*******************************************************************************
* Function Name: echo_counter_WriteControlRegister
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
void    echo_counter_WriteControlRegister(uint8 control) 
{
    echo_counter_CONTROL = control;
}

#endif  /* (!echo_counter_ControlRegRemoved) */


#if (!(echo_counter_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: echo_counter_WriteCounter
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
void echo_counter_WriteCounter(uint16 counter) \
                                   
{
    #if(echo_counter_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (echo_counter_GLOBAL_ENABLE & echo_counter_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        echo_counter_GLOBAL_ENABLE |= echo_counter_BLOCK_EN_MASK;
        CY_SET_REG16(echo_counter_COUNTER_LSB_PTR, (uint16)counter);
        echo_counter_GLOBAL_ENABLE &= ((uint8)(~echo_counter_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(echo_counter_COUNTER_LSB_PTR, counter);
    #endif /* (echo_counter_UsingFixedFunction) */
}
#endif /* (!(echo_counter_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: echo_counter_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) The present value of the counter.
*
*******************************************************************************/
uint16 echo_counter_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(echo_counter_UsingFixedFunction)
		(void)CY_GET_REG16(echo_counter_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(echo_counter_COUNTER_LSB_PTR_8BIT);
	#endif/* (echo_counter_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(echo_counter_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(echo_counter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(echo_counter_STATICCOUNT_LSB_PTR));
    #endif /* (echo_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: echo_counter_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint16) Present Capture value.
*
*******************************************************************************/
uint16 echo_counter_ReadCapture(void) 
{
    #if(echo_counter_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(echo_counter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(echo_counter_STATICCOUNT_LSB_PTR));
    #endif /* (echo_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: echo_counter_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint16) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void echo_counter_WritePeriod(uint16 period) 
{
    #if(echo_counter_UsingFixedFunction)
        CY_SET_REG16(echo_counter_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(echo_counter_PERIOD_LSB_PTR, period);
    #endif /* (echo_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: echo_counter_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) Present period value.
*
*******************************************************************************/
uint16 echo_counter_ReadPeriod(void) 
{
    #if(echo_counter_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(echo_counter_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(echo_counter_PERIOD_LSB_PTR));
    #endif /* (echo_counter_UsingFixedFunction) */
}


#if (!echo_counter_UsingFixedFunction)
/*******************************************************************************
* Function Name: echo_counter_WriteCompare
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
void echo_counter_WriteCompare(uint16 compare) \
                                   
{
    #if(echo_counter_UsingFixedFunction)
        CY_SET_REG16(echo_counter_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(echo_counter_COMPARE_LSB_PTR, compare);
    #endif /* (echo_counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: echo_counter_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint16) Present compare value.
*
*******************************************************************************/
uint16 echo_counter_ReadCompare(void) 
{
    return (CY_GET_REG16(echo_counter_COMPARE_LSB_PTR));
}


#if (echo_counter_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: echo_counter_SetCompareMode
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
void echo_counter_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    echo_counter_CONTROL &= ((uint8)(~echo_counter_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    echo_counter_CONTROL |= compareMode;
}
#endif  /* (echo_counter_COMPARE_MODE_SOFTWARE) */


#if (echo_counter_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: echo_counter_SetCaptureMode
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
void echo_counter_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    echo_counter_CONTROL &= ((uint8)(~echo_counter_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    echo_counter_CONTROL |= ((uint8)((uint8)captureMode << echo_counter_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (echo_counter_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: echo_counter_ClearFIFO
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
void echo_counter_ClearFIFO(void) 
{

    while(0u != (echo_counter_ReadStatusRegister() & echo_counter_STATUS_FIFONEMP))
    {
        (void)echo_counter_ReadCapture();
    }

}
#endif  /* (!echo_counter_UsingFixedFunction) */


/* [] END OF FILE */

