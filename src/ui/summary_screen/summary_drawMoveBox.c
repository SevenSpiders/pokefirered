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
    const u32 tile_fill = 0x7c -offset;
    const u32 tile_right = 0x4e -offset;
    u32 tile_top = 0x3d -offset;
    u32 tile_bot = 0x5d -offset;
    u32 corner_a = 0x3c -offset;
    u32 corner_b = 0x3e -offset;
    u32 corner_c = 0x5c -offset;
    u32 corner_d = 0x5e -offset;

    if (y0 <= 6)
    {
        // implement different top tiles
        corner_a = 0x11c;
        tile_top = 0x11d;
        corner_b = 0x11e;
    }

    if (y0 >= 15)
    {
        // implement different bottom tiles
        corner_c = 0x12c;
        tile_bot = 0x12d;
        corner_d = 0x12e;
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

    const u16 corner_a = 0x6C - offset;
    const u16 corner_b = (corner_a + 2) -offset;
    const u16 CORNER_C = 0x8C - offset;
    const u16 CORNER_D = (CORNER_C + 0x2) -offset;
    const u16 TILE_FILL = 0x7C - offset;
    const u16 TILE_BOTTOM = (CORNER_C + 0x1) -offset;
    const u16 TILE_TOP = (corner_a + 1) -offset;
    const u16 TILE_RIGHT = (TILE_FILL + 0x2)-offset;

    FillBgTilemapBufferRect(bg, TILE_FILL, x0, y0 + 1, w, 2, pal);
    FillBgTilemapBufferRect(bg, TILE_TOP, x0 + 1, y0, w - 2, 1, pal);
    FillBgTilemapBufferRect(bg, TILE_RIGHT, 29, y0 + 1, 1, 1, pal);
    FillBgTilemapBufferRect(bg, corner_a, x0, y0, 1, 1, pal);
    FillBgTilemapBufferRect(bg, corner_b, 29, y0, 1, 1, pal);
    FillBgTilemapBufferRect(bg, CORNER_C, x0, y0 + 2, 1, 1, pal);
    FillBgTilemapBufferRect(bg, CORNER_D, 29, y0 + 2, 1, 1, pal);
}

void DrawBox(u8 index, u8 mode)
{
    u8 offset = 0;//mode > 0 ? 3 : 0;
    // mode -> white, tween
    switch (index)
    {
    case 0:
        DrawBoxA(2, offset);
        break;
    case 1:
        DrawBoxB(6, offset);
        break;
    case 2:
        DrawBoxA(9, offset);
        break;
    case 3:
        DrawBoxB(13, offset);
        break;
    case 4:
        DrawBoxA(16, offset);
        break;
    
    default:
        break;
    }
}
