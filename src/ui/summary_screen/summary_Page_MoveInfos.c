#include "main.h"
#include "ui/summary_screen/summary_Page.h"
#include "ui/summary_screen/summary_MoveChanger.h"
#include "ui/summary_screen/drawMoveBox.h"
#include "constants/moves.h" // MOVE_NONE
#include "list_menu.h" // BlitMenuInfoIcon
#include "malloc.h" // AllocZeroed
#include "menu.h" // AddText
#include "data.h" // move names
#include "string.h" // pokesum pwr
#include "text.h"
#include "bg.h"

#define WIN_NAMES           0
#define WIN_MOVES           2
#define WIN_MOVES_TITLE     3

#define GetMoveNamePrinterYpos(x) ((x) * 28 + 5)
#define GetMoveCategoryPrinterYpos(x) ((x) * 28 + 17) // icon is 12px high
#define GetMovePpPrinterYpos(x) ((x) * 28 + 16) // names

// extern const u8 gText_PokeSum_Power; // does not work


extern void BlitMenuInfoIcon(u8 windowId, u8 iconId, u16 x, u16 y);

static u8 state;
static SummaryPage *sPage;
static u32 sScrollIndex;
static u32 maxScroll;

// 0 transparent, 1 beige, 2 light yellow, 3 orange, 4 yellow, 5 red, 6 ligth orange, 7 black, 8 light grey
// new 9 white -> edit summary_screen\text_moves.pal
static const u8 sPrintMoveTextColors[][3] = {
    {0, 7, 8}, // 0 default
    {0, 7, 9}, // 1 on dark bg
    {0, 3, 4}, // 2 
    {0, 8, 0}, // 3 power
    {0, 9, 0}, // 4 power on dark bg
};

void Page_ScrollDown(u32 amount)
{
    sScrollIndex += amount;
    if (sScrollIndex > maxScroll)
        sScrollIndex = maxScroll;
    DebugPrintf("scroll %d", sScrollIndex);
}

void Page_ScrollUp(u32 amount)
{
    if (sScrollIndex > amount)
        sScrollIndex -= amount;
    else
        sScrollIndex = 0;
    DebugPrintf("scroll %d", sScrollIndex);
}


SummaryPage *Page_MoveInfos_Init(void)
{
    DebugPrintf("init page");
    sPage = AllocZeroed(sizeof(SummaryPage));
    sPage->index = 3;
    return sPage;
}

void Page_SetPokemon(struct Pokemon * pokemon) 
{
    DebugPrintf("set pokemon");
    MoveChanger_SetPokemon(pokemon);
    maxScroll = 7;
}

void Page_FillRects() {
    u32 i;
    for (i =0; i < 5; i++)
    {
        DrawBox(i, 0);
    }
}


void Page_PrintMoveTexts(u8 i)
{
    MoveData *moveData;
    const u8 *titleColor = sPrintMoveTextColors[0];
    const u8 *pwrColor = sPrintMoveTextColors[3];
    moveData = MoveChanger_GetMoveData(i + sScrollIndex);

    if (moveData == NULL)
        return;

    DebugPrintf("move id: %d window: %d", moveData->id, moveData->powerStr[2]);

    if (i + sScrollIndex >= 4) {
        // titleColor = sPrintMoveTextColors[1];
        pwrColor = sPrintMoveTextColors[4];
    }

    // Title
    AddTextPrinterParameterized3(sPage->windowIds[WIN_NAMES], FONT_NORMAL, 
        3, GetMoveNamePrinterYpos(i), titleColor, TEXT_SKIP_DRAW, 
        gMoveNames[moveData->id]);
    // Power
    AddTextPrinterParameterized3(sPage->windowIds[WIN_NAMES], FONT_SMALL,
        21, GetMovePpPrinterYpos(i)-1, pwrColor, TEXT_SKIP_DRAW, 
        gMoveNames[MOVE_PSYCHO_BOOST]);
    // Power Value
    AddTextPrinterParameterized3(sPage->windowIds[WIN_NAMES], FONT_NORMAL,
        45, GetMovePpPrinterYpos(i), titleColor, TEXT_SKIP_DRAW, 
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
        moveData = MoveChanger_GetMoveData(i + sScrollIndex);
        if (moveData == NULL)
            continue;
        if (moveData->id == MOVE_NONE)
            continue;
        
        BlitMenuInfoIcon(windowId, moveData->type + 1, 3, GetMoveNamePrinterYpos(i)-1);
        BlitMenuInfoIcon(windowId, moveData->category + 24, posXCategory, GetMoveCategoryPrinterYpos(i));
    }

    // if (sScrollIndex > 0) DrawRect5();
    // if (sScrollIndex > 1) DrawRect4();
    // if (sScrollIndex > 2) DrawRect3();
    // if (sScrollIndex > 3) DrawRect4();
    // if (sScrollIndex > 4) DrawRect5();
}