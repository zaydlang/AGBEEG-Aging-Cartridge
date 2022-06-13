#ifndef MENU_H
#define MENU_H

#include "state.h"

void state_init_menu();
void state_run_menu();

static const State STATE_MENU = {
    state_init_menu,
    state_run_menu
};

#endif