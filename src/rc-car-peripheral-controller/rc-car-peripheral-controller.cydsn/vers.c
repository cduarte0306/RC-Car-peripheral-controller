/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "vers.h"
#include <cytypes.h>


uint8_t major_ = 1;
uint8_t minor_ = 2;
uint8_t build_ = 0;


void getVers(unsigned char* major, unsigned char* minor, unsigned char* build) {
    *major = major_;
    *minor = minor_;
    *build = build_;
}


/* [] END OF FILE */
