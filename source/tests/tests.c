#include <tonc.h>

#include "../font.h"
#include "../menu.h"
#include "../state.h"
#include "tests.h"
#include "../video.h"

__attribute__((section(".iwram")))
static Test* g_current_test;

void state_test_info_init(void* args) {
    g_current_test = (Test*) args;

    init_print(0, 28, 3);
    ags_print(g_current_test->name, 1, 0, 2);

    g_current_test->test_info_init();
}

void state_test_info_run() {
    if (key_hit(KEY_B)) {
        switch_state(STATE_MENU, NULL);
        return;
    }

    g_current_test->test_info_run();
}