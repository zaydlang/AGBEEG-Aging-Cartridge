#include <stdint.h>
#include <stdio.h>
#include <tonc.h>

#include "../../font.h"
#include "../tests.h"

__attribute__((__used__))
static uint32_t dma_source = 0xCAFEBABE;

__attribute((section(".ewram"))) 
static uint32_t test_result;
__attribute((section(".ewram"))) 
static char test_result_string[10];

__attribute__((naked)) 
__attribute__((target("arm"))) 
__attribute__((section(".iwram")))
int run_stm_test() {\
__asm__(\
    "push {r4 - r8}\n"\
\
    "ldr r0, =#0x040000D4\n"\
    "ldr r2, =#0xDEADBEEF\n"\
\
    "ldr r1, =dma_source\n"\
    "str r1, [r0], #4\n"\
\
    "ldr r3, =test_result\n"\
    "str r3, [r0], #4\n"\
    "sub r3, #4\n"\
\
    "ldr r1, =#0x84000001\n"\
    "str r1, [r0]\n"\
\
    "stm r3, {r1, r2}\n"\
    "add r3, #4\n"\
\
    "ldr r0, [r3]\n"\
    "pop {r4 - r8}\n"\
    "bx lr\n"\
);
}

TestResult test_cpu_stm_doesnt_lock_bus() {
    uint32_t result = run_stm_test();
    return result == 0xDEADBEEF ? PASS : FAIL;
}

void test_cpu_stm_doesnt_lock_bus_info_init() {
    sprintf(test_result_string, "%lX", test_result);

    ags_print("EXPECTED VALUE IN", 6, 6, 3);
    ags_print("TEST_RESULT:", 4, 7, 3);
    ags_print("DEADBEEF", 17, 7, 5);

    ags_print("ACTUAL VALUE IN", 6, 10, 3);
    ags_print("TEST_RESULT:", 4, 11, 3);

    ags_print(test_result_string, 17, 11, test_result == 0xDEADBEEF ? 5 : 4);
}

void test_cpu_stm_doesnt_lock_bus_info_run() {

}