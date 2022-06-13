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

#define SETUP \
\
    /* push callee saved registers */ \
\
    __asm__( \
        "push {r4-r11}" \
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
    /* r8 - r11 will be various registers that can be used */ \
    /* with the mul instruction to manipulate the amount of */ \
    /* idle cycles the precede the test. this puts the prefetch */ \
    /* buffer in a different state during each test. theoretically */ \
    /* this should cause timing differences. */ \
\
    __asm__( \
        "ldr r8,  =0x000000AA \n" \
        "ldr r9,  =0x0000AAAA \n" \
        "ldr r10, =0x00AAAAAA \n" \
        "ldr r11, =0xAAAAAAAA" \
    ); \
\
    /* now we start DMA */ \
    __asm__(\
        "ldr r7,  =#0x040000D4 \n" \
        "ldr r4,  =#0x00000000 \n" \
        "ldr r5,  =#0x00000000 \n" \
        "ldr r6,  =#0x80000004 \n" \
        "stmia r7!, {r4 - r6} \n" \
    ); \
\

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
        SETUP \
        TEARDOWN \
    }

#define CREATE_TEST(name, idles) \
    __attribute__((naked)) __attribute__((target("arm"))) static int name() {\
\
        SETUP \
\
        idles \
\
        TEARDOWN \
    }

CREATE_CALIBRATION(calibration1, IDLES_1)
CREATE_CALIBRATION(calibration2, IDLES_2)
CREATE_CALIBRATION(calibration3, IDLES_3)
CREATE_CALIBRATION(calibration4, IDLES_4)
CREATE_TEST(test1, IDLES_1)
CREATE_TEST(test2, IDLES_2)
CREATE_TEST(test3, IDLES_3)
CREATE_TEST(test4, IDLES_4)

static int (*calibration[4])()  = {calibration1, calibration2, calibration3, calibration4};
static int (*tests[4])() = {test1, test2, test3, test4};

static int waitstates[4] = {0x4000, 0x4004, 0x4010, 0x4014};

__attribute__((section(".iwram"))) static int results[16];

unsigned short expected[16] = {
    0x01, 0x01, 0x0c, 0x0c,
    0x01, 0x01, 0x0c, 0x0c,
    0x01, 0x01, 0x0c, 0x0c,
    0x01, 0x01, 0x0c, 0x0c,
};

bool do_test() {
    bool all_correct = true;

    for (int i = 0; i < 4; i++) {
        for (int w = 0; w < 4; w++) {
            REG_WAITCNT = waitstates[w];
            unsigned short result = tests[i]() - calibration[i]();
            all_correct &= result == expected[i * 4 + w];
            results[i * 4 + w] = result;
        }
    }

    return all_correct;
}

TestResult test_cpu_runs_idles_throughout_dma() {
    bool all_correct = do_test();
    return all_correct ? PASS : FAIL;
    return PASS;
}

void test_cpu_runs_idles_throughout_dma_info_init() {
    ags_print("WAIT:     .. N. .S NS", 4, 2, 3);
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
            ags_printf("%02X", 14 + w * 3, 12 + i, color, results[i * 4 + w]);
        }
    }
}

void test_cpu_runs_idles_throughout_dma_info_run() {
    
}