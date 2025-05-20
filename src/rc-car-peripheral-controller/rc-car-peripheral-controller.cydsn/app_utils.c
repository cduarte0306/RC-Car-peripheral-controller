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
#include <project.h>
#include "app_utils.h"



#if (CONFIG_INCLUDE_CRC == 1u)
    /* Thanks http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
     * CRC lookuptable for 
     *     polynomial:       0x4C11DB7
     *     input reflected:  No
     *     result reflected: No
     */
    static const uint32_t crc_table[256] =
    {
        0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
        0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
        0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
        0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
        0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
        0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
        0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
        0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
        0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
        0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
        0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
        0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
        0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
        0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
        0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
        0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
        0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
        0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
        0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
        0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
        0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
        0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
        0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
        0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
        0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
        0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
        0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
        0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
        0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
        0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
        0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
        0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
    };


    /**
     * @brief Reflects the bits in an 8-bit value. That is last bit because the first
     *        bit, and the first bit becomes the last bit, and same for all other bits
     *
     * @param[in] [val] Value to do bit reversal on
     *
     * @return uint8 Resultant bit reversed value
     */
    static uint8 reflect8(uint8 val)
    {
        uint8 ret = 0;

        for (int i = 0; i < 8; i++)
        {
            if ((val & (1 << i)) != 0)
            {
                ret |= (uint8)(1 << (7 - i));
            }
        }

        return ret;
    }


    /**
     * @brief Calculates 32-bit CRC value based on given byte sequence
     *
     * Thanks https://en.wikipedia.org/wiki/Cyclic_redundancy_check
     * Thanks http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
     *
     * CRC parameters:
     *     Bit length:       CRC-32
     *     Polynomial:       0x4C11DB7
     *     Initial value:    0xFFFFFFFF
     *     Final Xor Value:  0x0
     *     Input reflected:  Yes
     *     Result reflected: No
     *
     * @param[in] [data] Sequence of bytes to calculate the CRC for
     * @param[in] [len]  Number of bytes in the sequence
     *
     * @return uint32 CRC-32 value for the byte sequence specified
     */
    uint32 xCRC32(const uint8 data[], uint8 len) 
    {
        /* Initial value is 0xFFFFFFFF */
    	uint32 crc32 = 0xFFFFFFFFu;
    	
    	for (uint8 i = 0; i < len; i++) 
        {
            /* Reflect input and XOR-in next input byte into MSB of crc and get this MSB, that's our new intermediate divident */
    		const uint8 pos = (uint8)((crc32 ^ (reflect8(data[i]) << 24)) >> 24);
            
            /* Shift out the MSB used for division per lookuptable and XOR with the remainder */
    		crc32 = (uint32)((crc32 << 8) ^ (uint32)(crc_table[pos]));
    	}
    	
        /* Result is not reflected */
    	return crc32;
    }
#endif


#if (CONFIG_INCLUDE_FLETCHER16 == 1u)
/**
 * @brief Calculates or checks error checking key based on Fletcher16
 *
 * @param[in] data Pointer to data to operate the key on
 * @param[in] size Size of the data
 * @param[in] check If TRUE (1) the data is checked for corruption, if FALSE (0) the error checking key is generated
 */
uint16 xFletcher16(uint8 *data, uint16 size, uint8 check)
{
    uint32 sum1 = 0;
    uint32 sum2 = 0;
    
    /* So that iterator starts from the end of the
     * data instead the beggining */
    data += size - 1;

    /* Exclude the first 2 bytes if the error checking key
     * is being generated. First 2 bytes is where the yet 
     * to be calculated error checking key goes */
    if (not check)
    {
        size -= 2;
    }
    
    while (size--)
    {
        sum1 += *data;
        sum2 += sum1;
        
        /* sum1 is never going to overflow given that max possible sum is 255*2^16
         * sum2 is cumulative sum, and can overflow */
        if(sum2 >= 1048576) sum2 %= 255;
        
        data--;
    }

    sum1 %= 255;
    sum2 %= 255;

    /* Is it for checking the key? The key would be included as
     * part of the data, and this result should be 0 */
    if (check)
    {
        return (sum2 << 8) | sum1;
    }

    /* Otherwise it's for generating the key. */
    uint8 key1 = ~((sum1 + sum2) % 255);
    uint8 key2 = ~((sum1 + key1) % 255);
    
    return ((uint16) key1 << 8) | (uint16) key2;
}
#endif


