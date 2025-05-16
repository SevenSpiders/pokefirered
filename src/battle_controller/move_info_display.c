#include "global.h"
#include "battle.h"
#include "battle_controllers.h" // ChooseMoveStruct
#include "window.h"
#include "palette.h" // LoadPalette

static const u16 sSplitIcons_Pal[] = INCBIN_U16("graphics/battle_interface/menu_info.gbapal");
static const u8 sSplitIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/menu_info.4bpp");

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

void MoveSelectionDisplayMoveType(void)
{
    struct ChooseMoveStruct *moveInfo;
	u32 moveCategory, iconIndex, moveType;
    u8 fillValue;
    u8 posX = 7;
    u8 posY = 3;

    static const struct MoveMenuInfoIcon category_icons[] = {
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

    // StringCopy(gDisplayedStringBattle, gText_PokeSum_Power);
    // BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MOVE_PWR);
    

	moveInfo = (struct ChooseMoveStruct*)(&gBattleBufferA[gActiveBattler][MAX_BATTLERS_COUNT]);
    moveCategory = gBattleMoves[moveInfo->moves[gMoveSelectionCursor[gActiveBattler]]].category;
    moveType =  gBattleMoves[moveInfo->moves[gMoveSelectionCursor[gActiveBattler]]].type;
    iconIndex = moveCategory + NUMBER_OF_MON_TYPES;
    fillValue = 15; // 0 black, 1 red,2 orange, 3 yellow, 4 light green, 5 green, 6 light blue
    // 7 blue, 8 violet, 9 beige, 10 light beige, 11 brown, 12 pink, 13 dark, 14 blackish, 15 white

	LoadPalette(sSplitIcons_Pal, 10 * 0x10, 0x20);
    FillWindowPixelRect(B_WIN_MOVE_CATEGORY, fillValue, 0, 0, 64, 16);
    BlitMenuInfoIcon(B_WIN_MOVE_CATEGORY, sSplitIcons_Gfx, category_icons[iconIndex], posX, posY);
    BlitMenuInfoIcon(B_WIN_MOVE_CATEGORY, sSplitIcons_Gfx, category_icons[moveType], posX +16, posY);
	PutWindowTilemap(B_WIN_MOVE_CATEGORY);
	CopyWindowToVram(B_WIN_MOVE_CATEGORY, COPYWIN_FULL);
}