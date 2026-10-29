#include "fsm.h"

void fsm_init(fsm_t *fsm, const fsm_transition_t *table, size_t count,
              uint8_t initial, void *ctx)
{
    fsm->table = table;
    fsm->count = count;
    fsm->state = initial;
    fsm->ctx = ctx;
}

bool fsm_dispatch(fsm_t *fsm, uint8_t event)
{
    bool handled = false;

    for (size_t i = 0U; (i < fsm->count) && !handled; i++) {
        const fsm_transition_t *t = &fsm->table[i];

        if ((t->state == fsm->state) && (t->event == event)) {
            if (t->action != NULL) {
                t->action(fsm->ctx);
            }
            fsm->state = t->next;
            handled = true;
        }
    }
    return handled;
}
