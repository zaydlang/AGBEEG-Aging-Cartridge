#include <tonc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "font.h"

__attribute__((section(".iwram")))
static PrintData print_data;

void init_print(int bg, int screenblock, int charblock) {
    print_data.screenblock = screenblock;

    memset(&se_mem[screenblock], 0xFF, 32 * 32 * 2);
    memcpy(&tile_mem[charblock], FONT, NUM_CHARACTERS_IN_FONT * 8 * 4);
}

static uint8_t get_texture_index_from_character(char c) {
    if ('A' <= c && c <= 'Z') return c - 'A';
    if ('0' <= c && c <= '9') return c - '0' + 31;

    if (c == '.') return 26;
    if (c == '>') return 27;
    if (c == '!') return 28;
    if (c == '_') return 29;
    if (c == ':') return 30;

    return 0xFF;
}

static void print_character(char c, int tile, int palette) {
    se_mem[print_data.screenblock][tile] = (palette << 12) | get_texture_index_from_character(c);
}

void ags_print(char* string, int x, int y, int palette) {
    int tile = y * 32 + x;
    
    for (int i = 0; string[i] != 0; i++) {
        print_character(string[i], tile + i, palette);
    }
}

__attribute__((section(".iwram"))) char printf_out[30];

void ags_printf(char* format, int x, int y, int palette, ...) {
    // dumb hack but it works lol
    va_list va;
    va_start(va, palette);
    vsprintf((char*) printf_out, format, va);
    va_end(va);
    ags_print((char*) printf_out, x, y, palette);
} 

void ags_erase(int x1, int y, int x2) {
    for (int x = x1; x < x2; x++) {
        se_mem[print_data.screenblock][y * 32 + x] = 0xFF;
    }
}
