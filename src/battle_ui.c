#include "global.h"
#include "gflib.h"
#include "strings.h"
#include "battle_ui.h"
#include "battle_message.h" // BattlePutTextOnWindow
#include "battle_controllers.h" // ChooseMoveStruct
#include "graphics.h" // gMenuInfoElements_Gfx


typedef struct {
    u32 windowId;
    u32 windowId2;
    u32 windowIds[3];
    s32 offsetX;
    s32 offsetY;
    u8 row;
    u16 tilemapTop;
    bool8 isVisible; 
} IconTray;

struct IconData
{
    u8 width;
    u8 height;
    u16 offset;
};

#define WIN_P1 0
#define WIN_P2 1
#define WIN_O1 2
#define WIN_O2 3
#define OFFSET_1 20
#define OFFSET_2 40

static const struct WindowTemplate sIconWindowTemplates[] = {
    [0] = { // player
        .bg = 0,
        .tilemapLeft = 17,
        .tilemapTop = 13,
        .width = 8,
        .height = 1,
        .paletteNum = B_PLTT_ICONS,
        .baseBlock = B_TILE_ICONS
    },
    [1] = { // opponent
        .bg = 0,
        .tilemapLeft = 3,
        .tilemapTop = 5 +OFFSET_1,
        .width = 8,
        .height = 1,
        .paletteNum = B_PLTT_ICONS,
        .baseBlock = B_TILE_ICONS + 8
    }
};

static const u16 sBattleIcons_Pal[] = INCBIN_U16("graphics/battle_interface/status_icons.gbapal");
static const u8 sBattleIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/status_icons.4bpp");
static IconTray sIconTrays[2]; // 0: player, 1: enemy

static const struct IconData gTypeIconData[] = {
    [TYPE_NORMAL]   = { 32, 12, 0x20 },
    [TYPE_FLYING]   = { 32, 12, 0x60 },
    [TYPE_GROUND]   = { 32, 12, 0x48 },
    [TYPE_ROCK]     = { 32, 12, 0x44 },
    [TYPE_BUG]      = { 32, 12, 0x6C },
    [TYPE_FIRE]     = { 32, 12, 0x24 },
    [TYPE_WATER]    = { 32, 12, 0x28 },
    [TYPE_GRASS]    = { 32, 12, 0x2C },
    [TYPE_ELECTRIC] = { 32, 12, 0x40 },
    [TYPE_ICE]      = { 32, 12, 0x4C },
    [TYPE_FIGHTING] = { 32, 12, 0x64 },
    [TYPE_GHOST]    = { 32, 12, 0x68 },
    [TYPE_POISON]   = { 32, 12, 0x80 },
    [TYPE_PSYCHIC]  = { 32, 12, 0x84 },
    [TYPE_STEEL]    = { 32, 12, 0x88 },
    [TYPE_DARK]     = { 32, 12, 0x8C },
    [TYPE_DRAGON]   = { 32, 12, 0xA0 },
    [TYPE_MYSTERY]  = { 32, 12, 0xA4 }, 
    [MOVE_CATEGORY_PHYSICAL + NUMBER_OF_MON_TYPES] = {12, 12, 0x04},
    [MOVE_CATEGORY_SPECIAL + NUMBER_OF_MON_TYPES] = {12, 12, 0x06},
    [MOVE_CATEGORY_STATUS + NUMBER_OF_MON_TYPES] = {12, 12, 0x08},
};
#define STAT_NEGATIVE NUM_STATS-1

static const struct IconData gStatIconData[] = {
    [0] = {},
    [STAT_ATK]   = { 32, 12, 0x20 },
    [STAT_DEF]  = { 32, 12, 0x24 },
    [STAT_SPEED]  = { 32, 12, 0x28 },
    [STAT_SPATK]  = { 32, 12, 0x2C },
    [STAT_SPDEF]  = { 32, 12, 0x30 },
    [STAT_ATK + STAT_NEGATIVE]   = { 32, 12, 0x40 },
    [STAT_DEF + STAT_NEGATIVE]  = { 32, 12, 0x44 },
    [STAT_SPEED + STAT_NEGATIVE]  = { 32, 12, 0x48 },
    [STAT_SPATK + STAT_NEGATIVE]  = { 32, 12, 0x4C },
    [STAT_SPDEF + STAT_NEGATIVE]  = { 32, 12, 0x50 },
};


