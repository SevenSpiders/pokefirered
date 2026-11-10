#include "global.h"
#include "gflib.h"
#include "new_menu_helpers.h"
#include "minigames/pikachu_beach_internal.h"

#define WS_NEXT  0
#define WS_RESET 1
#define WS_STAY  2

#define WNEXT(b, c, d)  {b, c, d, WS_NEXT}
#define WRESET(b, c, d) {b, c, d, WS_RESET}
#define WSTAY(b, c, d)  {b, c, d, WS_STAY}
#define WDEF_NEXT       WNEXT(0x74, 0x74, 0)
#define WDEF_RESET      WRESET(0x74, 0x74, 0)
#define WDEF_STAY       WSTAY(0x74, 0x74, 0)

struct WaveStep
{
    u8 heightB;
    u8 heightC;
    u8 dataId;
    u8 op;
};

static const u8 sWaveTileLookup[][4] = {
    {0x00, 0x00, 0x00, 0x00},
    {0x0B, 0x0B, 0x0B, 0x0B},
    {0x0B, 0x02, 0x02, 0x06},
    {0x03, 0x0B, 0x07, 0x03},
    {0x06, 0x06, 0x06, 0x06},
    {0x07, 0x07, 0x07, 0x07},
    {0x06, 0x04, 0x04, 0x08},
    {0x05, 0x07, 0x08, 0x05},
    {0x0B, 0x0B, 0x11, 0x12},
    {0x0B, 0x0B, 0x13, 0x03},
    {0x14, 0x12, 0x04, 0x08},
    {0x13, 0x07, 0x08, 0x05},
    {0x06, 0x14, 0x06, 0x14},
    {0x13, 0x07, 0x13, 0x07},
    {0x08, 0x08, 0x08, 0x08},
    {0x14, 0x12, 0x14, 0x12},
    {0x0B, 0x11, 0x02, 0x14},
    {0x06, 0x14, 0x06, 0x14},
    {0x0C, 0x0C, 0x0D, 0x0D},
    {0x0D, 0x0D, 0x0D, 0x0D},
    {0x0E, 0x0F, 0x10, 0x0B},
    {0x12, 0x13, 0x12, 0x13},
};

static const u8 sWaveData[][8] = {
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x04, 0x06},
    {0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x06, 0x0E},
    {0x00, 0x00, 0x00, 0x10, 0x11, 0x06, 0x0E, 0x0E},
    {0x00, 0x00, 0x00, 0x15, 0x15, 0x0E, 0x0E, 0x0E},
    {0x00, 0x00, 0x00, 0x03, 0x05, 0x07, 0x0E, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x03, 0x05, 0x07, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x05, 0x07},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x04, 0x06},
    {0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x06, 0x0E},
    {0x00, 0x00, 0x00, 0x08, 0x0F, 0x0A, 0x0E, 0x0E},
    {0x00, 0x00, 0x00, 0x09, 0x0D, 0x0B, 0x0E, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x03, 0x05, 0x07, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x05, 0x07},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x04, 0x06},
    {0x00, 0x00, 0x00, 0x01, 0x10, 0x11, 0x06, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x15, 0x15, 0x0E, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x03, 0x05, 0x07, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x05, 0x07},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x04, 0x06},
    {0x00, 0x00, 0x00, 0x01, 0x08, 0x0F, 0x0A, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x09, 0x0D, 0x0B, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x05, 0x07},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x10, 0x11, 0x06},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x15, 0x15, 0x0E},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x05, 0x07},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x08, 0x0F, 0x0A},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x09, 0x0D, 0x0B},
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01},
    {0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14},
    {0x00, 0x00, 0x00, 0x12, 0x13, 0x13, 0x13, 0x13},
};

static const u8 sWaveStartTable[] = {0x01, 0x0E, 0x1A, 0x29, 0x32, 0x40, 0x4D, 0x5C};

