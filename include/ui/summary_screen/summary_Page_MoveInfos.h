#ifndef GUARD_SUMMARY_PAGE_MOVEINFO
#define GUARD_SUMMARY_PAGE_MOVEINFO

#include "summary_Page.h"

SummaryPage *Page_MoveInfos_Init(void);
void Page_DrawMoveIcons(void);
void Page_PrintMoveTexts();
void Page_ScrollDown(u32 amount);
void Page_ScrollUp(u32 amount);
void Page_SetPokemon(struct Pokemon * pokemon);
void Page_DrawBoxes(void);
void Page_DrawTweens(u8 direction); // 1 = up, 2 = down
void Page_SetScrolling(bool8 b);
void Page_SetSwapping(bool8 b);
bool32 Page_SwapMoves(struct Pokemon * mon);
void Page_SetCursor(u32 index);
u32 Page_GetCursor(void);
u32 Page_GetSelectionCursor(void);
bool8 Page_IsSwapping();
bool32 Page_IsScrolling();
void Page_PrintMoveDescription(void);
#endif