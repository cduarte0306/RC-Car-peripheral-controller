/* ========================================
 *
 * Copyright Linear Photonics, 2022
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Linear Photonics.
 *
 * ========================================
*/

#ifndef APP_UTILS_H
#define APP_UTILS_H

#include "types.h"
#include "RC_Config.h"

#if defined(CyWdtClear)
    #undef CyWdtClear
    #define CyWdtClear()  CY_WDT_CR_REG = CY_WDT_CR_FEED
#endif

#if ( CONFIG_INCLUDE_CRC == 1u )
    uint32 xCRC32(const uint8 data[], uint8 len);
#endif

#if ( CONFIG_INCLUDE_FLETCHER16 == 1u )
    uint16 xFletcher16(uint8 *data, uint16 size, uint8 check);
#endif

#if ((CONFIG_INCLUDE_KSZ8851SNL == 1u) || (CONFIG_INCLUDE_LWIP == 1u))
    uint32 ulApplicationGetNextSequenceNumber(uint32 ulSourceAddress, uint16 usSourcePort, uint32 ulDestinationAddress, uint16 usDestinationPort);
#endif


#if ( CONFIG_INCLUDE_RAND == 1u )
    long xApplicationGetRandomNumber(uint32* pulNumber);
#endif

#define ONE_MS   (    1u)
#define ONE_SEC  ( 1000u)
#define ONE_MIN  (60000u)

#if defined ( TIMESTAMP_INCLUDE_OK )
    uint32 xGetTimestamp( void );
    uint32 xGetElapsed(uint32 prev_time);
#else
    #define xGetTimestamp()   0lu
    #define xGetElapsed(prev_time)  0u
#endif

#if ! defined ( FREERTOS_INCLUDE_OK )
    #ifndef vTaskSuspendAll
        #define vTaskSuspendAll()
    #endif

    #ifndef xTaskResumeAll
        #define xTaskResumeAll()
    #endif

    #ifndef vTaskDelay
        #define vTaskDelay(x)  CyDelay(x)
    #endif

    #ifndef pdMS_TO_TICKS
        #define pdMS_TO_TICKS(x)
    #endif

    #ifndef BaseType_t
        #define BaseType_t  int32
    #endif

    #ifndef pdFAIL
        #define pdFAIL    0
    #endif

    #ifndef pdFALSE
        #define pdFALSE   0
    #endif

    #ifndef pdPASS
        #define pdPASS    1
    #endif

    #ifndef pdTRUE
        #define pdTRUE    1
    #endif
#endif


/* This defines a scope from within which the code can call `break;` to
 * exit from. Another way to think about it is as if this is a function,
 * but `break` is called instead of `return`.
 *
 * It allows to avoid deep nesting due to dependent conditions. So for
 * example:
 *
 *     if (foo1() == RET_PASS)
 *     {
 *         if (foo2() == RET_PASS)
 *         {
 *             ...
 *         }
 *         else
 *         {
 *             error_count++;
 *         }
 *     }
 *     else
 *     {
 *         error_count++;
 *     }
 *
 *     printf(error_count);
 *
 * Can be rewritten as:
 *
 *     NESTED (
 *         if (foo1() != RET_PASS)
 *         {
 *             error_count++;
 *             break;
 *         }
 *
 *         if (foo2() != RET_PASS)
 *         {
 *             error_count++;
 *             break;
 *         }
 *     )
 *
 *     printf(error_count);
 */
#define NESTED(x)  \
  while (1)        \
  {                \
    x              \
                   \
    break;         \
  }


#define IF_ELSE(cond, opt1, opt2) \
   if (cond) opt1                 \
   else      opt2


#define WAIT_OR_TIMEOUT(timeout, while_cond, fail_op)         \
    time_start = xGetTimestamp();                             \
    while ( while_cond )                                      \
    {                                                         \
        vTaskDelay(1);                                        \
                                                              \
        if ( xGetElapsed(time_start) > timeout )              \
        {                                                     \
            fail_op                                           \
        }                                                     \
    }

#define LP_YEAR ((((__DATE__[7] - '0')*10 + (__DATE__[8] - '0'))*10 + (__DATE__[9] - '0'))*10 + (__DATE__[10] - '0'))
#define LP_MONTH (__DATE__[2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
: __DATE__[2] == 'b' ? 2 \
: __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 3 : 4) \
: __DATE__[2] == 'y' ? 5 \
: __DATE__[2] == 'l' ? 7 \
: __DATE__[2] == 'g' ? 8 \
: __DATE__[2] == 'p' ? 9 \
: __DATE__[2] == 't' ? 10 \
: __DATE__[2] == 'v' ? 11 : 12)
#define LP_DAY ((__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0')*10 + (__DATE__[5] - '0'))
#define LP_DATE_AS_INT (LP_YEAR*10000 + LP_MONTH*100 + LP_DAY)


#ifndef PIN_READ
    #define PIN_READ(pin)        ( (pin##_PS & pin##_MASK) >> pin##_SHIFT )
#endif

#ifndef PIN_WRITE
    #define PIN_WRITE(pin, val)  pin##_DR = (pin##_DR & (uint8)(~pin##_MASK)) | ((uint8) ((val) << pin##_SHIFT) & pin##_MASK)
#endif

#ifndef ARR_LEN
    #define ARR_LEN(arr)         (sizeof(arr) / sizeof(arr[0]))
#endif

#ifndef FALSE
    #define FALSE     0
#endif

#ifndef RET_FAIL
    #define RET_FAIL  0
#endif

#ifndef OFF
    #define OFF       0
#endif

#ifndef LOW
    #define LOW       0
#endif

#ifndef TRUE
    #define TRUE      1
