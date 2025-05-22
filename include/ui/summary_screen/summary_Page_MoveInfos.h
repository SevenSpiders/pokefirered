#ifndef GUARD_SUMMARY_PAGE_MOVEINFO
#define GUARD_SUMMARY_PAGE_MOVEINFO

#include "summary_Page.h"

SummaryPage *Page_MoveInfos_Init(void);
void Page_DrawMoveIcons(void);
void Page_PrintMoveTexts(u8 i);
void Page_ScrollDown(u32 amount);
void Page_ScrollUp(u32 amount);
void Page_SetPokemon(struct Pokemon * pokemon);
void Page_DrawBoxes(void);
void Page_SetScrolling(bool8 b);
void Page_SetSwapping(bool8 b);
#endif