static const struct WaveStep sWaveSteps[] = {
    WDEF_STAY,
    WNEXT(0x74, 0x6C, 19),
    WNEXT(0x64, 0x5C, 20),
    WNEXT(0x5C, 0x5C, 21),
    WNEXT(0x64, 0x6C, 22),
    WDEF_NEXT,
    WNEXT(0x74, 0x6C, 23),
    WNEXT(0x64, 0x64, 24),
    WNEXT(0x64, 0x6C, 25),
    WDEF_NEXT, WDEF_NEXT, WDEF_NEXT, WDEF_NEXT,
    WDEF_RESET,
    WNEXT(0x74, 0x6C, 8),
    WNEXT(0x64, 0x5C, 9),
    WNEXT(0x54, 0x4C, 10),
    WNEXT(0x4C, 0x4C, 11),
    WNEXT(0x54, 0x5C, 12),
    WNEXT(0x64, 0x6C, 13),
    WDEF_NEXT, WDEF_NEXT, WDEF_NEXT, WDEF_NEXT, WDEF_NEXT,
    WDEF_RESET,
    WNEXT(0x74, 0x6C, 14),
    WNEXT(0x64, 0x5C, 15),
    WNEXT(0x54, 0x54, 16),
    WNEXT(0x54, 0x5C, 17),
    WNEXT(0x64, 0x6C, 18),
    WNEXT(0x74, 0x6C, 14),
    WNEXT(0x64, 0x5C, 15),
    WNEXT(0x54, 0x54, 16),
    WNEXT(0x54, 0x5C, 17),
    WNEXT(0x64, 0x6C, 18),
    WDEF_NEXT, WDEF_NEXT, WDEF_NEXT, WDEF_NEXT,
    WDEF_RESET,
    WNEXT(0x74, 0x6C, 19),
    WNEXT(0x64, 0x5C, 20),
    WNEXT(0x5C, 0x5C, 21),
    WNEXT(0x64, 0x6C, 22),
    WDEF_NEXT, WDEF_NEXT, WDEF_NEXT, WDEF_NEXT,
    WDEF_RESET,
    WNEXT(0x74, 0x6C, 23),
    WNEXT(0x64, 0x64, 24),
    WNEXT(0x64, 0x6C, 25),
    WNEXT(0x74, 0x6C, 23),
    WNEXT(0x64, 0x64, 24),
    WNEXT(0x64, 0x6C, 25),
    WNEXT(0x74, 0x6C, 23),
    WNEXT(0x64, 0x64, 24),
    WNEXT(0x64, 0x6C, 25),
    WDEF_NEXT, WDEF_NEXT, WDEF_NEXT, WDEF_NEXT,
    WDEF_RESET,
    WNEXT(0x74, 0x6C, 26),
    WNEXT(0x6C, 0x6C, 27),
    WNEXT(0x74, 0x6C, 14),
    WNEXT(0x64, 0x5C, 15),
    WNEXT(0x54, 0x54, 16),
    WNEXT(0x54, 0x5C, 17),
    WNEXT(0x64, 0x6C, 18),
    WNEXT(0x74, 0x6C, 26),
    WNEXT(0x6C, 0x6C, 27),
    WDEF_NEXT, WDEF_NEXT, WDEF_NEXT,
    WDEF_RESET,
    WNEXT(0x74, 0x6C, 8),
    WNEXT(0x64, 0x5C, 9),
    WNEXT(0x54, 0x4C, 10),
    WNEXT(0x4C, 0x4C, 11),
    WNEXT(0x54, 0x5C, 12),
    WNEXT(0x64, 0x6C, 13),
    WDEF_NEXT,
    WNEXT(0x74, 0x6C, 26),
    WNEXT(0x6C, 0x6C, 27),
    WNEXT(0x74, 0x6C, 26),
    WNEXT(0x6C, 0x6C, 27),
    WDEF_NEXT, WDEF_NEXT, WDEF_NEXT,
    WDEF_RESET,
    WNEXT(0x74, 0x6C, 14),
    WNEXT(0x64, 0x5C, 15),
    WNEXT(0x54, 0x54, 16),
    WNEXT(0x54, 0x5C, 17),
    WNEXT(0x64, 0x6C, 18),
    WNEXT(0x74, 0x6C, 19),
    WNEXT(0x64, 0x5C, 20),
    WNEXT(0x5C, 0x5C, 21),
    WNEXT(0x64, 0x6C, 22),
    WDEF_NEXT, WDEF_NEXT, WDEF_NEXT, WDEF_NEXT,
    WDEF_RESET,
    WNEXT(0x74, 0x6C, 1),
    WNEXT(0x64, 0x5C, 2),
    WNEXT(0x54, 0x4C, 3),
    WNEXT(0x44, 0x44, 4),
    WNEXT(0x44, 0x4C, 5),
    WNEXT(0x54, 0x5C, 6),
    WNEXT(0x64, 0x6C, 7),
    WDEF_STAY,
    WDEF_NEXT,
    WNEXT(0x74, 0x74, 29),
    WNEXT(0x74, 0x74, 30),
    WNEXT(0x74, 0x74, 30),
    WNEXT(0x74, 0x74, 30),
    WNEXT(0x74, 0x74, 30),
    WNEXT(0x74, 0x74, 30),
    WNEXT(0x74, 0x74, 30),
    WNEXT(0x74, 0x74, 30),
    WRESET(0x74, 0x74, 30),
};

