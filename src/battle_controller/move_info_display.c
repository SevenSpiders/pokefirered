#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
// #include "list_menu.h" // 
// #include "menu.h" // MENU_INFO_ICON_CATEGORY_PHYSICAL


// move_category_icons
// static const u16 sSplitIcons_Pal[] = INCBIN_U16("graphics/battle_interface/menu_info.gbapal");
// static const u8 sSplitIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/menu_info.4bpp");

// static void MoveSelectionDisplayMoveType(void)
// {
//     struct ChooseMoveStruct *moveInfo;
// 	u32 moveCategory;

// 	moveInfo = (struct ChooseMoveStruct*)(&gBattleBufferA[gActiveBattler][MAX_BATTLERS_COUNT]);
//     moveCategory = gBattleMoves[moveInfo->moves[gMoveSelectionCursor[gActiveBattler]]].category;
// 	LoadPalette(sSplitIcons_Pal, 10 * 0x10, 0x20);
// 	BlitBitmapToWindow(B_WIN_MOVE_CATEGORY, sSplitIcons_Gfx + 0x80 * moveCategory, 0, 0, 16, 16);
//     // BlitMenuInfoIcon(B_WIN_MOVE_CATEGORY, moveCategory + MENU_INFO_ICON_CATEGORY_PHYSICAL, 0, 0);

// 	PutWindowTilemap(B_WIN_MOVE_CATEGORY);
// 	CopyWindowToVram(B_WIN_MOVE_CATEGORY, 3);
// }