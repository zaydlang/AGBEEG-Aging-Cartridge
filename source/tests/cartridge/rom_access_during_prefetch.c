#include <tonc.h>

#include "../../font.h"
#include "../tests.h"

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

#define SETUP(idles) \
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
        "ldr r7,  =0x08000000 \n" \
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

#define CREATE_CALIBRATION(name, idles) \
    __attribute__((naked)) __attribute__((target("arm"))) static int name() {\
\
        SETUP(idles) \
        /* run the test */ \
\
        TEARDOWN \
    }

#define CREATE_READ_TEST(name, idles) \
    __attribute__((naked)) __attribute__((target("arm"))) static int name() {\
\
        SETUP(idles) \
        /* run the test */ \
\
        __asm__( \
            "ldr r6, [r7]" \
        ); \
\
        TEARDOWN \
    }

#define CREATE_WRITE_TEST(name, idles) \
    __attribute__((naked)) __attribute__((target("arm"))) static int name() {\
\
        SETUP(idles) \
        /* run the test */ \
\
        __asm__( \
            "str r6, [r7]" \
        ); \
\
        TEARDOWN \
    }

CREATE_CALIBRATION(calibration1, IDLES_1)
CREATE_CALIBRATION(calibration2, IDLES_2)
CREATE_CALIBRATION(calibration3, IDLES_3)
CREATE_CALIBRATION(calibration4, IDLES_4)
CREATE_CALIBRATION(calibration5, IDLES_5)
CREATE_CALIBRATION(calibration6, IDLES_6)
CREATE_CALIBRATION(calibration7, IDLES_7)
CREATE_CALIBRATION(calibration8, IDLES_8)
CREATE_READ_TEST(read_test1, IDLES_1)
CREATE_READ_TEST(read_test2, IDLES_2)
CREATE_READ_TEST(read_test3, IDLES_3)
CREATE_READ_TEST(read_test4, IDLES_4)
CREATE_READ_TEST(read_test5, IDLES_5)
CREATE_READ_TEST(read_test6, IDLES_6)
CREATE_READ_TEST(read_test7, IDLES_7)
CREATE_READ_TEST(read_test8, IDLES_8)
CREATE_WRITE_TEST(write_test1, IDLES_1)
CREATE_WRITE_TEST(write_test2, IDLES_2)
CREATE_WRITE_TEST(write_test3, IDLES_3)
CREATE_WRITE_TEST(write_test4, IDLES_4)
CREATE_WRITE_TEST(write_test5, IDLES_5)
CREATE_WRITE_TEST(write_test6, IDLES_6)
CREATE_WRITE_TEST(write_test7, IDLES_7)
CREATE_WRITE_TEST(write_test8, IDLES_8)

#define NUM_TESTS 8
static int (*calibration[8])()  = {calibration1, calibration2, calibration3, calibration4, calibration5, calibration6, calibration7, calibration8};
static int (*read_tests[8])() = {read_test1, read_test2, read_test3, read_test4, read_test5, read_test6, read_test7, read_test8};
static int (*write_tests[8])() = {write_test1, write_test2, write_test3, write_test4, write_test5, write_test6, write_test7, write_test8};

static int waitstates[4] = {0x4000, 0x4004, 0x4010, 0x4014};

__attribute__((section(".iwram"))) static unsigned short read_results[32];
__attribute__((section(".iwram"))) static unsigned short write_results[32];

static unsigned short read_expected[32] = {
    0x11, 0x0f, 0x0f, 0x0d,
    0x11, 0x0f, 0x0f, 0x0d,
    0x11, 0x0f, 0x0f, 0x0d,
    0x11, 0x0f, 0x10, 0x0e,
    0x11, 0x0f, 0x0f, 0x0d,
    0x11, 0x0f, 0x0f, 0x0d,
    0x11, 0x0f, 0x10, 0x0e,
    0x11, 0x0f, 0x0f, 0x0d,
};

static unsigned short write_expected[32] = {
    0x10, 0x0e, 0x0e, 0x0c,
    0x10, 0x0e, 0x0e, 0x0c,
    0x10, 0x0e, 0x0e, 0x0c,
    0x10, 0x0e, 0x0f, 0x0d,
    0x10, 0x0e, 0x0e, 0x0c,
    0x10, 0x0e, 0x0e, 0x0c,
    0x10, 0x0e, 0x0f, 0x0d,
    0x10, 0x0e, 0x0e, 0x0c,
};

static bool do_read_test() {
    bool all_correct = true;
    for (int i = 0; i < NUM_TESTS; i++) {
        for (int w = 0; w < 4; w++) {
            int waitstate = waitstates[w];
            REG_WAITCNT = waitstate;
            unsigned short result = read_tests[i]() - calibration[i]();
            all_correct &= result == read_expected[i * 4 + w];
            read_results[i * 4 + w] = result;
        }
    }

    return all_correct;
}

static bool do_write_test() {
    bool all_correct = true;
    for (int i = 0; i < NUM_TESTS; i++) {
        for (int w = 0; w < 4; w++) {
            int waitstate = waitstates[w];
            REG_WAITCNT = waitstate;
            unsigned short result = write_tests[i]() - calibration[i]();
            all_correct &= result == write_expected[i * 4 + w];
            write_results[i * 4 + w] = result;
        }
    }

    return all_correct;
}

TestResult test_rom_access_during_prefetch() {
    bool all_correct = do_read_test() && do_write_test();
    return all_correct ? PASS : FAIL;
    return PASS;
}

static void display_test_results(char* name, unsigned short* expected, unsigned short* results) {
    ags_print("WAIT:     .. N. .S NS", 2, 1, 3);
    ags_print(name, 24, 1, 6);
    ags_print("EXPECTED:", 0, 2, 3);
    
    for (int i = 0; i < 8; i++) {
        ags_printf(
            "%d IDLES:  %02X %02X %02X %02X", 
            2, 3 + i, 3, i,
            expected[4 * i + 0],
            expected[4 * i + 1],
            expected[4 * i + 2],
            expected[4 * i + 3]
        );
    }
    
    ags_print("ACTUAL:", 0, 11, 3);
    for (int i = 0; i < 8; i++) {
        ags_printf("%d IDLES:", 2, 12 + i, 3, i);

        for (int w = 0; w < 4; w++) {
            int color = results[i * 4 + w] == expected[i * 4 + w] ? 5 : 4;
            ags_printf("%02X", 12 + w * 3, 12 + i, color, results[i * 4 + w]);
        }
    }
}

typedef enum DisplayedTest {
    READ = 0,
    WRITE = 1
} DisplayedTest;

static DisplayedTest displayed_test;

void test_rom_access_during_prefetch_info_init() {
    displayed_test = READ;
}

void test_rom_access_during_prefetch_info_run() {
    if (key_hit(KEY_A)) {
        displayed_test ^= 1;
    }

    switch (displayed_test) {
        case READ:
            display_test_results("READ ", read_expected, read_results);
            break;
        case WRITE:
            display_test_results("WRITE", write_expected, write_results);
            break;
    }
}