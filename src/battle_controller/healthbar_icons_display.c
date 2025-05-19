#include "global.h"
#include "battle_controllers.h" // ChooseMoveStruct
#include "window.h" // blitBitmapRectToWindow
#include "palette.h" // LoadPalette
#include "strings.h" // gText_PokeSum_Power, gText_MoveInterfacePP, gText_TwoHyphens
#include "string_util.h" // ConvertIntToDecimalStringN, StringCopy
#include "battle_main.h" // gDisplayedStringBattle // maybe include "battle.h" instead
#include "battle_message.h" // BattlePutTextOnWindow


#define B_INTERFACE_GFX_STATUS_PSN_BATTLER0 21
#define PAL_STATUS_PSN 0

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
void HealthbarUpdateStatus(s16 posY)
{
    u32 statusIndex = 0;
    u32 posX = 6;
    // u32 posY = 1;
    u32 dX = 23;
    u32 i;

    static const struct MoveMenuInfoIcon status_icons[] = {
        [0] = { 20, 12, 0xC8 },
        [1]  = { 32, 12, 0xCC },
        // [2]  = { 32, 12, 0x48 },
        // [3]  = { 32, 12, 0x44 },
        // [4]  = { 32, 12, 0x6C },
        // [5]  = { 32, 12, 0x24 },
    };

    // statusIcon = gBattleMons[gActiveBattler].status1;
    // if (statusIcon == STATUS1_NONE)
    //     return;

    // LoadPalette(sSplitIcons_Pal, 10 * 0x10, sizeof(sSplitIcons_Pal));
    // FillWindowPixelRect(B_WIN_HB_STATUS_0, 1, 0, 0, 100, 100);
    for (i = 0; i < 4; i++)
    {
        BlitMenuInfoIcon(B_WIN_HB_STATUS_0, sSplitIcons_Gfx, status_icons[i%2], posX + i*dX, 0);
    }
    // BlitMenuInfoIcon(B_WIN_HB_STATUS_0, sSplitIcons_Gfx, status_icons[0], posX, 0);
    // BlitMenuInfoIcon(B_WIN_HB_STATUS_0, sSplitIcons_Gfx, status_icons[1], posX + 3, 0);
    PutWindowTilemap(B_WIN_HB_STATUS_0);
    CopyWindowToVram(B_WIN_HB_STATUS_0, COPYWIN_FULL);
    DebugPrintf("TEST");

    PutWindowTilemap(B_WIN_HB_STATUS_1);
    CopyWindowToVram(B_WIN_HB_STATUS_1, COPYWIN_FULL);
}