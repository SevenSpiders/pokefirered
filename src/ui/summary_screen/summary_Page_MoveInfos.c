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
extern const u8 *const gMoveDescriptionPointers[];

static u8 state;
static SummaryPage *sPage;
static u32 sScrollIndex;
static u32 sCursorIndex;
static u32 sSelectedIndex;
static u32 maxScroll;
static bool8 isScrolling, isSwapping;

#define NUM_BOXES           5
#define BOX_STATE_NORMAL    0
#define BOX_STATE_DARK      1
#define BOX_STATE_INVISIBLE 2

typedef struct {
    u8 index;
    u16 type;
    u32 state; // BOX_STATE_X
    bool8 dirty;
} BoxState;

static BoxState boxes[NUM_BOXES];
static bool8 Page_DrawBox(u32 i, u32 direction);

// 0 transparent, 1 beige, 2 light yellow, 3 orange, 4 yellow, 5 red, 6 ligth orange, 7 black, 8 light grey
// new 9 white -> edit summary_screen\text_moves.pal
static const u8 sPrintMoveTextColors[][3] = {
    {0, 7, 8}, // 0 default
    {0, 7, 9}, // 1 on dark bg
    {0, 3, 4}, // 2 
    {0, 8, 0}, // 3 power
    {0, 9, 0}, // 4 power on dark bg
};
static const u8 sLevelNickTextColors[][3] =
{
    {0, 14, 10},        // 0
    {0, 1, 2},          // 1 page header (white)
    {0, 9, 8},          // 2 male symbol color
    {0, 5, 4},          // 3 female symbol color
    {0, 2, 3},          // 4 orange unused?
    {0, 11, 10},        // 5 beige unused?
    {0, 14, 10},          // 6 Power
    {0, 7, 10},          // 7 Accuracy
};

static void SetBoxState(u32 i, u32 state)
{
    // DebugPrintf("set box %d state %d -> state %d", i, boxes[i].state, state);
    if (boxes[i].state == state)
        return;
    
    boxes[i].state = state;
    boxes[i].dirty = TRUE;
}

static void UpdateBoxes()
{
    u32 i, state_i;

    for(i=0; i < 4; i++)
    { 
        state_i = (sScrollIndex >= 4-i) ? BOX_STATE_DARK : BOX_STATE_NORMAL;
        SetBoxState ( i, state_i);
    }

    // box 5
    state_i = isScrolling ? BOX_STATE_DARK : BOX_STATE_INVISIBLE;
    SetBoxState( 4, state_i);
}

void Page_ScrollDown(u8 i)
{
    sScrollIndex += 1;
    if (sScrollIndex > maxScroll)
        sScrollIndex = maxScroll;
    
    UpdateBoxes();
}

void Page_ScrollUp(u8 i)
{
    if (sScrollIndex > 1)
        sScrollIndex -= 1;
    else
        sScrollIndex = 0;
    
    UpdateBoxes();
}

void Page_SetScrolling(bool8 b)
{
    isScrolling = b;
    sScrollIndex = 0;
    UpdateBoxes();
}

void Page_SetSwapping(bool8 b)
{
    isSwapping = b;
    sSelectedIndex = sScrollIndex + sCursorIndex;
}

void Page_SetCursor(u32 index)
{
    sCursorIndex = index;
}

void Page_SwapMoves()
{
    DebugPrintf("swap moves");
}

void Page_PrintMoveDescription()
{
    u32 WindowId = 4;

    MoveData *moveData;
    if (sCursorIndex >= 5)
        return;

    moveData = MoveChanger_GetMoveData(sCursorIndex + sScrollIndex);

    if (moveData == NULL || moveData->id == 0)
        return;

    // DebugPrintf("print description %d", moveData->id);

    // Power
    AddTextPrinterParameterized3(WindowId, FONT_NORMAL, 55, 1,
                                    sLevelNickTextColors[6], TEXT_SKIP_DRAW,
                                    moveData->powerStr);
    // Accuracy
    AddTextPrinterParameterized3(WindowId, FONT_NORMAL, 80, 1,
                                    sLevelNickTextColors[7], TEXT_SKIP_DRAW,
                                    moveData->accuracyStr);
    // Description
    AddTextPrinterParameterized4(WindowId, FONT_SMALL, 7, 17, 0, 0,
                                    sLevelNickTextColors[0], TEXT_SKIP_DRAW,
                                    gMoveDescriptionPointers[moveData->id]);
}


SummaryPage *Page_MoveInfos_Init(void)
{
    u32 i;
    DebugPrintf("init page");
    for(i=0; i<5; i++)
    {
        boxes[0].index = i;
    }
    boxes[4].state = BOX_STATE_INVISIBLE;

    sPage = AllocZeroed(sizeof(SummaryPage));
    sPage->index = 3;
    return sPage;
}

void Page_SetPokemon(struct Pokemon * pokemon) 
{
    maxScroll = MoveChanger_SetPokemon(pokemon) - 3;
    DebugPrintf("set pokemon -> moves %d", maxScroll);
}

void Page_DrawBoxes() {
    u32 i;
    for (i=0; i< 5; i++)
    {
        Page_DrawBox(i, 0);
    }
}

void Page_DrawTweens(u8 direction)
{
    u32 i;
    for (i=0; i< 5; i++)
    {
        Page_DrawBox(i, direction);
    }
}

static bool8 Page_DrawBox(u32 i, u32 direction)
{
    if (boxes[i].dirty == TRUE)
    {
        DrawBox(i, boxes[i].state, direction);
        DebugPrintf("print box -> state %d", boxes[i].state);
        boxes[i].dirty = FALSE;
        return TRUE;
    }
    else return FALSE;
}

static void PrintMoveText(u8 i)
{
    MoveData *moveData;
    const u8 *titleColor = sPrintMoveTextColors[0];
    const u8 *pwrColor = sPrintMoveTextColors[3];

    

    if (i == 4 && isScrolling == FALSE)
        return;

    moveData = MoveChanger_GetMoveData(i + sScrollIndex);

    if (moveData == NULL || moveData->id == 0)
        return;

    // DebugPrintf("move id: %d window: %d", moveData->id, moveData->powerStr[2]);

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

void Page_PrintMoveTexts()
{
    u32 i;
    bool8 dirty;

    for (i=0; i< 5; i++)
    {
        // dirty |= Page_DrawBox(i);
        PrintMoveText(i);
    }
    // if (dirty) 
    //     CopyWindowToVram(2, COPYWIN_MAP); // backgrounds
}


void Page_DrawMoveIcons(void)
{
    u32 i;
    u32 iMax = 4;
    u32 posXCategory = 4;
    u32 type, category;
    u32 windowId;
    MoveData *moveData;
    if (isScrolling == TRUE)
        iMax = 5;

    windowId = sPage->windowIds[WIN_MOVES];
    
    FillWindowPixelBuffer(windowId, 0);
    
    for (i = 0; i < iMax; i++)
    {
        // DebugPrintf("draw move icons in window %d", windowId);
        moveData = MoveChanger_GetMoveData(i + sScrollIndex);
        if (moveData == NULL)
            continue;
        if (moveData->id == MOVE_NONE)
            continue;
        
        BlitMenuInfoIcon(windowId, moveData->type + 1, 3, GetMoveNamePrinterYpos(i)-1);
        BlitMenuInfoIcon(windowId, moveData->category + 24, posXCategory, GetMoveCategoryPrinterYpos(i));
    }
}