#if defined ( TIMESTAMP_INCLUDE_OK )
        /**
        * @brief Gives time elapsed since this function was called last
        *   Reads the timer counter value and returns the difference
        *   to the input value (previous timer value)
        *
        * @param[in] [prev_time] Previous timestamp value
        *
        * @return uint32 
        *   Elapsed time (in ms)
        */
        uint32 xGetElapsed(uint32 prev_time)
        {
            uint32 curr_time = xGetTimestamp();
            if (prev_time > curr_time)
            {
                /* Value has overflowed */
                return 0xFFFFFFFF - prev_time + curr_time;
            }
    
            return curr_time - prev_time;
        }
        
        uint32 xGetTimestamp( void )
        {
            uint32 timestamp0 = 0;
            uint32 timestamp1 = 1;
            
            /* Due to the non-atmoic nature of reading these registers (being read one-by-one),
             * there is a chance that these can be read as the counter is transitioning value. 
             * Normally this is not an issue as `TREG0` increaments by 1, however it becomes an issue
             * when `TREGx` is at 0xFF and is about to rollover to the next register. At that moment
             * `TREG[x]` can be 0xFF and in between CPU cycles it rolls over and increments `TREG[x+1]`.
             * As a result, the timestamp could appear to have jumped by 255 ms into the future. If
             * this is used with `xGetElapsed`, then the start time may appear greater than the next
             * read time, thus giving an invalid elapsed calculation.
             *
             * This is fixed by ensuring that two sequential reads of the registers are the same (it is
             * not updating in between clock cycles).
             */
            do {
                timestamp0 = ( (uint32) (TREG0_Status) | (TREG1_Status << 8) | (TREG2_Status << 16) | (TREG3_Status << 24) );
                timestamp1 = ( (uint32) (TREG0_Status) | (TREG1_Status << 8) | (TREG2_Status << 16) | (TREG3_Status << 24) );
            } while( timestamp0 != timestamp1 );
            
            return timestamp0;
        }
#endif  /* CONFIG_INCLUDE_TIMESTAMP */



#if (CONFIG_INCLUDE_RAND == 1u)
    #if defined ( CY_PRS_PRS_H )
        /**
         * @brief Gets a pseudo-random value
         *
         * @param[out] [pulNumber] Randomly generated value
         *
         * @return uint32 Always pdPASS (1)
         */
        BaseType_t xApplicationGetRandomNumber(uint32* pulNumber)
        {
            *pulNumber = PRS_Read();
            
            return pdPASS;
        }
    #else
        #error "PRS_Read() is unvailable because CY_PRS_PRS_H is not defined."
    #endif
#endif



#if ((CONFIG_INCLUDE_KSZ8851SNL == 1u) || (CONFIG_INCLUDE_LWIP == 1u))
    #if (CONFIG_INCLUDE_RAND == 1u) && (defined(CY_PRS_PRS_H))
        /**
         * @brief Callback that provides the inputs necessary to generate a randomized TCP
         * Initial Sequence Number per RFC 6528.  
         *
         * In this case just a psuedo random number is used so THIS IS NOT RECOMMENDED FOR PRODUCTION SYSTEMS.
         *
         * @param[in] ulSourceAddress 
         * @param[in] usSourcePort 
         * @param[in] ulDestinationAddress 
         * @param[in] usDestinationPort 
         *
         * @return uint32 Randomly generated value
         */
        uint32 ulApplicationGetNextSequenceNumber(uint32 ulSourceAddress, uint16 usSourcePort, uint32 ulDestinationAddress, uint16 usDestinationPort)
        {
            (void) ulSourceAddress;
            (void) usSourcePort;
            (void) ulDestinationAddress;
            (void) usDestinationPort;

            /* TODO: Use algorithm outlined here https://datatracker.ietf.org/doc/html/rfc6528#section-3 */
            return PRS_Read();
        }
    #else
        #warning "PRS_Read() is unvailable because CY_PRS_PRS_H is not defined."

        uint32 ulApplicationGetNextSequenceNumber(uint32 ulSourceAddress, uint16 usSourcePort, uint32 ulDestinationAddress, uint16 usDestinationPort)
        {
            (void) ulSourceAddress;
            (void) usSourcePort;
            (void) ulDestinationAddress;
            (void) usDestinationPort;
            
            #if (defined(GET_TIMESTAMP_DEF))
                #warning "PRS_Read() is unvailable! Initial Sequence Number for TCP will NOT be random and is subject to time-based predictable ISN selection!"
                return xGetTimestamp();
            #else
                #warning "xGetTimestamp() and PRS_Read() is unavailable! Initial Sequence Number for TCP will be 0, which is subject to ISN selection!"
                return 0;
            #endif
        }
    #endif  /* if CONFIG_INCLUDE_RAND && ifdef CY_PRS_PRS_H */
#endif /* CONFIG_INCLUDE_KSZ8851SNL || CONFIG_INCLUDE_LWIP */

/* [] END OF FILE */
