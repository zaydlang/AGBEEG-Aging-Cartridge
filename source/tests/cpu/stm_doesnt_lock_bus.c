#include <stdint.h>
#include <stdio.h>
#include <tonc.h>

#include "../../font.h"
#include "../tests.h"

__attribute__((__used__))
static uint32_t dma_source = 0xCAFEBABE;

__attribute((section(".ewram"))) 
static uint32_t test_result[2];
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
\
    "ldr r1, =#0xDEDEDEDE\n"\
    "ldr r2, =#0xBABABABA\n"\
    "ldr r4, =test_result\n"\
\
    "ldr r3, =#0x85000002\n"\
    "str r3, [r0]\n"\
\
    "stm r4, {r1, r2}\n"\
\
    "pop {r4 - r8}\n"\
    "bx lr\n"\
);
}

TestResult test_cpu_stm_doesnt_lock_bus() {
    uint32_t result = run_stm_test();
    return test_result[0] == 0xCAFEBABE && test_result[1] == 0xBABABABA ? PASS : FAIL;
}

void test_cpu_stm_doesnt_lock_bus_info_init() {
    sprintf(test_result_string, "%lX", test_result);

    ags_print("EXPECTED VALUE IN", 6, 6, 3);
    ags_print("TEST_RESULT[0]:", 3, 7, 3);
    ags_print("CAFEBABE", 19, 7, 5);
    ags_print("TEST_RESULT[1]:", 3, 8, 3);
    ags_print("BABABABA", 19, 8, 5);

    ags_print("ACTUAL VALUE IN", 6, 11, 3);
    ags_print("TEST_RESULT[0]:", 3, 12, 3);
    ags_print("TEST_RESULT[1]:", 3, 13, 3);

    ags_printf("%08X", 19, 12, test_result[0] == 0xCAFEBABE ? 5 : 4, test_result[0]);
    ags_printf("%08X", 19, 13, test_result[1] == 0xBABABABA ? 5 : 4, test_result[1]);
}

void test_cpu_stm_doesnt_lock_bus_info_run() {

}