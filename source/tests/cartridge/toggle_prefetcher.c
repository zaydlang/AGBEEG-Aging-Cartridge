#include <tonc.h>

#include "../../font.h"
#include "../tests.h"
#include "../../util.h"

#define IDLES_1 \
    __asm__( \
        "mul r3, r8, r8 \n" \
    );

#define IDLES_2 \
    __asm__( \
        "mul r3, r8, r9 \n" \
    );

#define IDLES_3 \
    __asm__( \
        "mul r3, r8, r10 \n" \
    );

#define IDLES_4 \
    __asm__( \
        "mul r3, r8, r11 \n" \
    );

#define IDLES_5 \
    __asm__( \
        "mul r3, r8, r11 \n" \
        "mul r3, r8, r8 \n" \
    );

#define IDLES_6 \
    __asm__( \
        "mul r3, r8, r11 \n" \
        "mul r3, r8, r9 \n" \
    );

#define IDLES_7 \
    __asm__( \
        "mul r3, r8, r11 \n" \
        "mul r3, r8, r10 \n" \
    );

#define IDLES_8 \
    __asm__( \
        "mul r3, r8, r11 \n" \
        "mul r3, r8, r11 \n" \
    );

#define SETUP(idles, modify) \
\
    /* push callee saved registers */ \
\
    __asm__( \
        "push {r4-r11}" \
    ); \
\
    /* r8 - r11 will be various registers that can be used */ \
    /* with the mul instruction to manipulate the amount of */ \
    /* idle cycles the precede the test. this puts the prefetch */ \
    /* buffer in a different state during each test. theoretically */ \
    /* this should cause timing differences. r7 will contain an */ \
    /* address to ROM */ \
\
    __asm__( \
        "ldr r7, =0x04000204 \n" \
        "ldr r6, [r7]\n" \
    );\
\
    modify \
\
    __asm__(\
        "ldr r8,  =0x000000AA \n" \
        "ldr r9,  =0x0000AAAA \n" \
        "ldr r10, =0x00AAAAAA \n" \
        "ldr r11, =0xAAAAAAAA" \
    ); \
\
    /* now we start the timer */ \
\
    __asm__( \
        "ldr r0, =0x4000100 \n" \
        "ldr r1, =0x800000 \n" \
        "str r1, [r0]" \
    ); \
\
    /* manipulate the prefetch buffer a bunch */ \
\
    idles \

#define TEARDOWN \
    /* grab the timers value and stop it */ \
\
    __asm__( \
        "ldrh r3, [r0] \n" \
        "ldr r2, =#0x0 \n" \
        "str r2, [r0] \n" \
        "mov r0, r3" \
    ); \
\
    /* time to return */ \
\
    __asm__( \
        "pop {r4-r11} \n" \
        "bx lr" \
    ); \

#define CREATE_ENABLE_CALIBRATION(name, idles) \
    __attribute__((naked)) __attribute__((target("arm"))) static int name() {\
\
        SETUP(idles, __asm__("orr r6, #0x4000");) \
        /* run the test */ \
\
        TEARDOWN \
    }

#define CREATE_DISABLE_CALIBRATION(name, idles) \
    __attribute__((naked)) __attribute__((target("arm"))) static int name() {\
\
        SETUP(idles, __asm__("bic r6, #0x4000");) \
        /* run the test */ \
\
        TEARDOWN \
    }

#define CREATE_ENABLE_TEST(name, idles) \
    __attribute__((naked)) __attribute__((target("arm"))) static int name() {\
\
        SETUP(idles, __asm__("orr r6, #0x4000");) \
        /* run the test */ \
\
        __asm__( \
            "str r6, [r7]\n" \
            "nop\n nop\n nop\n nop\n nop\n"\
        ); \
\
        TEARDOWN \
    }

#define CREATE_DISABLE_TEST(name, idles) \
    __attribute__((naked)) __attribute__((target("arm"))) static int name() {\
\
        SETUP(idles, __asm__("bic r6, #0x4000");) \
        /* run the test */ \
\
        __asm__( \
            "str r6, [r7]\n" \
            "nop\n nop\n nop\n nop\n nop\n"\
        ); \
\
        TEARDOWN \
    }

CREATE_DISABLE_CALIBRATION(disable_calibration1, IDLES_1)
CREATE_DISABLE_CALIBRATION(disable_calibration2, IDLES_2)
CREATE_DISABLE_CALIBRATION(disable_calibration3, IDLES_3)
CREATE_DISABLE_CALIBRATION(disable_calibration4, IDLES_4)
CREATE_ENABLE_CALIBRATION(enable_calibration1, IDLES_1)
CREATE_ENABLE_CALIBRATION(enable_calibration2, IDLES_2)
CREATE_ENABLE_CALIBRATION(enable_calibration3, IDLES_3)
CREATE_ENABLE_CALIBRATION(enable_calibration4, IDLES_4)
CREATE_DISABLE_TEST(disable_test1, IDLES_1)
CREATE_DISABLE_TEST(disable_test2, IDLES_2)
CREATE_DISABLE_TEST(disable_test3, IDLES_3)
CREATE_DISABLE_TEST(disable_test4, IDLES_4)
CREATE_ENABLE_TEST(enable_test1, IDLES_1)
CREATE_ENABLE_TEST(enable_test2, IDLES_2)
CREATE_ENABLE_TEST(enable_test3, IDLES_3)
CREATE_ENABLE_TEST(enable_test4, IDLES_4)

