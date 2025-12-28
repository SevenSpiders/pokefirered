#include "global.h"
#include "gflib.h"
#include "strings.h"
#include "battle_ui.h"
#include "battle_message.h" // BattlePutTextOnWindow
#include "battle_controllers.h" // ChooseMoveStruct
#include "graphics.h" // gMenuInfoElements_Gfx

// static const u16 sBattleIcons_Pal[] = INCBIN_U16("graphics/battle_interface/menu_info.gbapal");
// static const u8 sBattleIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/menu_info.4bpp");

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
    [MOVE_CATEGORY_PHYSICAL + NUMBER_OF_MON_TYPES] = {14, 12, 0x0A},
    [MOVE_CATEGORY_SPECIAL + NUMBER_OF_MON_TYPES] = {14, 12, 0x0C},
    [MOVE_CATEGORY_STATUS + NUMBER_OF_MON_TYPES] = {14, 12, 0x0E},
};

void BlitTypeIcon(u8 windowId, u8 iconId, u16 x, u16 y)
{
    BlitBitmapRectToWindow(windowId, &gMenuInfoElements_Gfx[gTypeIconData[iconId].offset * TILE_SIZE_4BPP], 0, 0, 128, 128, 
        x, y, gTypeIconData[iconId].width, gTypeIconData[iconId].height);
}


static void DisplayMoveType(u8 type)
{
    BlitTypeIcon(B_WIN_MOVE_TYPE, type, 0, 0);
}

static void DisplayMoveCategory(u8 category)
{
    BlitTypeIcon(B_WIN_MOVE_TYPE, category + NUMBER_OF_MON_TYPES, 16, 0);
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

void BattleUI_DisplayMoveInfo()
{
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleBufferA[gActiveBattler][4]);
    u32 move = moveInfo->moves[gMoveSelectionCursor[gActiveBattler]];

    DebugPrintf("display move info");
    DisplayMoveCategory(gBattleMoves[move].category);
    DisplayMoveType(gBattleMoves[move].type);
    DisplayMovePower(moveInfo->currentPower[gMoveSelectionCursor[gActiveBattler]]);
}
