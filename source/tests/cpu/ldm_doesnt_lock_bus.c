#include <stdint.h>
#include <stdio.h>
#include <tonc.h>

#include "../../font.h"
#include "../tests.h"

__attribute__((__used__))
static uint32_t dma_source = 0xCAFEBABE;

__attribute((section(".ewram"))) 
__attribute__((__used__))
static uint32_t dma_dest[2];

__attribute((section(".ewram"))) 
static uint32_t test_result[2];

__attribute__((naked)) 
__attribute__((target("arm"))) 
__attribute__((section(".iwram")))
void run_ldm_test() {\
__asm__(\
    "push {r4 - r8}\n"\
\
    "ldr r0, =dma_dest\n"\
    "ldr r1, =#0xDEADBEEF\n"\
    "str r1, [r0]\n"\
    "ldr r1, =0x00C0FFEE\n"\
    "str r1, [r0, #4]\n"\
\
    "ldr r0, =#0x040000D4\n"\
\
    "ldr r1, =dma_source\n"\
    "str r1, [r0], #4\n"\
\
    "ldr r3, =dma_dest\n"\
    "str r3, [r0], #4\n"\
\
    "ldr r1, =#0x85000002\n"\
    "str r1, [r0]\n"\
\
    "ldm r3, {r0, r1}\n"\
    "ldr r2, =test_result\n"\
    "str r0, [r2]\n"\
    "str r1, [r2, #4]\n"\
\
    "pop {r4 - r8}\n"\
    "bx lr\n"\
);
}

TestResult test_cpu_ldm_doesnt_lock_bus() {
    run_ldm_test();
    return test_result[0] == 0xDEADBEEF && test_result[1] == 0xCAFEBABE ? PASS : FAIL;
}

void test_cpu_ldm_doesnt_lock_bus_info_init() {
    ags_print("EXPECTED VALUE IN", 6, 6, 3);
    ags_print("TEST_RESULT[0]:", 3, 7, 3);
    ags_print("DEADBEEF", 19, 7, 5);
    ags_print("TEST_RESULT[1]:", 3, 8, 3);
    ags_print("CAFEBABE", 19, 8, 5);

    ags_print("ACTUAL VALUE IN", 6, 11, 3);
    ags_print("TEST_RESULT[0]:", 3, 12, 3);
    ags_print("TEST_RESULT[1]:", 3, 13, 3);

    ags_printf("%08X", 19, 12, test_result[0] == 0xDEADBEEF ? 5 : 4, test_result[0]);
    ags_printf("%08X", 19, 13, test_result[1] == 0xCAFEBABE ? 5 : 4, test_result[1]);
}

void test_cpu_ldm_doesnt_lock_bus_info_run() {

}