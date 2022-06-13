# ROM Access During Prefetch
This test checks some weird behavior of the GBA prefetcher which can cause some ROM data accesses to delay one cycle longer than expected. 

## Prefetcher 1-Cycle Delay Behavior
Basically, when the prefetcher is currently in the middle of prefetching an opcode and a ROM data access is made (either reads or writes, doesn't matter), the access will idle an extra cycle than it normally does if either:

A) The prefetcher is in THUMB mode and is 1 cycle away from finishing it's current fetch.

B) The prefetcher is in ARM mode and is 1 cycle away from finishing fetching either halfword that makes up the full word.

It's worth noting that ROM accesses normally take (<# of waitstates> + 1) cycles to complete. It seems logical that this + 1 cycle is special in some way. Exactly how it's special, I'm not sure, but the fact remains that if a ROM data access is made while the prefetcher is in that special cycle, the ROM access stalls an extra cycle.

## How The Test Works
This test will work through 4 different waitstate configurations (the same ones used in the mGBA test suite, 0x4000, 0x4004, 0x4010, and 0x4014). For each waitstate configuration, it will run 1-8 idle cycles and then immediately perform a ROM data read. Then it will time how long that data read took and compare it against results from hardware.

The test will perform this same procedure for both reads and writes. The expected behavior for reads and writes is the same. This makes a total of 32 * 2 = 64 tests.

Press A within this test's submenu to cycle between the read tests and the write tests.
