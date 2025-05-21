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
// #include "bg.h" // FillBgTilemapBufferRect_Palette0

#define WIN_NAMES           0
#define WIN_MOVES           2
#define WIN_MOVES_TITLE     3

#define GetMoveNamePrinterYpos(x) ((x) * 28 + 5)
#define GetMoveCategoryPrinterYpos(x) ((x) * 28 + 17) // icon is 12px high
#define GetMovePpPrinterYpos(x) ((x) * 28 + 16) // names

// extern const u8 gText_PokeSum_Power; // not working


extern void BlitMenuInfoIcon(u8 windowId, u8 iconId, u16 x, u16 y);


static SummaryPage *sPage;
static u32 sOffset;
static u32 sCursorIndex;
static u32 numMoves;

static const u8 sPrintMoveTextColors[][3] = {
    {0, 7, 8}, // 0 default
    {0, 1, 2}, // 1 red
    {0, 3, 4}, // 2 yellow
    {0, 5, 6}, // 3 green
    {0, 8, 0}, // 4 power 
};

void Page_SetOffset(u32 offset)
{
    if (offset == 0)
        sOffset -= 1;
    else
        sOffset += offset;

    if (sOffset > numMoves - 6)
        sOffset = numMoves - 6;
    else if (sOffset < 0)
        sOffset = 0;

    DebugPrintf("set offset %d", sOffset);
}

void Page_FillRects()
{
    // test implementation to darken or lighten move backgrounds
    DebugPrintf("Fill rect");
    // FillBgTilemapBufferRect_Palette0(2, 0, 15, 13, 15, 7); // right side
}

SummaryPage *Page_MoveInfos_Init(void)
{
    DebugPrintf("init page");
    sPage = AllocZeroed(sizeof(SummaryPage));
    sPage->index = 3;
    return sPage;
}

void Page_SetCursor(u32 cursorIndex)
{
    sCursorIndex = cursorIndex;
}

void Page_SetPokemon(struct Pokemon * pokemon) 
{
    numMoves = MoveChanger_SetPokemon(pokemon);
}

void Page_PrintMoveTexts(u8 i)
{
    MoveData *moveData;
    moveData = MoveChanger_GetMoveData(i + sOffset);
    DebugPrintf("move id: %d window: %d", moveData->id, moveData->powerStr[2]);

    // Title
    AddTextPrinterParameterized3(sPage->windowIds[WIN_NAMES], FONT_NORMAL, 
        3, GetMoveNamePrinterYpos(i), sPrintMoveTextColors[0], TEXT_SKIP_DRAW, 
        gMoveNames[moveData->id]);
    // Power
    AddTextPrinterParameterized3(sPage->windowIds[WIN_NAMES], FONT_SMALL,
        21, GetMovePpPrinterYpos(i)-1, sPrintMoveTextColors[4], TEXT_SKIP_DRAW, 
        gMoveNames[MOVE_PSYCHO_BOOST]);
    // Power Value
    AddTextPrinterParameterized3(sPage->windowIds[WIN_NAMES], FONT_NORMAL,
        45, GetMovePpPrinterYpos(i), sPrintMoveTextColors[0], TEXT_SKIP_DRAW, 
        moveData->powerStr);
}


void Page_DrawMoveIcons(void)
{
    u32 i;
    u32 posXCategory = 4;
    u32 type, category;
    u32 windowId;
    MoveData *moveData;

    windowId = sPage->windowIds[WIN_MOVES];
    
    FillWindowPixelBuffer(windowId, 0);
    
    for (i = 0; i < 5; i++)
    {
        DebugPrintf("draw move icons in window %d", windowId);
        moveData = MoveChanger_GetMoveData(i + sOffset);
        if (moveData->id == MOVE_NONE)
            continue;
        
        BlitMenuInfoIcon(windowId, moveData->type + 1, 3, GetMoveNamePrinterYpos(i)-1);
        BlitMenuInfoIcon(windowId, moveData->category + 24, posXCategory, GetMoveCategoryPrinterYpos(i));
    }
}