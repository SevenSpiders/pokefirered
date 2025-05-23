#include "main.h"
#include "bg.h"


// tile set indexes
/*
    A --- B
    |     |
    C --- D
*/

static void DrawBoxA(u32 y0, u8 offset) {
    const u8 x0 = 16;
    const u8 w = 14;
    const u8 pal = 4;
    const u8 bg = 2;
    bool8 white = offset == 0;

    const u32 tile_fill =   white ? 0x49 : 0x4c;
    const u32 tile_right =  white ? 0x4b : 0x4e;
    u32 tile_top =          white ? 0x3a : 0x3d;
    u32 tile_bot =          white ? 0x5a : 0x5d;
    u32 corner_a =          white ? 0x39 : 0x3c;
    u32 corner_b =          white ? 0x3b : 0x3e;
    u32 corner_c =          white ? 0x59 : 0x5c;
    u32 corner_d =          white ? 0x5b : 0x5e;

    if (y0 <= 6)
    {
        // implement different top tiles
        corner_a = white ? 0x119 : 0x11c;
        tile_top = white ? 0x11a : 0x11d;
        corner_b = white ? 0x11b : 0x11e;
    }

    if (y0 >= 15)
    {
        // implement different bottom tiles
        corner_c = white ? 0x12c : 0x12c;
        tile_bot = white ? 0x12d : 0x12d;
        corner_d = white ? 0x12e : 0x12e;
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

static void DrawBoxB(u32 y0, u8 offset) {
    const u8 x0 = 16;
    const u8 w = 14;
    const u8 pal = 4;
    const u8 bg = 2;
    bool8 white = offset == 0;

    const u16 corner_a =    white ? 0x69 : 0x6C;
    const u16 corner_b =    white ? 0x6b : 0x6e;
    const u16 CORNER_C =    white ? 0x89 : 0x8C;
    const u16 CORNER_D =    white ? 0x8b : 0x8e;
    const u16 TILE_FILL =   white ? 0x79 : 0x7c;
    const u16 TILE_BOTTOM = white ? 0x79 : 0x7c;
    const u16 TILE_TOP =    white ? 0x6a : 0x6d;
    const u16 TILE_RIGHT =  white ? 0x7b : 0x7e;

    FillBgTilemapBufferRect(bg, TILE_FILL, x0, y0 + 1, w, 2, pal);
    FillBgTilemapBufferRect(bg, TILE_TOP, x0 + 1, y0, w - 2, 1, pal);
    FillBgTilemapBufferRect(bg, TILE_RIGHT, 29, y0 + 1, 1, 1, pal);
    FillBgTilemapBufferRect(bg, corner_a, x0, y0, 1, 1, pal);
    FillBgTilemapBufferRect(bg, corner_b, 29, y0, 1, 1, pal);
    FillBgTilemapBufferRect(bg, CORNER_C, x0, y0 + 2, 1, 1, pal);
    FillBgTilemapBufferRect(bg, CORNER_D, 29, y0 + 2, 1, 1, pal);
}

void DrawBox(u8 index, u8 mode, u32 direction)
{
    
    switch (index)
    {
    case 0:
        if (direction == 0) DrawBoxA(2, mode);
        else if (direction == 1) DrawBoxB(2, mode);
        break;
    case 1:
        if (direction == 0) DrawBoxB(6, mode);
        else if (direction == 1) DrawBoxA(6, mode);
        break;
    case 2:
        if (direction == 0) DrawBoxA(9, mode);
        else if (direction == 1) DrawBoxB(9, mode);
        break;
    case 3:
        if (direction == 0) DrawBoxB(13, mode);
        else if (direction == 1) DrawBoxA(13, mode);
        break;
    case 4:
        if (direction == 0) DrawBoxA(16, mode);
        else if (direction == 1) DrawBoxB(16, mode);
        break;
    
    default:
        break;
    }
}
