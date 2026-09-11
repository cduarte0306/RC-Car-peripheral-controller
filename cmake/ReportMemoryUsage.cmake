# Prints flash (text+data) and RAM (data+bss) usage against the device's
# actual memory sizes (from cm3gcc.ld), as a percentage. Invoked as a
# POST_BUILD step; expects ELF_FILE, CMAKE_SIZE, CY_ROM_SIZE, CY_RAM_SIZE.

execute_process(
    COMMAND ${CMAKE_SIZE} --format=sysv ${ELF_FILE}
    OUTPUT_VARIABLE SIZE_OUTPUT
    RESULT_VARIABLE SIZE_RESULT
)

if(NOT SIZE_RESULT EQUAL 0)
    message(WARNING "Failed to run ${CMAKE_SIZE} on ${ELF_FILE}")
    return()
endif()

set(TEXT_SIZE 0)
set(DATA_SIZE 0)
set(BSS_SIZE 0)

string(REPLACE "\n" ";" SIZE_LINES "${SIZE_OUTPUT}")
foreach(LINE ${SIZE_LINES})
    if(LINE MATCHES "^\\.text[ \t]+([0-9]+)")
        set(TEXT_SIZE ${CMAKE_MATCH_1})
    elseif(LINE MATCHES "^\\.data[ \t]+([0-9]+)")
        set(DATA_SIZE ${CMAKE_MATCH_1})
    elseif(LINE MATCHES "^\\.bss[ \t]+([0-9]+)")
        set(BSS_SIZE ${CMAKE_MATCH_1})
    endif()
endforeach()

math(EXPR FLASH_USED "${TEXT_SIZE} + ${DATA_SIZE}")
math(EXPR RAM_USED "${DATA_SIZE} + ${BSS_SIZE}")

math(EXPR FLASH_PCT_X10 "(${FLASH_USED} * 1000) / ${CY_ROM_SIZE}")
math(EXPR RAM_PCT_X10 "(${RAM_USED} * 1000) / ${CY_RAM_SIZE}")
math(EXPR FLASH_PCT_INT "${FLASH_PCT_X10} / 10")
math(EXPR FLASH_PCT_FRAC "${FLASH_PCT_X10} % 10")
math(EXPR RAM_PCT_INT "${RAM_PCT_X10} / 10")
math(EXPR RAM_PCT_FRAC "${RAM_PCT_X10} % 10")

message(STATUS "Flash: ${FLASH_USED} / ${CY_ROM_SIZE} bytes (${FLASH_PCT_INT}.${FLASH_PCT_FRAC}%)")
message(STATUS "RAM:   ${RAM_USED} / ${CY_RAM_SIZE} bytes (${RAM_PCT_INT}.${RAM_PCT_FRAC}%)")
