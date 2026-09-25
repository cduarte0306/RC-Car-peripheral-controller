#ifndef MOTORBOOTLOADERDEFS_H
#define MOTORBOOTLOADERDEFS_H

#include <stdint-gcc.h>

#define APPL_METADATA         0x8000U
#define APPL_START_ADDR       0x8100U

#define CY_ROW_LENGTH         256U                                // Number of bytes per row in PSoC flash
#define CY_FLASH_SECTOR_SIZE  65536u                              // FLash sector size
#define CY_BL_REGION_END      APPL_START_ADDR                     // Ending memory address of bootloader region in flash
#define CY_BL_NUM_ROWS        1024U                               // Maximum number of rows in flash
#define CY_IMAGE_MAX_SIZE     0x38000U                            // Max address space of main app
#define CY_FIRST_APP_ROW      (APPL_START_ADDR / CY_ROW_LENGTH)   // First app row

#ifdef __cplusplus
extern "C" {
namespace BlDefs {
#endif
enum
{
    BootLoader_WriteRow,        /**< Write to specified row  */
    Bootloader_Verify_Write,    /**< Verify write            */
    Bootloader_Finalize,        /**< Finalize update process */
};

/**
 * @brief BL status codes
 * 
 */
enum
{
    Bl_Noop,
    Bl_Ok,
    Bl_WriteInProgress,
    Bl_Err,
    Bl_Finished    
};

typedef struct _HEADER
{
    uint16_t len;
} tHdr;

typedef struct _CMDHDR
{
    uint8_t cmd;
    uint32_t row;
    uint32_t crc32;
    uint32_t status;
} tCmdHdr;

typedef struct _BLXFER
{
    tHdr hdr;
    tCmdHdr cmdHdr;
} tBlXfer;

#ifdef __cplusplus
}
}
#endif

#endif