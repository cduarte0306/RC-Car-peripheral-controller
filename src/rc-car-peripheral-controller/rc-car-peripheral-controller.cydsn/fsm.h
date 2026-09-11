#ifndef FSM_H
#define FSM_H

#include <stdint.h>

struct Transitions;

typedef struct FsmHandle
{
    uint8_t state;
    uint8_t numTransitions;  /**< Number of transitions */
    void (*callback)(void*);
    struct Transitions* transitions;  // Array of transitions
} tFsmState;

typedef struct Transitions
{
    uint8_t state;        /**< State to transition to */
    uint8_t transitionId; /**< ID to move FSM to this transition */
    tFsmState* stateHandle;
} tTransitions;

typedef struct FSM
{
    uint8_t state;        /**< State to transition to */
    tFsmState stateHandle;
} tFsm;

typedef struct
{
    uint8_t numSteps;
    uint8_t started;
    void* base;
    tFsmState* stateHdl;
} tFsmHandle;

/**
 * @brief Connect the FSM's states to one another
 * 
 * @param fsmConf Pointer to FSM configuration struct
 * @param startingState FSM starting state
 * @param n   Number of FSM states
 * @return    Handle to current FSM stage 
 */
uint8_t FSM_Init(tFsmHandle* hdl, tFsm* fsmConf, uint8_t startingState, uint8_t n);

/**
 * @brief Trigger the next step to the current FSM state
 * 
 * @param hdl Pointer to FSM handle
 * @param state FSM state to trigger
 * @return uint8_t Error code
 */
uint8_t FSM_Step(tFsmHandle* hdl, uint8_t state);

/**
 * @brief Run poll on fsm handle
 * 
 * @param hdl Pointer to FSM handle
 * @return uint8_t Error code
 */
uint8_t FSM_Poll(tFsmHandle* hdl, void* arg);

#endif // FSM_H