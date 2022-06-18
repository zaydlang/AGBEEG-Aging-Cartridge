# CPU Runs Idles During DMA
This test checks that DMA only halts the CPU once the CPU attempts to access the bus.

## CPU Idle Cycle Behavior
The CPU can actually keep running during a DMA and finish any idle cycles it wants. The CPU will only halt once it tries to access the bus, because the CPU can't get access to the bus if DMA is currently accessing it.


It's worth noting that there may be a missing piece to this test. I implemented this theory in my emulator, and while I am now passing the 0x4000 and 0x4004 waitstate configurations, I do not pass the 0x4010 and 0x4014 ones. I'm not even 1 cycle off, I'm 4 whole cycles off. Maybe I implemented this wrongly in my emulator, or maybe there's some nuance to this behavior that is not fully understood. Feel free to PR this test suite if you research this and find the missing puzzle piece.

## How The Test Works
This test will work through 4 different waitstate configurations (the same ones used in the mGBA test suite, 0x4000, 0x4004, 0x4010, and 0x4014). For each waitstate configuration, it will trigger a DMA, and then immediately it will run 1-4 idle cycles using a `mul` opcode. Then it will time how long that `mul` opcode took and compare it against results from hardware.
