#ifndef GUARD_STATUSICONS
#define GUARD_STATUSICONS


#define STAT_NEGATE 7 // Offset for negative status icons in icons_status array
#define ICON1(address) { 16, 8,  address} // normal icons
#define ICON2(address) { 11, 8, address} // number icon
#define ICON3(address) { 16, 8,  address} // type icons

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
    // { width, height, offset }
    [STAT_ATK] =    { 16, 8,  0xc6}, // Attack
    [STAT_DEF] =    { 16, 8,  0xc8}, // Defense
    [STAT_SPEED] =  { 16, 8,  0xce}, // Speed
    [STAT_SPATK] =  { 16, 8,  0xca}, // Sp. Atk
    [STAT_SPDEF] =  { 16, 8,  0xcc}, // Sp. Def
    [STAT_ACC] =    { 16, 8,  0xe6}, // Accuracy
    [STAT_EVASION] =    { 16, 8, 0xe8}, // Evasion

    [STAT_ATK   + STAT_NEGATE] = { 16, 8, 0xd6}, // negative Attack
    [STAT_DEF   + STAT_NEGATE] = { 16, 8, 0xd8}, // negative Defense
    [STAT_SPEED + STAT_NEGATE] = { 16, 8, 0xda}, // negative Sp. Atk
    [STAT_SPATK + STAT_NEGATE] = { 16, 8, 0xdc}, // negative Sp. Def
    [STAT_SPDEF + STAT_NEGATE] = { 16, 8, 0xde}, // negative Speed
    [STAT_ACC   + STAT_NEGATE] = { 16, 8, 0xf6}, // negative Accuracy
    [STAT_EVASION + STAT_NEGATE] = { 16, 8, 0xf8}, // negative Evasion
};

static const IconData icons_status[] = {
    // { width, height, offset }
    [STATUS_NULL] = {},

    [STATUS_SLEEP] = { 16, 8, 0x124}, // Sleep
    [STATUS_POISON] = { 16, 8, 0x120}, // Poison
    [STATUS_BURN] = { 16, 8, 0x128}, // Burn
    [STATUS_FREEZE] = { 16, 8, 0x126}, // Freeze
    [STATUS_PARALYSIS] = { 16, 8, 0x122}, // Paralysis


    [STATUS_ALWAYS_HITS] = { 16, 8, 0xea}, // Critical Hit
    [STATUS_FLINCHED] = { 16, 8, 0xec}, // Flinch
    [STATUS_CONFUSED] = { 16, 8, 0xea}, // Confusion
    [STATUS_WRAPPED] = { 16, 8, 0xfe}, // wrapped


};

static const IconData icons_numbers[] = {
    // { width, height, offset }
    [0] = {11, 8, 0x13a}, // x0 multiplier
    [1] = {11, 8, 0x130}, // x1 multiplier
    [2] = {11, 8, 0x132}, // x2 multiplier
    [3] = {11, 8, 0x134}, // x3 multiplier
    [4] = {11, 8, 0x136}, // x4 multiplier
    [5] = {11, 8, 0x138}, // x5 multiplier
    [6] = {11, 8, 0x13a}, // x6 multiplier
    [7] = {11, 8, 0x13a}, // x7 multiplier
    [8] = {11, 8, 0x13a}, // x8 multiplier
    [9] = {11, 8, 0x13a}, // x9 multiplier
    [10] = {11, 8, 0x13a}, // highlighted x6 multiplier
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

static void BlitIcon(u8 windowId,const u8 *gfx, IconData icon, u16 x, u16 y)
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
    LoadPalette(sIcons_Pal, 10 * 0x10, sizeof(sIcons_Pal));
}

#endif // GUARD_STATUSICONS