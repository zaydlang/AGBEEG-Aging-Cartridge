# Stm Doesn't Lock Bus
This test checks the bus-locking behavior of the STM opcode. 

## Bus Locking Behavior
See swp_locks_bus.md within the same directory. Compared to SWP, STM does not lock the bus. In
other words, DMA can interrupt the STM and happen in the middle of two of the STM's memory
accesses.

## How The Test Works
This test has a function in IWRAM that will run a DMA then immediately perform a STM. The DMA will
try to overwrite a value in memory that the STM will try to read from. The STM has two writes, and
the second write will be to the region that the DMA is also overwriting. Because this function is 
in IWRAM, and because DMA takes 2 cycles to start up, the CPU has *just* enough time to finish 
executing the first write of the STM opcode before the DMA fires. One of two things can happen:


A) The STM opcode locks the bus during execution, causing the DMA to occur after the STM write and
   therefore overwrite whatever value the STM wrote.
   
B) The STM opcode does not lock the bus during execution, causing the STM write to occur after the
   DMA and therefore overwrite whatever value the DMA wrote.

The DMA writes the value 0xCAFEBABE twice, and the STM writes the values 0xDEDEDEDE and 0xBABABABA. 
If done correctly, then 0xCAFEBABE and 0xBABABABA should be the final valueS written in `test_result`.
