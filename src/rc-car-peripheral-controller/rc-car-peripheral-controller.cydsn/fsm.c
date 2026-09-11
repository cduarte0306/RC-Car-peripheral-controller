#include <project.h>

#include "fsm.h"
#include "FreeRTOS.h"
#include "app_utils.h"
#include "logging.h"

#include "FreeRTOS.h"


uint8_t FSM_Init(tFsmHandle* hdl, tFsm* fsmConf, uint8_t startingState, uint8_t n)
{
    if (hdl == NULL || fsmConf == NULL)
    {
        return FALSE;
    }

    tFsmState* startingStateHandle = NULL;

    for (uint8_t i = 0; i < n; i ++)
    {
        tFsmState* stateHandle = &fsmConf[i].stateHandle;
        for (uint8_t j = 0; j < stateHandle->numTransitions; j ++)  // Connect with all handles
        {
            tTransitions* transitions = &stateHandle->transitions[j];
            for (uint8_t k = 0; (k < n) && (transitions->stateHandle == NULL); k ++)
            {
                tFsmState* hdl = &((tFsm*)(fsmConf + k))->stateHandle;
                if (transitions->transitionId == hdl->state)  // The matching state has been found
                {
                    // vPrintf("Mapping: %d -> %d\r\n", stateHandle->state, hdl->state);
                    transitions->stateHandle = hdl;
                    break;
                }
            }
        }

        if (stateHandle->state == startingState)
        {
            startingStateHandle = stateHandle;
        }
    }

    if (startingStateHandle == NULL)
    {
        return FALSE;
    }

    hdl->stateHdl = startingStateHandle;
    hdl->started  = FALSE;
    hdl->numSteps = n;
    hdl->base     = (void*) fsmConf;
    return TRUE; // Success
}

uint8_t FSM_Step(tFsmHandle* hdl, uint8_t state)
{
    if (!hdl)
    {
        return FALSE;
    }

    if (hdl->stateHdl->state == state) // We're already on this state
    {
        return TRUE;
    }

    tFsmState* stateHdl = hdl->stateHdl;
    for (uint8_t i = 0; i < stateHdl->numTransitions; i++)
    {
        tTransitions* transitions = &stateHdl->transitions[i];
        if (transitions->state == state)
        {
            hdl->stateHdl = transitions->stateHandle;
            return TRUE;
        }
    }
    return FALSE;
}

uint8_t FSM_Poll(tFsmHandle* hdl, void* arg)
{
    if (!hdl)
    {
        return FALSE;
    }

    hdl->stateHdl->callback(arg);
    return TRUE;
}