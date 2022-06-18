#include <tonc.h>
#include <string.h>

#include "ci.h"
#include "font.h"
#include "menu.h"
#include "tests/tests.h"
#include "video.h"

static int g_cursor_x = 0;
static int g_cursor_y = 0;

static int all_tests_pass;
static int display_status_text;
static int frames_till_toggle_status_text;

static void update_cursor_pos() {
    oam_mem[0].attr0 = (g_cursor_y * 2 + 2) * 8;
    oam_mem[0].attr1 = (g_cursor_x + 14) * 8;
    oam_mem[0].attr2 = 0;
}

static void do_and_draw_test_results() {
    int draw_y = 2;

    all_tests_pass = 1;
    for (int i = 0; i < NUM_TEST_CATEGORIES; i++) {
        int draw_x = 14;
        int all_tests_in_category_pass = 1;

        TestCategory test_category = test_categories[i];
        for (int j = 0; j < MAX_TESTS_PER_CATEGORY && test_category.tests[j].run_test != NULL; j++) {
            Test test = test_category.tests[j];
            TestResult test_result = test.run_test();

            char* ci_result = test_result == PASS ? "PASS" : "FAIL";
            ci_send(test.name, strlen(test.name));
            ci_send(ci_result, 4);

            switch (test_result) {
                case PASS:
                    ags_print("O", draw_x, draw_y, 5);
                    break;
                case FAIL:
                    ags_print("X", draw_x, draw_y, 4);
                    all_tests_in_category_pass = 0;
                    all_tests_pass = 0;
                    break;
            }

            draw_x += 1;
        }

        ags_print(">", 23, draw_y, 6);
        if (all_tests_in_category_pass) {
            ags_print("PASS", 24, draw_y, 5);
        } else {
            ags_print("FAIL", 24, draw_y, 4);
        }

        draw_y += 2;
    }

    ci_send("DONE", 4);
    display_status_text = 1;
}

void state_init_menu() {
    REG_IME = 0;
    REG_IE  = 1;

    init_print(0, 28, 3);
    ags_print("AGBEEG AGING CARTRIDGE", 1, 0, 2);
    ags_print("CARTRIDGE...", 2, 2, 3);
    ags_print("CPU.........", 2, 4, 3);
    ags_print("DMA.........", 2, 6, 3);

    do_and_draw_test_results();

    update_cursor_pos();
    memcpy(tile_mem_obj[0], CURSOR_TEXTURE, sizeof(CURSOR_TEXTURE));
    frames_till_toggle_status_text = 32;
}

void state_run_menu() {
    frames_till_toggle_status_text--;

    if (frames_till_toggle_status_text == 0) {
        frames_till_toggle_status_text = 32;
        display_status_text ^= 1;
    }

    if (display_status_text) {
        if (all_tests_pass) {
            ags_print("CONGRATULATIONS!!", 7, 17, 4);
        } else {
            ags_print("ERROR OCCURED!!", 8, 17, 4);
        }
    } else {
        ags_erase(7, 17, 24);
    }

    if (key_hit(KEY_A)) {
        switch_state(TEST_INFO, &test_categories[g_cursor_y].tests[g_cursor_x]);
        return;
    }
    
    if (key_hit(KEY_RIGHT)) g_cursor_x += 1;
    if (key_hit(KEY_LEFT )) g_cursor_x -= 1;
    if (key_hit(KEY_DOWN )) g_cursor_y += 1;
    if (key_hit(KEY_UP   )) g_cursor_y -= 1;

    if (g_cursor_x > test_categories[g_cursor_y].num_tests - 1) {
        g_cursor_x = test_categories[g_cursor_y].num_tests - 1;
    }

    if (g_cursor_x < 0) {
        g_cursor_x = 0;
    }

    if (g_cursor_y < 0) {
        g_cursor_y = 0;
    }

    if (g_cursor_y > NUM_TEST_CATEGORIES - 1) {
        g_cursor_y = NUM_TEST_CATEGORIES - 1;
    }

    update_cursor_pos();
}