#include <tonc.h>
#include <string.h>

#include "grit/nintendo.h"
#include "video.h"

static uint16_t rgb_to_15(uint32_t rgb) {
    uint16_t r = (rgb >> 16) & 0xFF;
    uint16_t g = (rgb >>  8) & 0xFF;
    uint16_t b = (rgb >>  0) & 0xFF;

    return ((r >> 3) <<  0) | 
           ((g >> 3) <<  5) |
           ((b >> 3) << 10);
}

void clear_screen() {
    RegisterRamReset(RESET_PALETTE | RESET_VRAM | RESET_OAM);

    // disable all objs
    for (int i = 0; i < 128; i++) {
        oam_mem[i].attr0 = 0x0200;
    }
}

void reset_display() {
    clear_screen();

    // set up obj palettes
    pal_obj_mem[1] = rgb_to_15(0x2652eb); // blue

    // set up BG0 for a 4bpp 32x32t map, using
    // using charblock 0 and screenblock 31
    REG_BG0CNT = BG_CBB(3) | BG_SBB(28) | BG_4BPP | BG_REG_32x32;
    REG_BG3CNT = BG_CBB(0) | BG_SBB(30) | BG_8BPP | BG_REG_32x32;

    REG_DISPCNT  = DCNT_MODE0 | DCNT_BG0 | DCNT_BG3 | DCNT_OBJ;
    REG_DISPSTAT = DSTAT_VBL_IRQ;

    memcpy(pal_bg_mem,      nintendoPal,   nintendoPalLen);
    memcpy(&tile_mem[0][0], nintendoTiles, nintendoTilesLen);
    memcpy(&se_mem[30][0],  nintendoMap,   nintendoMapLen);

    // set up bg / font palettes
    pal_bg_mem[16 * 2 + 1] = rgb_to_15(0xEBD200); // gold
    pal_bg_mem[16 * 3 + 1] = rgb_to_15(0x490097); // violet
    pal_bg_mem[16 * 4 + 1] = rgb_to_15(0xD80000); // red
    pal_bg_mem[16 * 5 + 1] = rgb_to_15(0x108820); // green
    pal_bg_mem[16 * 6 + 1] = rgb_to_15(0xd820a0); // pink
}