static void BlitTypeIcon(u8 windowId, u8 iconId, u16 x, u16 y)
{
    BlitBitmapRectToWindow(windowId, &gMenuInfoElements_Gfx[gTypeIconData[iconId].offset * TILE_SIZE_4BPP], 0, 0, 128, 128, 
        x, y, gTypeIconData[iconId].width, gTypeIconData[iconId].height);
}

static void BlitStatIcon(u8 windowId, u8 iconId, u16 x, u16 y)
{
    BlitBitmapRectToWindow(windowId, &sBattleIcons_Gfx[gStatIconData[iconId].offset * TILE_SIZE_4BPP], 0, 0, 128, 160, 
        x, y, gStatIconData[iconId].width, gStatIconData[iconId].height);
}


static void DisplayMoveType(u8 type)
{
    BlitTypeIcon(B_WIN_MOVE_TYPE, type, 0, 0);
}

static void DisplayMoveCategory(u8 category)
{
    BlitTypeIcon(B_WIN_MOVE_TYPE, category + NUMBER_OF_MON_TYPES, 34, 0);
}

static void DisplayMovePower(u8 power)
{
    // Pwr
    StringCopy(gDisplayedStringBattle, gText_MovePwr);
    BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_PP);    
    
    // Power number
    if (power == 0)
    {
        *gDisplayedStringBattle = CHAR_SPACE;
        StringCopy(gDisplayedStringBattle+1, gText_PokeSum_TwoHyphens);
    }
    else
        ConvertIntToDecimalStringN(gDisplayedStringBattle, power, STR_CONV_MODE_RIGHT_ALIGN, 3);
    BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_PP_REMAINING); // defined in battle_bg.c
}

static void DisplayStatIcon(u8 statType, bool8 negative)
{
    if (negative) statType += STAT_NEGATIVE;
    BlitStatIcon(B_WIN_STATUS1, statType, 0, 0);
}

static void BattleUI_AddWindow(u8 battlerId)
{
    u32 i;
    u32 offsetY = sIconWindowTemplates[battlerId].tilemapTop;
    for (i=0; i<3; i++)
    {
        sIconTrays[battlerId].windowIds[i] = AddWindow(&sIconWindowTemplates[battlerId]);
        sIconTrays[battlerId].isVisible = (sIconTrays[battlerId].windowIds[i] != WINDOW_NONE);
        SetWindowAttribute(sIconTrays[battlerId].windowIds[i], WINDOW_TILEMAP_TOP, OFFSET_1*i + offsetY);
        DebugPrintf("add tray window %d", sIconTrays[battlerId].windowIds[i]);
    }
}

static void BattleUI_RemoveWindow(u8 battlerId)
{
    ClearWindowTilemap(sIconTrays[battlerId].windowId);
    RemoveWindow(sIconTrays[battlerId].windowId);
    RemoveWindow(sIconTrays[battlerId].windowId2);
    sIconTrays[battlerId].isVisible = FALSE;
}

void BattleUI_LoadGfx()
{
    sIconTrays[0].windowId = B_WIN_STATUS1;
    sIconTrays[0].tilemapTop = 13;

    sIconTrays[1].windowId = B_WIN_STATUS2;
    sIconTrays[1].tilemapTop = 13;

    LoadPalette(gMenuInfoElements2_Pal, BG_PLTT_ID(B_PLTT_TYPE), PLTT_SIZE_4BPP);
    LoadPalette(sBattleIcons_Pal, BG_PLTT_ID(B_PLTT_ICONS), PLTT_SIZE_4BPP);
    FillWindowPixelRect(B_WIN_MOVE_TYPE, 15, 0, 0, 64, 16);
    // FillWindowPixelRect(B_WIN_STATUS1, 15, 0, 0, 64, 16);
    // BattleUI_UpdateStatusIcons();
    // BattleUI_AddWindow(0);
    BattleUI_UpdateStatusIcons(0);
}

