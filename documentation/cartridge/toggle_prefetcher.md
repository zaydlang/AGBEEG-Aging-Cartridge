# Toggle Prefetcher

This test toggles the prefetcher to observe exactly what happens when you enable / disable the prefetcher.

## Prefetcher 1-Cycle Delay Behavior

When disabling the prefetcher, the prefetcher doesn't immediately stop - it seems to finish whatever transfer it was currently working on. Additionally, the prefetch buffer is not discarded. Rather, the contents of the prefetch buffer are used up by the CPU till the prefetch buffer empties. Once the prefetch buffer is empty, the next access the CPU makes is a nonsequential one.

When enabling the prefetcher, no interesting behavior is observed. The prefetcher seems to just immediately start filling the buffer from wherever the CPU is.

## How The Test Works

There are two subtests here. In one subtest, the prefetcher will be disabled and its timing effects will be recorded. In the other subtest, the prefetcher will be enabled and its timing effects will also be recorded. These two subtests are ran for 4 different waitstate configurations (the same ones used in the mGBA test suite, 0x_000, 0x_004, 0x_010, and 0x_014). For each waitstate configuration, 1-4 idle cycles are run before the WAITCNT modification, followed by 5 NOPs.

Press A within this test's submenu to cycle between the disable tests and the enable tests.
