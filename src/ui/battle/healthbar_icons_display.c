#include "global.h"
#include "battle_controllers.h" // ChooseMoveStruct
#include "window.h" // blitBitmapRectToWindow
#include "palette.h" // LoadPalette
#include "strings.h" // gText_PokeSum_Power, gText_MoveInterfacePP, gText_TwoHyphens
#include "string_util.h" // ConvertIntToDecimalStringN, StringCopy
#include "battle_main.h" // gDisplayedStringBattle // maybe include "battle.h" instead
#include "battle_message.h" // BattlePutTextOnWindow
#include "random.h" // Random

#define B_INTERFACE_GFX_STATUS_PSN_BATTLER0 21
#define PAL_STATUS_PSN 0
#define MULTIPLIER_ICON_INDEX 25 // Offset for multiplier icons in sSplitIcons_Gfx

struct MoveMenuInfoIcon
{
    u8 width;
    u8 height;
    u16 offset;
};

static void BlitMenuInfoIcon(u8 windowId,const u8 *gfx, struct MoveMenuInfoIcon icon, u16 x, u16 y)
{
    BlitBitmapRectToWindow(windowId, &gfx[icon.offset * TILE_SIZE_4BPP], 0, 0, 128, 160, x, y, icon.width, icon.height);
}


static const u16 sSplitIcons_Pal[] = INCBIN_U16("graphics/battle_interface/menu_info.gbapal");
static const u8 sSplitIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/menu_info.4bpp");


// public
void HealthbarUpdateStatus(s16 _nothing)
{
    u32 statusIndex = 0;
    u32 posX = 0;
    u32 posY = 0;//3;
    u32 dX = 18;
    u32 i, index, numStatus;

    static const struct MoveMenuInfoIcon status_icons[] = {
        // { width, height, offset }
        [0] = { 16, 8,  0xc6}, // Attack
        [1] = { 16, 8,  0xc8}, // Defense
        [2] = { 16, 8,  0xca}, // Sp. Atk
        [3] = { 16, 8,  0xcc}, // Sp. Def
        [4] = { 16, 8,  0xce}, // Speed

        [5] = { 16, 8, 0xd6}, // negative Attack
        [6] = { 16, 8, 0xd8}, // negative Defense
        [7] = { 16, 8, 0xda}, // negative Sp. Atk
        [8] = { 16, 8, 0xdc}, // negative Sp. Def
        [9] = { 16, 8, 0xde}, // negative Speed

        [10] = { 16, 8, 0xe6}, // Accuracy
        [11] = { 16, 8, 0xe8}, // Evasion
        [12] = { 16, 8, 0xea}, // Critical Hit
        [13] = { 16, 8, 0xec}, // Flinch
        [14] = { 16, 8, 0xee}, // Confusion

        [15] = { 16, 8, 0xf6}, // negative Accuracy
        [16] = { 16, 8, 0xf8}, // negative Evasion
        [17] = { 16, 8, 0xfa}, // negative Critical Hit
        [18] = { 16, 8, 0xfc}, // negative Flinch
        [19] = { 16, 8, 0xfe}, // negative Confusion

        [20] = { 16, 8, 0x120}, // Poison
        [21] = { 16, 8, 0x122}, // Paralysis
        [22] = { 16, 8, 0x124}, // Sleep
        [23] = { 16, 8, 0x126}, // Freeze
        [24] = { 16, 8, 0x128}, // Burn

        [MULTIPLIER_ICON_INDEX] =     {8,8, 0x130}, // x1 multiplier
        [MULTIPLIER_ICON_INDEX + 1] = {8,8, 0x131}, // x2 multiplier
        [MULTIPLIER_ICON_INDEX + 2] = {8,8, 0x132}, // x3 multiplier
        [MULTIPLIER_ICON_INDEX + 3] = {8,8, 0x133}, // x4 multiplier
        [MULTIPLIER_ICON_INDEX + 4] = {8,8, 0x134}, // x5 multiplier
        [MULTIPLIER_ICON_INDEX + 5] = {8,8, 0x135}, // x6 multiplier
    };

    // SetWindowAttribute(B_WIN_HB_STATUS_0, WINDOW_TILEMAP_TOP, 0);

    // statusIcon = gBattleMons[gActiveBattler].status1;
    // if (statusIcon == STATUS1_NONE)
    //     return;

    LoadPalette(sSplitIcons_Pal, 10 * 0x10, sizeof(sSplitIcons_Pal));
    FillWindowPixelBuffer(B_WIN_HB_STATUS_0, PIXEL_FILL(0));
    // ClearWindowTilemap(B_WIN_HB_STATUS_0);
    numStatus = (u8) (Random() % 6) + 1;
    for (i = 0; i < numStatus; i++)
    {
        index = (u8)(Random() % 25); // random status effect
        BlitMenuInfoIcon(B_WIN_HB_STATUS_0, sSplitIcons_Gfx, status_icons[index], posX, posY);
        posX += 16;

        // index = (u8)(Random() % 5) + 1;
        // BlitMenuInfoIcon(B_WIN_HB_STATUS_0, sSplitIcons_Gfx, status_icons[MULTIPLIER_ICON_INDEX + index], posX, posY);
        // posX += 10;
    }
    // BlitMenuInfoIcon(B_WIN_HB_STATUS_0, sSplitIcons_Gfx, status_icons[0], posX, 0);
    // BlitMenuInfoIcon(B_WIN_HB_STATUS_0, sSplitIcons_Gfx, status_icons[1], posX + 3, 0);
    PutWindowTilemap(B_WIN_HB_STATUS_0);
    CopyWindowToVram(B_WIN_HB_STATUS_0, COPYWIN_FULL);
    // DebugPrintf("TEST");

    PutWindowTilemap(B_WIN_HB_STATUS_1);
    CopyWindowToVram(B_WIN_HB_STATUS_1, COPYWIN_FULL);
}



void StatusIcons_SetOffset(u16 x, u16 y)
{
    DebugPrintf("StatusIcons_SetOffset: x=%d, y=%d", x, y);
    SetWindowAttribute(B_WIN_HB_STATUS_0, WINDOW_TILEMAP_TOP, y / 8 + 5);
    // CopyWindowToVram(B_WIN_HB_STATUS_0, COPYWIN_FULL);
    HealthbarUpdateStatus(0);
}