void BattleUI_DisplayMoveInfo()
{
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleBufferA[gActiveBattler][4]);
    u32 move = moveInfo->moves[gMoveSelectionCursor[gActiveBattler]];

    // DebugPrintf("display move info");
    DisplayMoveCategory(gBattleMoves[move].category);
    DisplayMoveType(gBattleMoves[move].type);
    DisplayMovePower(moveInfo->currentPower[gMoveSelectionCursor[gActiveBattler]]);
    // BattleUI_UpdateStatusIcons();
    PutWindowTilemap(B_WIN_MOVE_TYPE);
	CopyWindowToVram(B_WIN_MOVE_TYPE, COPYWIN_FULL);
}

void BattleUI_UpdateStatusIcons(u8 battlerId)
{
    // u32 windowId = sIconTrays[battlerId].windowIds[0];
    // u32 windowId2 = sIconTrays[battlerId].windowIds[1];
    u32 windowId = sIconTrays[battlerId].windowId;
    DebugPrintf("update status %d, %d", windowId, 0);
    // DisplayStatIcon(STAT_ATK, FALSE);
    // check isVisible
    FillWindowPixelRect(windowId, 12, 0, 0, 64, 8);
    PutWindowTilemap(windowId);
	CopyWindowToVram(windowId, COPYWIN_FULL);

    // FillWindowPixelRect(windowId2, 12, 0, 0, 64, 8);
    // PutWindowTilemap(windowId2);
    // CopyWindowToVram(windowId2, COPYWIN_FULL);
}

void BattleUI_UpdateBG0Offset(u16 x, u16 y)
{
    // u32 i;
    // DebugPrintf("window id %d", sIconTrays[0].windowId);
    // for (i=0; i<2; i++)
    // {
    //     u32 tilemapTop =  y / 8 + sIconTrays[i].tilemapTop;
    //     SetWindowAttribute(sIconTrays[i].windowId, WINDOW_TILEMAP_TOP, tilemapTop);
    // }
    u32 tilemapTop =  y / 8 + sIconTrays[0].tilemapTop;
    SetWindowAttribute(sIconTrays[0].windowId, WINDOW_TILEMAP_TOP, tilemapTop);
    BattleUI_UpdateStatusIcons(0);
}
// static bool8 sIsShowingInfo;

// #define PALETTE_ICONS (1 << 10)
// #define PALETTE_TEXT_BOX (1 << 0)
// #define PALETTE_HP (1 << 1)
// #define PALETTE_TEXT (1 << 5)
// #define PALETTES_TO_FADE PALETTES_ALL - PALETTE_ICONS - PALETTE_TEXT_BOX - PALETTE_HP - PALETTE_TEXT
// // #define PALETTES_TO_FADE PALETTES_OBJECTS

// static void ShowInfo(void)
// {
//     Healthbox_Blank(gHealthboxSpriteIds[1]);
//     // show info window
//     DebugPrintf("Show info");
//     BeginNormalPaletteFade(PALETTES_TO_FADE, 0, 0, 8, RGB_BLACK);
// }

// static void ResetHealthBar(void)
// {
//     Healthbox_HideInfo(gHealthboxSpriteIds[1]);
//     UpdateHealthboxAttribute(gHealthboxSpriteIds[1], &gEnemyParty[gBattlerPartyIndexes[0]], HEALTHBOX_ALL);
//     DebugPrintf("Hide info");
//     BeginNormalPaletteFade(PALETTES_TO_FADE, 0, 8, 0, RGB_BLACK);
// }
