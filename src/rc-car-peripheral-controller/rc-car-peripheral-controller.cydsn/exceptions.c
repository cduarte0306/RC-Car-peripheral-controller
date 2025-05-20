/* ========================================
 *
 * Copyright Linear Photonics, 2022
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Linear Photonics.
 *
 * File version: v2022.06.23
 * ========================================
*/

#include <project.h>

#include "cyapicallbacks.h"
#include "RC_Config.h"

#if !__has_include("logger.h")
        #define vLoggingPrintfCritical(...) do {} while(0)
#endif

#if CONFIG_INCLUDE_FREERTOS
    #include "FreeRTOS.h"
    #include "FreeRTOSConfig.h"
#endif

/* In older versions of core_cm3.h these are not defined, so define them */
#if (__CM3_CMSIS_VERSION < 0x050000)
    /* MemManage Fault Status Register (part of SCB Configurable Fault Status Register) */
    #define SCB_CFSR_MMARVALID_Pos             (SCB_SHCSR_MEMFAULTACT_Pos + 7U)               /*!< SCB CFSR (MMFSR): MMARVALID Position */
    #define SCB_CFSR_MMARVALID_Msk             (1UL << SCB_CFSR_MMARVALID_Pos)                /*!< SCB CFSR (MMFSR): MMARVALID Mask */

    #define SCB_CFSR_MSTKERR_Pos               (SCB_SHCSR_MEMFAULTACT_Pos + 4U)               /*!< SCB CFSR (MMFSR): MSTKERR Position */
    #define SCB_CFSR_MSTKERR_Msk               (1UL << SCB_CFSR_MSTKERR_Pos)                  /*!< SCB CFSR (MMFSR): MSTKERR Mask */

    #define SCB_CFSR_MUNSTKERR_Pos             (SCB_SHCSR_MEMFAULTACT_Pos + 3U)               /*!< SCB CFSR (MMFSR): MUNSTKERR Position */
    #define SCB_CFSR_MUNSTKERR_Msk             (1UL << SCB_CFSR_MUNSTKERR_Pos)                /*!< SCB CFSR (MMFSR): MUNSTKERR Mask */

    #define SCB_CFSR_DACCVIOL_Pos              (SCB_SHCSR_MEMFAULTACT_Pos + 1U)               /*!< SCB CFSR (MMFSR): DACCVIOL Position */
    #define SCB_CFSR_DACCVIOL_Msk              (1UL << SCB_CFSR_DACCVIOL_Pos)                 /*!< SCB CFSR (MMFSR): DACCVIOL Mask */

    #define SCB_CFSR_IACCVIOL_Pos              (SCB_SHCSR_MEMFAULTACT_Pos + 0U)               /*!< SCB CFSR (MMFSR): IACCVIOL Position */
    #define SCB_CFSR_IACCVIOL_Msk              (1UL /*<< SCB_CFSR_IACCVIOL_Pos*/)             /*!< SCB CFSR (MMFSR): IACCVIOL Mask */

    /* BusFault Status Register (part of SCB Configurable Fault Status Register) */
    #define SCB_CFSR_BFARVALID_Pos            (SCB_CFSR_BUSFAULTSR_Pos + 7U)                  /*!< SCB CFSR (BFSR): BFARVALID Position */
    #define SCB_CFSR_BFARVALID_Msk            (1UL << SCB_CFSR_BFARVALID_Pos)                 /*!< SCB CFSR (BFSR): BFARVALID Mask */

    #define SCB_CFSR_STKERR_Pos               (SCB_CFSR_BUSFAULTSR_Pos + 4U)                  /*!< SCB CFSR (BFSR): STKERR Position */
    #define SCB_CFSR_STKERR_Msk               (1UL << SCB_CFSR_STKERR_Pos)                    /*!< SCB CFSR (BFSR): STKERR Mask */

    #define SCB_CFSR_UNSTKERR_Pos             (SCB_CFSR_BUSFAULTSR_Pos + 3U)                  /*!< SCB CFSR (BFSR): UNSTKERR Position */
    #define SCB_CFSR_UNSTKERR_Msk             (1UL << SCB_CFSR_UNSTKERR_Pos)                  /*!< SCB CFSR (BFSR): UNSTKERR Mask */

    #define SCB_CFSR_IMPRECISERR_Pos          (SCB_CFSR_BUSFAULTSR_Pos + 2U)                  /*!< SCB CFSR (BFSR): IMPRECISERR Position */
    #define SCB_CFSR_IMPRECISERR_Msk          (1UL << SCB_CFSR_IMPRECISERR_Pos)               /*!< SCB CFSR (BFSR): IMPRECISERR Mask */

    #define SCB_CFSR_PRECISERR_Pos            (SCB_CFSR_BUSFAULTSR_Pos + 1U)                  /*!< SCB CFSR (BFSR): PRECISERR Position */
    #define SCB_CFSR_PRECISERR_Msk            (1UL << SCB_CFSR_PRECISERR_Pos)                 /*!< SCB CFSR (BFSR): PRECISERR Mask */

    #define SCB_CFSR_IBUSERR_Pos              (SCB_CFSR_BUSFAULTSR_Pos + 0U)                  /*!< SCB CFSR (BFSR): IBUSERR Position */
    #define SCB_CFSR_IBUSERR_Msk              (1UL << SCB_CFSR_IBUSERR_Pos)                   /*!< SCB CFSR (BFSR): IBUSERR Mask */

    /* UsageFault Status Register (part of SCB Configurable Fault Status Register) */
    #define SCB_CFSR_DIVBYZERO_Pos            (SCB_CFSR_USGFAULTSR_Pos + 9U)                  /*!< SCB CFSR (UFSR): DIVBYZERO Position */
    #define SCB_CFSR_DIVBYZERO_Msk            (1UL << SCB_CFSR_DIVBYZERO_Pos)                 /*!< SCB CFSR (UFSR): DIVBYZERO Mask */

    #define SCB_CFSR_UNALIGNED_Pos            (SCB_CFSR_USGFAULTSR_Pos + 8U)                  /*!< SCB CFSR (UFSR): UNALIGNED Position */
    #define SCB_CFSR_UNALIGNED_Msk            (1UL << SCB_CFSR_UNALIGNED_Pos)                 /*!< SCB CFSR (UFSR): UNALIGNED Mask */

    #define SCB_CFSR_NOCP_Pos                 (SCB_CFSR_USGFAULTSR_Pos + 3U)                  /*!< SCB CFSR (UFSR): NOCP Position */
    #define SCB_CFSR_NOCP_Msk                 (1UL << SCB_CFSR_NOCP_Pos)                      /*!< SCB CFSR (UFSR): NOCP Mask */

    #define SCB_CFSR_INVPC_Pos                (SCB_CFSR_USGFAULTSR_Pos + 2U)                  /*!< SCB CFSR (UFSR): INVPC Position */
    #define SCB_CFSR_INVPC_Msk                (1UL << SCB_CFSR_INVPC_Pos)                     /*!< SCB CFSR (UFSR): INVPC Mask */

    #define SCB_CFSR_INVSTATE_Pos             (SCB_CFSR_USGFAULTSR_Pos + 1U)                  /*!< SCB CFSR (UFSR): INVSTATE Position */
    #define SCB_CFSR_INVSTATE_Msk             (1UL << SCB_CFSR_INVSTATE_Pos)                  /*!< SCB CFSR (UFSR): INVSTATE Mask */

    #define SCB_CFSR_UNDEFINSTR_Pos           (SCB_CFSR_USGFAULTSR_Pos + 0U)                  /*!< SCB CFSR (UFSR): UNDEFINSTR Position */
    #define SCB_CFSR_UNDEFINSTR_Msk           (1UL << SCB_CFSR_UNDEFINSTR_Pos)                /*!< SCB CFSR (UFSR): UNDEFINSTR Mask */
