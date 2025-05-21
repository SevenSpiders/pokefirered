#include "main.h"
#include "ui/summary_screen/summary_Page.h"
#include "ui/summary_screen/summary_MoveChanger.h"
#include "constants/moves.h" // MOVE_NONE
#include "list_menu.h" // BlitMenuInfoIcon
#include "malloc.h" // AllocZeroed
#include "menu.h" // AddText
#include "data.h" // move names
#include "string.h" // pokesum pwr
#include "text.h"

#define WIN_NAMES           0
#define WIN_MOVES           2
#define WIN_MOVES_TITLE     3

#define GetMoveNamePrinterYpos(x) ((x) * 28 + 5)
#define GetMoveCategoryPrinterYpos(x) ((x) * 28 + 17) // icon is 12px high
#define GetMovePpPrinterYpos(x) ((x) * 28 + 16) // names

extern const u8 *gText_PokeSum_Power;


extern void BlitMenuInfoIcon(u8 windowId, u8 iconId, u16 x, u16 y);


static SummaryPage *sPage;
static u32 sOffset;

static const u8 sPrintMoveTextColors[][3] = {
    {0, 7, 8}, // 0 default
    {0, 1, 2}, // 1 red
    {0, 3, 4}, // 2 yellow
    {0, 5, 6}, // 3 green
    {0, 8, 0}, // 4 power 
};

void Page_SetOffset(u32 offset)
{
    sOffset += offset;
    DebugPrintf("set offset %d", sOffset);
}

SummaryPage *Page_MoveInfos_Init(void)
{
    DebugPrintf("init page");
    sPage = AllocZeroed(sizeof(SummaryPage));
    sPage->index = 3;
    return sPage;
}

void Page_PrintMoveTexts(u8 i)
{
    MoveData *moveData;
    moveData = MoveChanger_GetMoveData(i + sOffset);

    // // Title
    // AddTextPrinterParameterized3(sPage->windowIds[WIN_NAMES], FONT_NORMAL, 
    //     3, GetMoveNamePrinterYpos(i), sPrintMoveTextColors[0], TEXT_SKIP_DRAW, 
    //     gMoveNames[moveData->id]);
    // // Power
    // AddTextPrinterParameterized3(sPage->windowIds[WIN_MOVES_TITLE], FONT_SMALL, 
    //     21, GetMovePpPrinterYpos(i)-1, sPrintMoveTextColors[4], TEXT_SKIP_DRAW, 
    //     gText_PokeSum_Power);
    // // Power Value
    // AddTextPrinterParameterized3(sPage->windowIds[WIN_MOVES_TITLE], FONT_NORMAL, 
    //     45, GetMovePpPrinterYpos(i), sPrintMoveTextColors[0], TEXT_SKIP_DRAW, 
    //     moveData->powerStr);
}


void Page_DrawMoveIcons(void)
{
    // u32 i;
    // u32 posXCategory = 4;
    // u32 type, category;
    // u32 windowId;
    // MoveData *moveData;
    
    // FillWindowPixelBuffer(windowId, 0);
    
    // for (i = 0; i < 5; i++)
    // {
    //     windowId = sPage->windowIds[WIN_MOVES];
    //     DebugPrintf("draw move icons in window %d", windowId);
    //     moveData = MoveChanger_GetMoveData(i + sOffset);
    //     if (moveData->id == MOVE_NONE)
    //         continue;
        
    //     BlitMenuInfoIcon(windowId, moveData->type + 1, 3, GetMoveNamePrinterYpos(i)-1);
    //     BlitMenuInfoIcon(windowId, moveData->category + 24, posXCategory, GetMoveCategoryPrinterYpos(i));
    // }
}