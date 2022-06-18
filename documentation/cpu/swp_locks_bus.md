# Swp Locks Bus
This test checks the bus-locking behavior of the SWP opcode. 

## Bus Locking Behavior
A SWP opcode in ARMv4T is composed of a read cycle and a write cycle, in that order. Since SWP is
typically used by software to implement thread-related mechanisms like semaphores, the CPU locks
the bus throughout the read and write cycle, preventing anything else (e.g. DMA) from accessing 
the bus while the SWP is being executed.

## How The Test Works
This test has a function in IWRAM that will run a DMA then immediately perform a SWP. The DMA and
the SWP both write to the same word in memory. Because this function is in IWRAM, and because DMA 
takes 2 cycles to start up, the CPU has *just* enough time to begin executing the SWP opcode before
the DMA fires. The first thing the SWP opcode does is its read cycle, so this read cycle is 
guaranteed to occur before the DMA. Then, one of two things can happen:


A) The SWP opcode locks the bus during execution, causing the DMA to occur after the SWP write and
   therefore overwrite whatever value the SWP wrote.
   
B) The SWP opcode does not lock the bus during execution, causing the SWP write to occur after the
   DMA and therefore overwrite whatever value the DMA wrote.

The DMA writes the value 0xCAFEBABE, and the SWP writes the value 0xDEADBEEF. If done correctly,
then 0xCAFEBABE should be the final value written in `test_result`
