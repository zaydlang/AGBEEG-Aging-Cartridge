#include <tonc.h>
#include <string.h>
#include <maxmod.h>

#include "ci.h"
#include "font.h"
#include "menu.h"
#include "tests/tests.h"
#include "video.h"

#include "soundbank.h"
#include "soundbank_bin.h"

enum pride_state {
    PRIDE_NONE, 
    PRIDE_WAITING,
    PRIDE_HOLD,
    PRIDE_FADE_IN,
    PRIDE_SWIPE_RIGHT,
    PRIDE_SWIPE_LEFT,
    PRIDE_SWIPE_UP,
    PRIDE_SWIPE_DOWN,
};

typedef struct {
    uint16_t color;
    uint16_t line;
} FlagColorChange;

static int g_cursor_x = 0;
static int g_cursor_y = 0;

static int all_tests_pass;
static int display_status_text;
static int frames_till_toggle_status_text;
static int frames_till_music_start;
static int frames_till_pride_start;
static int pride_state;
static int pride_timer;
static FlagColorChange* pride_active_flag;
static int pride_active_flag_index;
static int pride_active_flag_line;
static bool started_sound = false;

// Flags are specified in {color, scanline}
static const FlagColorChange rainbow_flag[] = {
    {RGB_TO_15(0xE50000), 0},
    {RGB_TO_15(0xFF8D00), 26},
    {RGB_TO_15(0xFFEE00), 53},
    {RGB_TO_15(0x028121), 80},
    {RGB_TO_15(0x004CFF), 106},
    {RGB_TO_15(0x770088), 133},
    {0, 0}
};

static const FlagColorChange ace_flag[] = {
    {RGB_TO_15(0x000000), 0},
    {RGB_TO_15(0xA4A4A4), 40},
    {RGB_TO_15(0xFFFFFF), 80},
    {RGB_TO_15(0x810081), 120},
    {0, 0}
};

static const FlagColorChange bi_flag[] = {
    {RGB_TO_15(0xD60270), 0},
    {RGB_TO_15(0x9B4F96), 64},
    {RGB_TO_15(0x0038A8), 96},
    {0, 0}
};

static const FlagColorChange trans_flag[] = {
    {RGB_TO_15(0x55CDFC), 0},
    {RGB_TO_15(0xF7A8B8), 32},
    {RGB_TO_15(0xFFFFFF), 64},
    {RGB_TO_15(0xF7A8B8), 96},
    {RGB_TO_15(0x55CDFC), 128},
    {0, 0}
};

static const FlagColorChange enby_flag[] = {
    {RGB_TO_15(0xFCF431), 0},
    {RGB_TO_15(0xFCFCFC), 40},
    {RGB_TO_15(0x9D59D2), 80},
    {RGB_TO_15(0x282828), 120},
    {0, 0}
};


static const FlagColorChange pan_flag[] = {
    {RGB_TO_15(0xFF1C8D), 0},
    {RGB_TO_15(0xFFD700), 53},
    {RGB_TO_15(0x1AB3FF), 107},
    {0, 0}
};

#define PRIDE_FLAG_COUNT 6
static FlagColorChange* pride_flags[] = {
    rainbow_flag,
    ace_flag,
    bi_flag,
    trans_flag,
    enby_flag,
    pan_flag
};

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


void hBlank() {
    pal_bg_mem[0] = pride_active_flag[pride_active_flag_line].color;
    uint16_t next_change_line = pride_active_flag[pride_active_flag_line + 1].line;
    if (next_change_line == REG_VCOUNT) {
        pride_active_flag_line++;
    }
}

void state_init_menu() {
    init_print(0, 28, 3);
    ags_print("AGBEEG AGING CARTRIDGE", 1, 0, 2);
    ags_print("CARTRIDGE...", 2, 2, 3);
    ags_print("CPU.........", 2, 4, 3);
    ags_print("DMA.........", 2, 6, 3);

    do_and_draw_test_results();

    update_cursor_pos();
    memcpy(tile_mem_obj[0], CURSOR_TEXTURE, sizeof(CURSOR_TEXTURE));
    frames_till_toggle_status_text = 32;

    started_sound = false;
}

void state_run_menu() {
    if (all_tests_pass) {
        if (!started_sound) {
            started_sound = true;

            mm_sound_effect congratulations = {
                { SFX_CONGRATULATIONS } ,			// id
                (int)(1.0f * (1<<10)),	// rate
                0,		// handle
                255,	// volume
                127,	// panning
            };

            frames_till_music_start = 136;

            mmEffectEx(&congratulations);
        }

        if (frames_till_music_start == 0) {
            pride_state = PRIDE_WAITING;
            pride_timer = 609 - 60;
            mmStart( MOD_MUSIC, MM_PLAY_LOOP );
        }

        if (frames_till_music_start > -1) {
            frames_till_music_start--;
        }

        if (pride_state != PRIDE_NONE) {
            pride_timer--;
            switch (pride_state) {
                case PRIDE_WAITING:
                    if (pride_timer == 0) {
                        pride_timer = 60;    

                        pride_state = PRIDE_FADE_IN;
                    }
                    break;
                case PRIDE_FADE_IN: 
                    REG_BLDCNT = BLD_BUILD(BLD_ALL, 0, 2);
                    REG_BLDY = 31 - (pride_timer / 2);
                    if (pride_timer == 0) {
                        pride_timer = 60;
                        pride_state = PRIDE_HOLD;

                        REG_BLDCNT = BLD_BUILD(BLD_BG2, BLD_BG3 | BLD_BACKDROP, 1);
                        REG_BLDALPHA = BLDA_BUILD(8,8);

                        pride_active_flag = rainbow_flag;
                        pride_active_flag_line = 0;
	                    irq_add(II_HBLANK, hBlank);
                    }
                    break;
                case PRIDE_HOLD:
                    if (pride_timer == 0) {
                        pride_timer = 60;

                        pride_active_flag_index++;
                        pride_active_flag_index %= PRIDE_FLAG_COUNT;

                        pride_active_flag = pride_flags[pride_active_flag_index];
                    }
                        pride_active_flag_line = 0;

                    pal_bg_mem[0] = pride_active_flag[0].color;
                    break;
            }
        }
    }

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
        REG_BLDCNT = 0;
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