static void WriteWaveColumns(u8 destCol, u8 heightB, u8 heightC, const u8 *waveData)
{
    struct SurfGame *game = GetSurfGame();
    struct SurfMinigameSetupTaskData *ptr = GetSurfMinigameSetupTaskDataPtr();
    u16 *map = (u16 *)ptr->bg2TilemapBuffer;
    s32 i, row, col0, col1;
    const u8 *meta;
    u8 id;

    (void)heightB;
    (void)heightC;

    col0 = destCol & 31;
    col1 = (destCol + 1) & 31;

    for (row = 0; row < 2; row++)
    {
        map[row * 32 + col0] = 0;
        map[row * 32 + col1] = 0;
        game->gbTiles[col0][row] = 0;
        game->gbTiles[col1][row] = 0;
    }

    for (i = 0; i < 8; i++)
    {
        id = waveData[i];
        if (id >= ARRAY_COUNT(sWaveTileLookup))
            id = 0;
        meta = sWaveTileLookup[id];
        row = 2 + i * 2;

        map[row * 32 + col0] = meta[0];
        map[row * 32 + col1] = meta[1];
        map[(row + 1) * 32 + col0] = meta[2];
        map[(row + 1) * 32 + col1] = meta[3];

        game->gbTiles[col0][row] = meta[0];
        game->gbTiles[col1][row] = meta[1];
        game->gbTiles[col0][row + 1] = meta[2];
        game->gbTiles[col1][row + 1] = meta[3];
    }

    for (row = 18; row < SURF_WAVE_ROWS; row++)
    {
        map[row * 32 + col0] = 0x0B;
        map[row * 32 + col1] = 0x0B;
        game->gbTiles[col0][row] = 0x0B;
        game->gbTiles[col1][row] = 0x0B;
    }

    ScheduleBgCopyTilemapToVram(SURF_BG_OCEAN);
}

static void GetCurrentWave(u8 *heightB, u8 *heightC, const u8 **waveData)
{
    struct SurfGame *game = GetSurfGame();
    const struct WaveStep *step;
    u8 fn = game->waveFunctionNumber;

    if (fn == 0)
    {
        if (game->distance[0] == SURF_DISTANCE_SHORE)
            game->waveFunctionNumber = 0x6A;
        else if (game->distance[0] < SURF_DISTANCE_SHORE && game->waveType != 0)
            game->waveFunctionNumber = sWaveStartTable[(game->waveType - 1) & 7];
        *heightB = SURF_WATERLINE_DEFAULT;
        *heightC = SURF_WATERLINE_DEFAULT;
        *waveData = sWaveData[0];
        return;
    }

    if (fn >= ARRAY_COUNT(sWaveSteps))
        fn = 0;

    step = &sWaveSteps[fn];
    *heightB = step->heightB;
    *heightC = step->heightC;
    *waveData = sWaveData[step->dataId];

    if (step->op == WS_NEXT)
        game->waveFunctionNumber++;
    else if (step->op == WS_RESET)
        game->waveFunctionNumber = 0;
}

