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

extern const u8 gText_PokeSum_Power;


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

void Page_SetPokemon(struct Pokemon * pokemon) 
{
    DebugPrintf("set pokemon");
    MoveChanger_SetPokemon(pokemon);
}

static void DrawRect1(u32 y0) {
    u8 x0 = 16;
    u8 w = 14;
    u8 pal = 4;
    u32 tile_fill = 0x7c;
    u32 tile_right = 0x4e;
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

    FillBgTilemapBufferRect(2, tile_fill, x0, y0+1, w-1, 2, pal);
    FillBgTilemapBufferRect(2, tile_bot, x0+1, y0+3, w-2, 1, pal);
    FillBgTilemapBufferRect(2, tile_top, x0+1, y0, w-2, 1, pal);
    FillBgTilemapBufferRect(2, tile_right, 29, y0+1, 1, 2, pal);
    FillBgTilemapBufferRect(2, corner_a, x0, y0, 1, 1, pal); // pal 0: top violet, 1 top grey, 2 violet
    FillBgTilemapBufferRect(2, corner_b, 29, y0, 1, 1, pal);
    FillBgTilemapBufferRect(2, corner_c, x0, y0+3, 1, 1, pal);
    FillBgTilemapBufferRect(2, corner_d, 29, y0+3, 1, 1, pal);
}

// tile set indexes
/*
    A --- B
    |     |
    C --- D
*/
#define CORNER_A    (0X6C) // TOP LEFT  -> +1: inv, 0: top, -1: right corner
#define CORNER_B    (CORNER_A + 2)
#define CORNER_C    (0X8C)
#define CORNER_D    (CORNER_C + 0x2)
#define TILE_FILL   (0x7c)
#define TILE_BOTTOM (CORNER_C + 0x1)
#define TILE_TOP    (CORNER_A + 1)
#define TILE_RIGHT  (TILE_FILL + 0x2)

static void DrawRect2(u32 y0) {
    u8 x0 = 16;
    u8 w = 14;
    u8 pal = 4;
    FillBgTilemapBufferRect(2, TILE_FILL, x0, y0+1, w, 2, pal);
    FillBgTilemapBufferRect(2, TILE_TOP, x0+1, y0, w-2, 1, pal);
    FillBgTilemapBufferRect(2, TILE_RIGHT, 29, y0+1, 1, 1, pal);
    FillBgTilemapBufferRect(2, CORNER_A, x0, y0, 1, 1, pal); // pal 0: top violet, 1 top grey, 2 violet
    FillBgTilemapBufferRect(2, CORNER_B, 29, y0, 1, 1, pal);
    FillBgTilemapBufferRect(2, CORNER_C, x0, y0+2, 1, 1, pal);
    FillBgTilemapBufferRect(2, CORNER_D, 29, y0+2, 1, 1, pal);
}

void Page_FillRects() {
   
    DebugPrintf("_____________fill rect");
    
    DrawRect1(2); // move 1
    DrawRect2(6); // move 2
    DrawRect1(9); // move 3
    DrawRect2(13); // move 4
    DrawRect1(16); // move 5
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