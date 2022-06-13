#ifndef STATE_H
#define STATE_H

typedef struct State {
    void (*state_init)(void* args);
    void (*state_run)();
} State;

void switch_state(State state, void* args);
void state_run();

#endif