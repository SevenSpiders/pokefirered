#include "main.h"
#include "ui/summary_screen/summary_Page.h"
#include "ui/summary_screen/summary_MoveChanger.h"
#include "constants/moves.h" // MOVE_NONE
#include "list_menu.h" // BlitMenuInfoIcon
#include "malloc.h" // AllocZeroed

#define POKESUM_WIN_MOVES_TYPE 5 // temp
#define GetMoveNamePrinterYpos(x) ((x) * 28 + 5)
#define GetMoveCategoryPrinterYpos(x) ((x) * 28 + 17) // icon is 12px high

extern void BlitMenuInfoIcon(u8 windowId, u8 iconId, u16 x, u16 y);


static SummaryPage *sPage;



SummaryPage *Page_MoveInfos_Init(void)
{
    sPage = AllocZeroed(sizeof(SummaryPage));
    sPage->index = 3;
    return sPage;
}



static void Page_DrawMoveTypeIcons(void)
{
    u8 i;
    u8 posXCategory = 4;
    u16 type, category;
    u8 windowId = sPage->windowIds[POKESUM_WIN_MOVES_TYPE];
    MoveData *moveData;

    FillWindowPixelBuffer(windowId, 0);

    for (i = 0; i < 4; i++)
    {
        moveData = MoveChanger_GetMoveData(i);
        if (moveData->id == MOVE_NONE)
            continue;
        
        BlitMenuInfoIcon(windowId, moveData->type + 1, 3, GetMoveNamePrinterYpos(i)-1);
        BlitMenuInfoIcon(windowId, moveData->category + 24, posXCategory, GetMoveCategoryPrinterYpos(i));
    }
}