#endif


void CyBoot_IntDefaultHandler_Enomem_Exception_Callback(void)
{
    #if defined(DEBUG)
        __asm("BKPT #0");
    #endif
    
    /* Reset */
    CY_LIB_RESET_CR2_REG |= CY_LIB_RESET_CR2_RESET;
    
    /* For sanity */
    for(;;);
}


// Thanks https://gist.github.com/ryankurte/f2bab6e7e23bb915b7734d4e1780d89f
void CyBoot_IntDefaultHandler_Exception_EntryCallback(void)
{    
    // Grab an instance of the SCB so we can `p/x *scb` from the debugger
    //SCB_Type* scb = SCB;
    
    // Identify hardfault type
    if (SCB->HFSR & SCB_HFSR_FORCED_Msk)
    {
        vLoggingPrintfCritical("psoc: FATAL | Hard fault -");
        
        // Forced hardfault
        // See page 1920 in https://www.cypress.com/file/136211/download
        
        // Copy faults for easy debugging
        //size_t mmu_fault   = (SCB->CFSR >> 0)  & 0xFF;
        //size_t bus_fault   = (SCB->CFSR >> 8)  & 0xFF;
        //size_t usage_fault = (SCB->CFSR >> 16) & 0xFFFF;
        
        // Check for memory manger faults
        if (SCB->CFSR & SCB_CFSR_MMARVALID_Msk)     { vLoggingPrintfCritical(" Fault in Memory Manage Address Register!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_MSTKERR_Msk)       { vLoggingPrintfCritical(" Stacking from exception has caused one or more access violations!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_MUNSTKERR_Msk)     { vLoggingPrintfCritical(" Unstack from exception return has caused one or more access violations!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_DACCVIOL_Msk)      { vLoggingPrintfCritical(" Data access violation!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_IACCVIOL_Msk)      { vLoggingPrintfCritical(" Instruction access violation!\r\n"); }

        // Check for bus faults
        if (SCB->CFSR & SCB_CFSR_BFARVALID_Msk)     { vLoggingPrintfCritical(" Fault in Bus Fault Address Register!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_STKERR_Msk)        { vLoggingPrintfCritical(" Stacking from exception has caused one or more bus faults!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_UNSTKERR_Msk)      { vLoggingPrintfCritical(" Unstack from exception return has caused one or more bus faults!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_IMPRECISERR_Msk)   { vLoggingPrintfCritical(" Imprecise data bus error!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_PRECISERR_Msk)     { vLoggingPrintfCritical(" Precise data bus error return!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_IBUSERR_Msk)       { vLoggingPrintfCritical(" Instruction bus error!\r\n"); }

        // Check for usage faults
        if (SCB->CFSR & SCB_CFSR_DIVBYZERO_Msk)     { vLoggingPrintfCritical(" Div By Zero!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_UNALIGNED_Msk)     { vLoggingPrintfCritical(" Attempt to make an unaligned memory access!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_NOCP_Msk)          { vLoggingPrintfCritical(" Attempt to use unsupported coprocessor instruction!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_INVPC_Msk)         { vLoggingPrintfCritical(" Attempt to load EXC_RETURN into PC illegally!\r\n"); }
        if (SCB->CFSR & SCB_CFSR_INVSTATE_Msk)      { vLoggingPrintfCritical(" Invalid combination of EPSR and instruction!\r\n This means a function pointer is likely null.\r\n"); }
        if (SCB->CFSR & SCB_CFSR_UNDEFINSTR_Msk)    { vLoggingPrintfCritical(" Attempt to execute undefined instruction!\r\n"); }
        
        #if defined(DEBUG)
            __asm("BKPT #0");
        #endif
        
        /* Reset */
        CY_LIB_RESET_CR2_REG |= CY_LIB_RESET_CR2_RESET;
        
        /* For sanity */
        for(;;);

    } 
    else if (SCB->HFSR & SCB_HFSR_VECTTBL_Msk) 
    {
        // Vector table bus fault
        vLoggingPrintfCritical("psoc: FATAL | Vector table bus Fault!\r\n");
        
        
        #if defined(DEBUG)
            __asm("BKPT #0");
        #endif
        
        /* Reset */
        CY_LIB_RESET_CR2_REG |= CY_LIB_RESET_CR2_RESET;
        
        /* For sanity */
        for(;;);
    }
    else if (SCB->HFSR & SCB_HFSR_DEBUGEVT_Msk)
    {
        /* This bit is set if there is a fault related to debug.
         * This is only possible when halting debug is not enabled. 
         * For monitor enabled debug, it only happens for BKPT when the current 
         * priority is higher than the monitor. When both halting and monitor 
         * debug are disabled, it only happens for debug events that are not 
         * ignored (minimally, BKPT). 
         */
        vLoggingPrintfCritical("psoc: FATAL | Debug Fault -");
        
        if (SCB->DFSR & SCB_DFSR_EXTERNAL_Msk) { vLoggingPrintfCritical(" External debug request\r\n"); }
        if (SCB->DFSR & SCB_DFSR_VCATCH_Msk)   { vLoggingPrintfCritical(" Vector catch exception\r\n"); }
        if (SCB->DFSR & SCB_DFSR_DWTTRAP_Msk)  { vLoggingPrintfCritical(" Data Watchpoint and Trace (DWT) trap\r\n"); }
        if (SCB->DFSR & SCB_DFSR_BKPT_Msk)     { vLoggingPrintfCritical(" Breakpoint\r\n"); }
        if (SCB->DFSR & SCB_DFSR_HALTED_Msk)   { vLoggingPrintfCritical(" Halt requested by NVIC\r\n"); }

        #if defined(DEBUG)
            __asm("BKPT #0");
        #endif
        
        /* Reset */
        CY_LIB_RESET_CR2_REG |= CY_LIB_RESET_CR2_RESET;
        
        /* For sanity */
        for(;;);
    }

    vLoggingPrintfCritical("psoc: FATAL | Unknown PSoC Fault!\r\n");
    
    #if defined(DEBUG)
        __asm("BKPT #0");
    #endif
    
    /* Reset */
    CY_LIB_RESET_CR2_REG |= CY_LIB_RESET_CR2_RESET;
    
    /* For sanity */
    for(;;);
}


#if ( CONFIG_INCLUDE_FREERTOS == 1 )
    
    #include "task.h"
    
    #if ( configCHECK_FOR_STACK_OVERFLOW > 0 )
        void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
        {    
            uint32 uxHighWaterMark = uxTaskGetStackHighWaterMark(xTask);

            vLoggingPrintfCritical("sys: FATAL | Stack overflow - <%s@0x%08x>  stack space: %u bytes\r\n", pcTaskName, (void *) xTask, uxHighWaterMark);
                        
            #if defined(DEBUG)
                __asm("BKPT #0");
                for(;;);
            #endif
            
            /* Reset */
            CY_LIB_RESET_CR2_REG |= CY_LIB_RESET_CR2_RESET;
            
            /* For sanity */
            for(;;);
        }
    #endif  /* configCHECK_FOR_STACK_OVERFLOW > 0 */


    #if ( configUSE_MALLOC_FAILED_HOOK > 0 )
        void vApplicationMallocFailedHook( void )
        {
            vLoggingPrintfCritical("sys: FATAL | Malloc failed - heap size: %u bytes\r\n", CYDEV_HEAP_SIZE);
            
            #if defined(DEBUG)
                __asm("BKPT #0");
                for(;;);
            #endif
            
            /* Reset */
            CY_LIB_RESET_CR2_REG |= CY_LIB_RESET_CR2_RESET;
            
            /* For sanity */
            for(;;);
        }
    #endif  /* configUSE_MALLOC_FAILED_HOOK > 0 */
    
#endif  /* CONFIG_INCLUDE_FREERTOS == 1 */

/* [] END OF FILE */