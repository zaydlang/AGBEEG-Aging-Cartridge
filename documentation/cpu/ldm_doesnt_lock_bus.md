# Ldm Doesn't Lock Bus
This test checks the bus-locking behavior of the LDM opcode. 

## Bus Locking Behavior
See swp_locks_bus.md within the same directory. Compared to SWP, LDM does not lock the bus. In
other words, DMA can interrupt the LDM and happen in the middle of two of the LDM's memory
accesses.

## How The Test Works
This test has a function in IWRAM that will run a DMA then immediately perform a LDM. The DMA will
try to overwrite a value in memory that the LDM will try to read from. The LDM has two reads, and
the second read will be from the region that the DMA is overwriting. Because this test is in IWRAM,
the LDM will have just enough time to execute its first read before the DMA fires. Two things can
happen:

A) The LDM opcode locks the bus during execution, causing the DMA to occur after the LDM read and
   therefore the LDM reads the original value.
   
B) The LDM opcode does not lock the bus during execution, causing the LDM read to occur after the
   DMA and therefore the LDM reads the modified value

The region starts off with a value of 0xCAFEBABE, and DMA overwrites this with the value 0xDEADBEEF. 
If done correctly, then 0xDEADBEEF should be the value the LDM reads.
