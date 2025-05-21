#ifndef GUARD_SUMMARY_PAGE_MOVEINFO
#define GUARD_SUMMARY_PAGE_MOVEINFO

#include "summary_Page.h"

SummaryPage *Page_MoveInfos_Init(void);
void Page_DrawMoveIcons(void);
void Page_PrintMoveTexts(u8 i);
void Page_SetOffset(u32 offset);
void Page_SetPokemon(struct Pokemon * pokemon);
void Page_FillRects(void)
#endif