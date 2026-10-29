/**
 * @file  fsm.h
 * @brief Table-driven finite state machine.
 *
 * Behaviour lives in a const transition table (placed in flash), which keeps
 * the logic reviewable at a glance and easy to trace against requirements.
 */
#ifndef FSM_H
#define FSM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*fsm_action_t)(void *ctx);

typedef struct {
    uint8_t state;          /**< current state */
    uint8_t event;          /**< triggering event */
    uint8_t next;           /**< state after the transition */
    fsm_action_t action;    /**< optional, may be NULL */
} fsm_transition_t;

typedef struct {
    const fsm_transition_t *table;
    size_t count;
    uint8_t state;
    void *ctx;              /**< user data passed to actions */
} fsm_t;

void fsm_init(fsm_t *fsm, const fsm_transition_t *table, size_t count,
              uint8_t initial, void *ctx);

/** @return false if the event is not valid in the current state (ignored). */
bool fsm_dispatch(fsm_t *fsm, uint8_t event);

#endif /* FSM_H */
