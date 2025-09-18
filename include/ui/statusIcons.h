#ifndef GUARD_STATUSICONS
#define GUARD_STATUSICONS


#define STAT_NEGATE 7 // Offset for negative status icons in icons_status array
// { width, height, offset }
#define ICON1(address) { 14, 8,  address} // normal icons
#define ICON_NUMBER(address) { 8, 8, address}
#define ICON_TYPE(address) { 32, 8,  address}
#define ICON_CATEGORY(address) {14, 11, address}

#include "pokemon.h" // BattlePokemon + stats

typedef struct
{
    u8 width;
    u8 height;
    u16 offset;
} IconData;

// static const u16 sIcons_Pal[] = INCBIN_U16("graphics/battle_interface/menu_info.gbapal");
// static const u8 sIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/menu_info.4bpp");
static const u16 sIcons_Pal[] = INCBIN_U16("graphics/battle_interface/icons.gbapal");
static const u8 sIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/icons.4bpp");


static const IconData icons_types[] = {
};

static const IconData icons_stats[] = {
    [STAT_ATK] =    ICON1(0), // Attack
    [STAT_DEF] =    ICON1(2), // Defense
    [STAT_SPEED] =  ICON1(4), // Speed
    [STAT_SPATK] =  ICON1(6), // Sp. Atk
    [STAT_SPDEF] =  ICON1(8), // Sp. Def
    [STAT_ACC] =    ICON1(10), // Accuracy
    [STAT_EVASION] =    ICON1(12), // Evasion

    [STAT_ATK   + STAT_NEGATE] = ICON1(16),
    [STAT_DEF   + STAT_NEGATE] = ICON1(18),
    [STAT_SPEED + STAT_NEGATE] = ICON1(20), // negative Sp. Atk
    [STAT_SPATK + STAT_NEGATE] = ICON1(22), // negative Sp. Def
    [STAT_SPDEF + STAT_NEGATE] = ICON1(24), // negative Speed
    [STAT_ACC   + STAT_NEGATE] = ICON1(26), // negative Accuracy
    [STAT_EVASION + STAT_NEGATE] = ICON1(28), // negative Evasion
};

static const IconData icons_status[] = {
    // { width, height, offset }
    [STATUS_NONE] = {},

    // row 1
    [STATUS_POISON] =       ICON1(64),
    [STATUS_SLEEP] =        ICON1(66),
    [STATUS_BURN] =         ICON1(68),
    [STATUS_FREEZE] =       ICON1(70),
    [STATUS_PARALYSIS] =    ICON1(72),

    // row 2
    [STATUS_CONFUSED] =     ICON1(80),
    [STATUS_WRAPPED] =      ICON1(82),
    // [STATUS_CONFUSED] =     ICON1(84),
    // [STATUS_WRAPPED] =      ICON1(86),
};

static const IconData icons_numbers[] = {
    // { width, height, offset }
    [0] = ICON_NUMBER(32), // x0 multiplier
    [1] = ICON_NUMBER(33), // x1 multiplier
    [2] = ICON_NUMBER(34), // x2 multiplier
    [3] = ICON_NUMBER(35), // x3 multiplier
    [4] = ICON_NUMBER(36), // x4 multiplier
    [5] = ICON_NUMBER(37), // x5 multiplier
    [6] = ICON_NUMBER(38), // x6 multiplier
    [7] = ICON_NUMBER(39), // x7 multiplier
    [8] = ICON_NUMBER(40), // x8 multiplier
    [9] = ICON_NUMBER(41), // x9 multiplier
    [10] = ICON_NUMBER(42), // highlighted x6 multiplier
};

static const IconData icons_zone[] = {
    [0] = { 19, 10, 4}, // Green Background
    [1] = { 19, 10, 7}, // Red Background
    [2] = { 16, 8,  0xc0}, // Spikes x1
    [3] = { 16, 8,  0xc2}, // Spikes x2
    [4] = { 16, 8,  0xc4}, // Spikes x3
    [5] = { 16, 8,  0xd0}, // Light Screen
    [6] = { 16, 8,  0xd2}, // Reflect
    [7] = { 16, 8,  0xd4}, // Safeguard
    [8] = { 16, 8,  0xe0}, // Mist
};

static void BlitIcon(u8 windowId, IconData icon, u16 x, u16 y)
{
    BlitBitmapRectToWindow(
        windowId, 
        &sIcons_Gfx[icon.offset * TILE_SIZE_4BPP], // gfx header
        0, 0, 128, 160, // gfx dimensions
        x, y, icon.width, icon.height
    );
}

static void Icons_Init()
{
    LoadPalette(sIcons_Pal, PLTT_ID(11), sizeof(sIcons_Pal));
}

#endif // GUARD_STATUSICONS