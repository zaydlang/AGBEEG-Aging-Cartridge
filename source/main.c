#include <tonc.h>
#include <maxmod.h>

#include "tests/tests.h"
#include "state.h"
#include "menu.h"

#include "soundbank.h"
#include "soundbank_bin.h"

int main() {
    switch_state(STATE_MENU, NULL);

	irq_init(isr_master);
	irq_add(II_VBLANK, mmVBlank);
        
    mmInitDefault( (mm_addr)soundbank_bin, 16 );

    while (1) {
        VBlankIntrWait();
        mmFrame();

        REG_IF = 0xFFFF;

        key_poll();

        state_run();
    }
}