#endif

#ifndef RET_PASS
    #define RET_PASS  1
#endif

#ifndef ON
    #define ON        1
#endif

#ifndef HIGH
    #define HIGH      1
#endif

#ifndef not
    #define not       !
#endif

#ifndef NULL
    #define NULL      0
#endif

#if defined(DEBUG)
    #define IS_DEBUG    (TRUE)
#else
    #define IS_DEBUG    (FALSE)
#endif


/**
 * If the condition is False, the compiler will complain that the array size is negative,
 * meaning an assertion failure.
 *
 * For information on the macro, follow this link:
 * https://stackoverflow.com/questions/4079243/how-can-i-use-sizeof-in-a-preprocessor-macro/18511691#18511691
 */
#ifndef ASSERT_TRUE
    #define ASSERT_TRUE(condition) ((void) sizeof(char[1 - 2*!!(!(condition))]))
#endif


/* Allows to examine compile time values by hovering over macro usage and seeing resultant error
 * Useful for debugging `ASSERT_TRUE` failures
 *
 * From https://stackoverflow.com/a/35261673 */
#ifndef COMPILE_SHOW_VAL
    #define COMPILE_SHOW_VAL(val) char (*_)[ val ] = 1
#endif


#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1) : __FILE__)

/* Allows to access type's members */
#ifndef SIZEOF_MEMBER
    #define SIZEOF_MEMBER(type, member)  sizeof(((type *) 0)->member)
#endif


#ifndef min
    #define min(a, b) (((a)<(b))? (a): (b))
#endif

#ifndef max
    #define max(a, b) (((a)>(b))? (a): (b))
#endif

#ifndef lerp
    #define lerp(x, x0, y0, x1, y1) (((float) (y0)) + (((float) (x)) - ((float) (x0)))*((((float) (y1)) - ((float) (y0)))/(((float) (x1)) - ((float) (x0)))))
#endif

#ifndef absf
    #define absf(x) (((x) > 0)? (x): -(x))
#endif

#ifndef eqf
    #define eqf(a, b, comp) (absf(a - b) < comp)
#endif

#ifndef is_btwn_inc
    #define is_btwn_inc(a, x, b) (((a) <= (x)) && ((x) <= (b)))
#endif

#ifndef is_btwn_lft
    #define is_btwn_lft(a, x, b) (((a) <= (x)) && ((x) < (b)))
#endif

#ifndef is_btwn_rgt
    #define is_btwn_rgt(a, x, b) (((a) < (x)) && ((x) <= (b)))
#endif

#ifndef is_btwn_exc
    #define is_btwn_exc(a, x, b) (((a) < (x)) && ((x) < (b)))
#endif

#ifndef bound
    #define bound(a, x, b)  min(b, max(x, a))
#endif

#ifndef abs_diff
    #define abs_diff(a, b) (max(a, b) - min(a, b))
#endif

#ifndef get_bit
    #define get_bit(x, n)  (((x) & (1 << (n))) > 0)
#endif

#ifndef set_bit
    #define set_bit(x, n, v)  \
       x = (v)?               \
         ( (x) |  ( 1 << (n) ) ): \
         ( (x) & ~( 1 << (n) ) );
#endif

#ifndef SET_PIN
    #define SET_PIN(pin, val)   pin##_DR = (pin##_DR & (uint8)(~pin##_MASK)) | ((uint8) ((val) << pin##_SHIFT) & pin##_MASK)
#endif

#ifndef GET_PIN
    #define GET_PIN(pin)        ( (pin##_PS & pin##_MASK) >> pin##_SHIFT )
#endif


typedef struct {
    float _val_avg;
    float _val_dev;

    float _val_dev_avg;
    float _val_dev_dev;

    float val_out;
} val_filter_t;


/**
 * @brief Low-filter used to filter-out high frequency noise from telemetry readings or to
 * provide transition delays.
 *
 * @param[in, out] avg_val Averaged result to be updated
 * @param[in]      val     Value to be low-pass filtered
 * @param[in]      weight  Low pass filter coefficient. The smaller the value, the smaller the frequency cut-off is.
 */
static inline void avg(float *avg_val, const uint16 val, const float weight)
{
    *avg_val += (val - *avg_val)*weight;
}


/**
 * @brief Provides filtering to incoming data that improves precision by a factor of 10 at
 *  the expense of being 4x less accurate. Uses recurrent formulas for exponential average
 *  and deviation
 *
 * @param[in, out] data Context to use for processing
 * @param[in]      val  Latest sample to filter
 *
 * @returns float Filtered value
 */

/** RMA 9/3/2024: This function is too big for inline **/
inline float filter_value(val_filter_t *data, float val)
{
    float diff_dev = data->_val_dev - data->_val_dev_avg;
    float diff_val = val - data->_val_avg;
    
    /* Calculate statistics for deviation to account for stability in data variation */
    data->_val_dev_dev += 0.1*(absf(diff_dev) - data->_val_dev_dev);
    data->_val_dev_avg += 0.1*diff_dev;

    /* Calculate statistics for incoming data  */
    data->_val_dev += 0.1*(absf(diff_val) - data->_val_dev);
    data->_val_avg += 0.1*diff_val;

    /* Resultant filtered value is bounded by deviation from average, effectively performing hysterisis
     * +/- 0.5 is added to bounds to gurantee a minimum of 1 unit hysterisis span */
    data->val_out = bound( data->_val_avg - data->_val_dev - data->_val_dev_dev, data->val_out, data->_val_avg + data->_val_dev + data->_val_dev_dev );

    return data->val_out;
}

#endif  /* APP_UTILS_H */

/* [] END OF FILE */
