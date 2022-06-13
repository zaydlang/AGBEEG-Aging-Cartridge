#include <tonc.h>

#include "tests/tests.h"
#include "state.h"
#include "menu.h"

int main() {
    switch_state(STATE_MENU, NULL);

    while (1) {
        Halt();
        REG_IF = 0xFFFF;

        key_poll();

        state_run();
    }
}