#define NUM_TESTS 4
static int (*disable_calibration[4])()  = {disable_calibration1, disable_calibration2, disable_calibration3, disable_calibration4};
static int (*enable_calibration[4])()  = {enable_calibration1, enable_calibration2, enable_calibration3, enable_calibration4};
static int (*disable_tests[4])() = {disable_test1, disable_test2, disable_test3, disable_test4};
static int (*enable_tests[4])() = {enable_test1, enable_test2, enable_test3, enable_test4};

static int disable_waitstates[4] = {0x4000, 0x4004, 0x4010, 0x4014};
static int enable_waitstates[4] = {0x0000, 0x0004, 0x0010, 0x0014};

__attribute__((section(".iwram"))) static unsigned short disable_results[32];
__attribute__((section(".iwram"))) static unsigned short enable_results[32];

static unsigned short disable_expected[16] = {
    0x26, 0x25, 0x1B, 0x1A,
    0x26, 0x25, 0x1B, 0x1A,
    0x26, 0x25, 0x1B, 0x1A,
    0x26, 0x25, 0x1A, 0x19,
};

static unsigned short enable_expected[16] = {
    0x27, 0x26, 0x1C, 0x1B,
    0x27, 0x26, 0x1C, 0x1B,
    0x27, 0x26, 0x1C, 0x1B,
    0x27, 0x26, 0x1C, 0x1B,
};

static bool do_test(int (*tests[4])(), int (*calibration[4])(), unsigned short expected[16], unsigned short* results, int waitstates[4]) {
    bool all_correct = true;
    for (int i = 0; i < NUM_TESTS; i++) {
        for (int w = 0; w < 4; w++) {
            int waitstate = waitstates[w];

            // GBA hangs if you assign REG_WAITCNT
            // too frequently, hence the waitloops

            REG_WAITCNT = waitstate;
            waitloop(100);

            unsigned short calib = calibration[i]();
            waitloop(100);

            REG_WAITCNT = waitstate;
            waitloop(100);

            unsigned short result = tests[i]() - calib;
            waitloop(100);

            all_correct &= result == expected[i * 4 + w];
            results[i * 4 + w] = result;
        }
    }

    return all_correct;
}

TestResult test_toggle_prefetcher() {
    // the fact that && is short-circuiting, even with volatile keywords, has got to be some of the coolest yet densest stuff i've seen :|
    // so i have to do this stupid dance instead of just doing all_correct = do_test1 && do_test2;
    volatile bool all_correct = true;
    all_correct &= do_test(disable_tests, disable_calibration, disable_expected, disable_results, disable_waitstates);
    all_correct &= do_test(enable_tests, enable_calibration, enable_expected, enable_results, enable_waitstates);

    return all_correct ? PASS : FAIL;
}

static void display_test_results(char* name, unsigned short* expected, unsigned short* results) {
    ags_print("WAIT:     .. N. .S NS", 4, 2, 3);
    ags_print(name, 12, 18, 6);
    ags_print("EXPECTED:", 2, 4, 3);
    
    for (int i = 0; i < 4; i++) {
        ags_printf(
            "%d IDLES:  %02X %02X %02X %02X", 
            4, 5 + i, 3, i,
            expected[4 * i + 0],
            expected[4 * i + 1],
            expected[4 * i + 2],
            expected[4 * i + 3]
        );
    }
    
    ags_print("ACTUAL:", 2, 11, 3);
    for (int i = 0; i < 4; i++) {
        ags_printf("%d IDLES:", 4, 12 + i, 3, i);

        for (int w = 0; w < 4; w++) {
            int color = results[i * 4 + w] == expected[i * 4 + w] ? 5 : 4;
            ags_printf("%02X", 14 + w * 3, 12 + i, color, results[i * 4 + w] & 0xFF);
        }
    }
}

typedef enum DisplayedTest {
    DISABLE = 0,
    ENABLE = 1
} DisplayedTest;

static DisplayedTest displayed_test;

void test_toggle_prefetcher_info_init() {
    displayed_test = DISABLE;
}

void test_toggle_prefetcher_info_run() { 
    if (key_hit(KEY_A)) {
        displayed_test ^= 1;
    }

    switch (displayed_test) {
        case DISABLE:
            display_test_results("DISABLE", disable_expected, disable_results);
            break;
        case ENABLE:
            display_test_results("ENABLE ", enable_expected, enable_results);
            break;
    }
}
