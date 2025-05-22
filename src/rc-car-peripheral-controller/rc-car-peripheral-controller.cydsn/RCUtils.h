/* ========================================
 *
 * Copyright Linear Photonics, 2022
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Linear Photonics.
 *
 * File version: v2022.03.17
 * ========================================
 */
#ifndef LPUTILS_H
#define LPUTILS_H

#include "app_utils.h"
    
#include "logging.h"
    
    
#if defined ( TIMESTAMP_INCLUDE_OK )
    #include "TREG0.h"
    #include "TREG1.h"
    #include "TREG2.h"
    #include "TREG3.h"
#endif


#if defined ( UART_INCLUDE_OK )
    #include UART_H
    #include "logging.h"
#endif

#if defined ( FREERTOS_INCLUDE_OK )
    #include "FreeRTOSConfig.h"
    #include "FreeRTOS.h"
    #include "task.h"
#endif

#if defined ( LWIP_INCLUDE_OK )
    #include "lwipopts.h"
#endif

#if defined ( KSZ8851SNL_INCLUDE_OK )
    #include "ksz8851snl_config.h"
    #include "ksz8851snl.h"

    #if defined ( LWIP_INCLUDE_OK )
        #include "ksz8851snl/lwip/netif_ksz8851snl.h"
    #elif defined ( FREERTOS_INCLUDE_OK )
        #include "FreeRTOSIPConfig.h"
        #include "ksz8851snl/FreeRTOS/NetworkInterface.h"
    #else
    #endif
#endif

#endif  /* LPUTILS_H */
    