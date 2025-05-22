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

static void DrawRectA(u32 y0) {
    const u8 x0 = 16;
    const u8 w = 14;
    const u8 pal = 4;
    const u8 bg = 2;
    const u32 tile_fill = 0x7c;
    const u32 tile_right = 0x4e;
    u32 tile_top = 0x3d;
    u32 tile_bot = 0x5d;
    u32 corner_a = 0x3c;
    u32 corner_b = 0x3e;
    u32 corner_c = 0x5c;
    u32 corner_d = 0x5e;

    if (y0 <= 6)
    {
        // implement different top tiles
        corner_a = 0x119;
        tile_top = 0x11a;
        corner_b = 0x11b;
    }

    if (y0 >= 15)
    {
        // implement different bottom tiles
        corner_c = 0x11c;
        tile_bot = 0x11d;
        corner_d = 0x11e;
    }

    FillBgTilemapBufferRect(bg, tile_fill, x0, y0+1, w-1, 2, pal);
    FillBgTilemapBufferRect(bg, tile_bot, x0+1, y0+3, w-2, 1, pal);
    FillBgTilemapBufferRect(bg, tile_top, x0+1, y0, w-2, 1, pal);
    FillBgTilemapBufferRect(bg, tile_right, 29, y0+1, 1, 2, pal);
    FillBgTilemapBufferRect(bg, corner_a, x0, y0, 1, 1, pal); // pal 0: top violet, 1 top grey, 2 violet
    FillBgTilemapBufferRect(bg, corner_b, 29, y0, 1, 1, pal);
    FillBgTilemapBufferRect(bg, corner_c, x0, y0+3, 1, 1, pal);
    FillBgTilemapBufferRect(bg, corner_d, 29, y0+3, 1, 1, pal);
}

// tile set indexes
/*
    A --- B
    |     |
    C --- D
*/
static void DrawRectB(u32 y0) {
    const u8 x0 = 16;
    const u8 w = 14;
    const u8 pal = 4;
    const u8 bg = 2;

    const u16 corner_a = 0x6C; // TOP LEFT
    const u16 CORNER_B = (corner_a + 2);
    const u16 CORNER_C = 0x8C;
    const u16 CORNER_D = (CORNER_C + 0x2);
    const u16 TILE_FILL = 0x7C;
    const u16 TILE_BOTTOM = (CORNER_C + 0x1);
    const u16 TILE_TOP = (corner_a + 1);
    const u16 TILE_RIGHT = (TILE_FILL + 0x2);

    FillBgTilemapBufferRect(bg, TILE_FILL, x0, y0 + 1, w, 2, pal);
    FillBgTilemapBufferRect(bg, TILE_TOP, x0 + 1, y0, w - 2, 1, pal);
    FillBgTilemapBufferRect(bg, TILE_RIGHT, 29, y0 + 1, 1, 1, pal);
    FillBgTilemapBufferRect(bg, corner_a, x0, y0, 1, 1, pal);
    FillBgTilemapBufferRect(bg, CORNER_B, 29, y0, 1, 1, pal);
    FillBgTilemapBufferRect(bg, CORNER_C, x0, y0 + 2, 1, 1, pal);
    FillBgTilemapBufferRect(bg, CORNER_D, 29, y0 + 2, 1, 1, pal);
}

static void DrawRect1()
{
    DrawRectA(2);
}

static void DrawRect2()
{
    DrawRectB(6);
}

static void DrawRect3()
{
    DrawRectA(9);
}

static void DrawRect4()
{
    DrawRectB(13);
}

static void DrawRect5()
{
    DrawRectA(16);
}

void Page_FillRects() {
    DrawRect1();
    DrawRect2();
    DrawRect3();
    DrawRect4();
    DrawRect5();
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