#include "global.h"
#include "gflib.h"
#include "strings.h"
#include "battle_ui.h"
#include "battle_message.h" // BattlePutTextOnWindow
#include "battle_controllers.h" // ChooseMoveStruct
#include "graphics.h" // gMenuInfoElements_Gfx

static const u16 sBattleIcons_Pal[] = INCBIN_U16("graphics/battle_interface/status_icons.gbapal");
static const u8 sBattleIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/status_icons.4bpp");

struct IconData
{
    u8 width;
    u8 height;
    u16 offset;
};

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

void BattleUI_LoadGfx()
{
    LoadPalette(gMenuInfoElements2_Pal, BG_PLTT_ID(B_PLTT_TYPE), PLTT_SIZE_4BPP);
    LoadPalette(sBattleIcons_Pal, BG_PLTT_ID(B_PLTT_ICONS), PLTT_SIZE_4BPP);
    FillWindowPixelRect(B_WIN_MOVE_TYPE, 15, 0, 0, 64, 16);
    // FillWindowPixelRect(B_WIN_STATUS1, 15, 0, 0, 64, 16);
    BattleUI_UpdateStatusIcons();
}

void BattleUI_DisplayMoveInfo()
{
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleBufferA[gActiveBattler][4]);
    u32 move = moveInfo->moves[gMoveSelectionCursor[gActiveBattler]];

    // DebugPrintf("display move info");
    DisplayMoveCategory(gBattleMoves[move].category);
    DisplayMoveType(gBattleMoves[move].type);
    DisplayMovePower(moveInfo->currentPower[gMoveSelectionCursor[gActiveBattler]]);
    BattleUI_UpdateStatusIcons();
    PutWindowTilemap(B_WIN_MOVE_TYPE);
	CopyWindowToVram(B_WIN_MOVE_TYPE, COPYWIN_FULL);
}

void BattleUI_UpdateStatusIcons()
{
    DebugPrintf("update status");
    // DisplayStatIcon(STAT_ATK, FALSE);
    FillWindowPixelRect(B_WIN_STATUS1, 12, 0, 0, 64, 8);
    PutWindowTilemap(B_WIN_STATUS1);
	CopyWindowToVram(B_WIN_STATUS1, COPYWIN_FULL);
}
