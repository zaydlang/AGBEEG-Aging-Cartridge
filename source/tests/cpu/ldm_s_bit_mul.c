#include <stdint.h>
#include <stdio.h>
#include <tonc.h>

#include "../../font.h"
#include "../tests.h"

__attribute__((__used__))
static uint32_t ldm_source[2] = { 0x0702, 0x0803 };

__attribute((section(".ewram"))) 
static uint32_t test_result[2];

__attribute__((naked)) 
__attribute__((target("arm"))) 
__attribute__((section(".iwram")))
int run_mul_test1() {\
__asm__(\
    "push {r4 - r9}\n"\
\
    "mrs r0, cpsr\n"\
    "bic r0, #0x1f\n"\
    "orr r0, #0x11\n"\
    "msr cpsr, r0\n"\
\
    "ldr r8, =#0x0904\n"\
    "ldr r9, =#0x0A05\n"\
\
    "ldr r1, =ldm_source\n"\
    "ldm r1, {r8, r9}^\n"\
    "mul r1, r9, r8\n"\
\
    "mrs r0, cpsr\n"\
    "bic r0, #0x1f\n"\
    "orr r0, #0x10\n"\
    "msr cpsr, r0\n"\
\
    "pop {r4 - r9}\n"\
    "mov r0, r1\n"\
    "bx lr\n"\
);
}

__attribute__((naked)) 
__attribute__((target("arm"))) 
__attribute__((section(".iwram")))
int run_mul_test2() {\
__asm__(\
    "push {r4 - r9}\n"\
\
    "mrs r0, cpsr\n"\
    "bic r0, #0x1f\n"\
    "orr r0, #0x11\n"\
    "msr cpsr, r0\n"\
\
    "ldr r8, =#0x0904\n"\
    "ldr r9, =#0x0A05\n"\
\
    "ldr r1, =ldm_source\n"\
    "ldm r1, {r8, r9}^\n"\
    "mul r1, r8, r9\n"\
\
    "mrs r0, cpsr\n"\
    "bic r0, #0x1f\n"\
    "orr r0, #0x10\n"\
    "msr cpsr, r0\n"\
\
    "pop {r4 - r9}\n"\
    "mov r0, r1\n"\
    "bx lr\n"\
);
}

#define TEST_RESULT_1 0x96871E
#define TEST_RESULT_2 0x382506

TestResult test_cpu_ldm_s_bit_mul() {
    test_result[0] = run_mul_test1();
    test_result[1] = run_mul_test2();
    return test_result[0] == TEST_RESULT_1 && test_result[1] == TEST_RESULT_2 ? PASS : FAIL;
}

void test_cpu_ldm_s_bit_mul_init() {
    ags_print("EXPECTED VALUE IN", 6, 5, 3);
    ags_printf("TEST_RESULT[0]:", 3, 7, VIOLET);
    ags_printf("%08X", 19, 7, GREEN, TEST_RESULT_1);
    ags_printf("TEST_RESULT[1]:", 3, 8, VIOLET);
    ags_printf("%08X", 19, 8, GREEN, TEST_RESULT_2);

    ags_print("ACTUAL VALUE IN", 6, 10, 3);
    ags_printf("TEST_RESULT[0]:", 3, 12, VIOLET);
    ags_printf("%08X", 19, 12, test_result[0] == TEST_RESULT_1 ? GREEN : RED, test_result[0]);
    ags_printf("TEST_RESULT[1]:", 3, 13, VIOLET);
    ags_printf("%08X", 19, 13, test_result[1] == TEST_RESULT_2 ? GREEN : RED, test_result[1]);
}

void test_cpu_ldm_s_bit_mul_run() {

}