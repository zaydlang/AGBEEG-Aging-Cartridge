#include "state.h"
#include "video.h"

static State g_current_state;

void switch_state(State state, void* args) {
    g_current_state = state;
    
    reset_display();

    g_current_state.state_init(args);
}

void state_run() {
    g_current_state.state_run();
}