void SurfMinigame_InitOcean(void)
{
    struct SurfGame *game = GetSurfGame();
    s32 i;
    u8 savedFn = game->waveFunctionNumber;
    u8 savedDist = game->distance[0];
    u8 savedType = game->waveType;

    game->waveFunctionNumber = 0;
    game->distance[0] = 0;
    game->waveType = 0;
    game->xOffset = 0xA0;

    for (i = 0; i < SURF_WAVE_COLS; i += 2)
        WriteWaveColumns(i, SURF_WATERLINE_DEFAULT, SURF_WATERLINE_DEFAULT, sWaveData[0]);

    for (i = 0; i < SURF_WAVE_COLS; i++)
        game->waveHeight[i] = SURF_WATERLINE_DEFAULT;

    game->waveY = SURF_WATERLINE_DEFAULT;
    game->waveFunctionNumber = savedFn;
    game->distance[0] = savedDist;
    game->waveType = savedType;
    game->scxHi = 0;
    game->scx2 = 0;
}

void SurfMinigame_GenerateBGMap(void)
{
    struct SurfGame *game = GetSurfGame();
    u8 scxPixel = game->scx >> 8;
    u8 aligned;
    u8 heightB, heightC;
    const u8 *waveData;
    s32 i;
    u8 destCol;

    if (scxPixel == game->scx2)
        return;
    game->scx2 = scxPixel;

    aligned = scxPixel & 0xF0;
    if (aligned == game->scxHi)
        return;
    game->scxHi = aligned;

    GetCurrentWave(&heightB, &heightC, &waveData);

    for (i = 0; i < SURF_WAVE_COLS - 2; i++)
        game->waveHeight[i] = game->waveHeight[i + 2];
    game->waveHeight[SURF_WAVE_COLS - 2] = heightB;
    game->waveHeight[SURF_WAVE_COLS - 1] = heightC;

    destCol = ((scxPixel + game->xOffset) & 0xF0) >> 3;
    WriteWaveColumns(destCol, heightB, heightC, waveData);
}

void SurfMinigame_ScrollAndGenerateBG(u16 add)
{
    struct SurfGame *game = GetSurfGame();

    game->xOffset = (add == SURF_SCROLL_GOAL) ? 0xA0 : game->xOffset;
    if (game->xOffset == 0)
        game->xOffset = 0xA0;
    game->scx += add;
    SurfMinigame_GenerateBGMap();
}

void SurfMinigame_ReadBGMapBuffer(void)
{
    struct SurfGame *game = GetSurfGame();
    u8 scxPixel = game->scx >> 8;
    u8 col = (scxPixel + SURF_PIKA_X) >> 3;
    u8 row = (game->waveY + SURF_Y_OFFSET) >> 3;

    col &= 31;
    if (row >= SURF_WAVE_ROWS)
        row = SURF_WAVE_ROWS - 1;
    game->bgTileUnderBoard = game->gbTiles[col][row];
}

void SurfMinigame_SetPikachuHeight(void)
{
    struct SurfGame *game = GetSurfGame();
    u8 scxPixel = game->scx >> 8;
    u8 idx = (scxPixel & 8) ? 14 : 13;
    u8 height = game->waveHeight[idx];
    u8 tile = game->bgTileUnderBoard;

    if (tile == 0x06 || tile == 0x14)
        game->waveY = height - (scxPixel & 7);
    else if (tile == 0x07)
        game->waveY = height + (scxPixel & 7);
    else
        game->waveY = height;
}
