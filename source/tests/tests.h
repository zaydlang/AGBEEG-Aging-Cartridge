#ifndef TEST_H
#define TEST_H

#include <stddef.h>

#include "../state.h"

#define NUM_TEST_CATEGORIES    3
#define MAX_TESTS_PER_CATEGORY 8

typedef enum TestResult {
    PASS,
    FAIL
} TestResult;

typedef struct Test {
    char* name;
    TestResult (*run_test)();
    void (*test_info_init)();
    void (*test_info_run)();
} Test;

typedef struct TestCategory {
    Test tests[MAX_TESTS_PER_CATEGORY];
    int num_tests;
} TestCategory;

TestResult test_rom_access_during_prefetch();
void test_rom_access_during_prefetch_info_init();
void test_rom_access_during_prefetch_info_run();

TestResult test_toggle_prefetcher();
void test_toggle_prefetcher_info_init();
void test_toggle_prefetcher_info_run();

TestResult test_cpu_swp_locks_bus();
void test_cpu_swp_locks_bus_info_init();
void test_cpu_swp_locks_bus_info_run();

TestResult test_cpu_runs_idles_throughout_dma();
void test_cpu_runs_idles_throughout_dma_info_init();
void test_cpu_runs_idles_throughout_dma_info_run();

void state_test_info_init(void* args);
void state_test_info_run();

static const TestCategory test_category_cartridge = {
    {
        { "ROM ACCESS DURING PREFETCH", test_rom_access_during_prefetch, test_rom_access_during_prefetch_info_init, test_rom_access_during_prefetch_info_run },
        { "TOGGLE PREFETCHER", test_toggle_prefetcher, test_toggle_prefetcher_info_init, test_toggle_prefetcher_info_run },
        { NULL }
    },

    2
};

static const TestCategory test_category_cpu = {
    {
        { "SWP LOCKS BUS", test_cpu_swp_locks_bus, test_cpu_swp_locks_bus_info_init, test_cpu_swp_locks_bus_info_run },
        { NULL }
    },
    
    1
};

static const TestCategory test_category_dma = {
    {
        { "CPU RUNS IDLES DURING DMA", test_cpu_runs_idles_throughout_dma, test_cpu_runs_idles_throughout_dma_info_init, test_cpu_runs_idles_throughout_dma_info_run  },
        { NULL }
    },

    1
};

static const TestCategory test_categories[NUM_TEST_CATEGORIES] = {
    test_category_cartridge,
    test_category_cpu,
    test_category_dma
};

static const State TEST_INFO = { state_test_info_init, state_test_info_run };

#endif