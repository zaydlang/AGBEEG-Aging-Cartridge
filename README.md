# AGBEEG-Aging-Cartridge
A collection of some of the tests I've written

## What This Cartridge Tests
This cartridge tests some extremely nitpicky behavior about the GBA. It will not test anything that is required to get most commercial / homebrew games running - rather, it aims to test things that are only needed if you want to create a perfectly accurate emulator (and it may help get some poorly designed roms running too! :P)

## How to Use
This cartridge is very similar to AGS. Unlike AGS though, this cartridge allows you to select a test with the arrow keys, and press A to see information about why that test failed. You can press B to go back to the main menu. If you want more information as to why a test failed, you can check `documentation/<category name>/<test name>.md`. 


Note that this is very much a WIP test suite. There are only a few tests as of writing this, and I don't know how to pass all of them. As time passes, I intend to add more tests to this project. PRs welcome!

## Using this Test Rom for CI

Using this test rom for CI is actually really easy. Create a single-byte MMIO register at 0x04999990. The ROM will send test results to that address in the following format:
- null-terminated test name (as a string)
- null-terminated test result ("PASS" or "FAIL")
- ... repeat for all tests ...
- null-terminated text "DONE"