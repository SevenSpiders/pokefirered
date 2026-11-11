#include "global.h"
#include "gflib.h"
#include "decompress.h"
#include "task.h"
#include "menu.h"
#include "new_menu_helpers.h"
#include "text_window.h"
#include "scanline_effect.h"
#include "constants/rgb.h"
#include "minigames/pikachu_beach_internal.h"

static EWRAM_DATA struct SurfMinigameGfxManager *sSurfMinigameGfxManager = NULL;

static const u16 sOceanPal[] = {
    RGB(31, 31, 31),
    RGB(8, 20, 31),
    RGB(12, 24, 31),
    RGB(20, 28, 31),
    RGB(31, 31, 31),
    RGB(8, 20, 31),
    RGB(12, 24, 31),
    RGB(20, 28, 31),
    RGB(31, 31, 31),
    RGB(8, 20, 31),
    RGB(12, 24, 31),
    RGB(20, 28, 31),
    RGB(31, 31, 31),
    RGB(8, 20, 31),
    RGB(12, 24, 31),
    RGB(20, 28, 31),
};
static const u32 sBg_Tiles[] = INCBIN_U32("graphics/pikachu_beach/surfing_pikachu_ui.4bpp.lz");
static const u16 sPikachuSurf_Pal[] = INCBIN_U16("graphics/pikachu_beach/spritesheet_pikachu_surf.gbapal");
static const u32 sPikachuSurfTiles[] = INCBIN_U32("graphics/pikachu_beach/spritesheet_pikachu_surf.4bpp.lz");
static const u16 sPikachuPaddle_Pal[] = INCBIN_U16("graphics/pikachu_beach/spritesheet_pikachu_paddle.gbapal");
static const u32 sPikachuPaddleTiles[] = INCBIN_U32("graphics/pikachu_beach/spritesheet_pikachu_paddle.4bpp.lz");

static const struct CompressedSpriteSheet sSpriteSheets[] = {
    {.data = sPikachuPaddleTiles, .size = 512 * 4, .tag = GFXTAG_PIKACHU_PADDLE},
    {.data = sPikachuSurfTiles, .size = 512 * 18, .tag = GFXTAG_PIKACHU_SURF},
};

static const struct SpritePalette sSpritePalettes[] = {
    {.data = sPikachuPaddle_Pal, .tag = PALTAG_PIKACHU_PADDLE},
    {.data = sPikachuSurf_Pal, .tag = PALTAG_PIKACHU_SURF},
};

static const struct BgTemplate sBgTemplates[] = {
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {
        .bg = 3,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    },
};

static const struct WindowTemplate sWindowTemplates[] = {
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1
    },
    {
        .bg = 0,
        .tilemapLeft = 4,
        .tilemapTop = 3,
        .width = 22,
        .height = 10,
        .paletteNum = 15,
        .baseBlock = 61
    },
    DUMMY_WIN_TEMPLATE
};

static const u8 sTextColors[] = {1, 2, 3};
static const u8 sText_Hp[] = _("HP ");
static const u8 sText_Rad[] = _("  RAD ");
static const u8 sText_Spacer[] = _("  ");
static const u8 sText_Pts[] = _(" PTS");
static const u8 sText_HiScore[] = _("HI-SCORE!!");

static const struct OamData sOam_Progress = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .size = ST_OAM_SIZE_0,
    .priority = 0,
};

static const union AnimCmd sAnim_Progress[] = {
    ANIMCMD_FRAME(0, 0xFF),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Progress[] = {
    sAnim_Progress
};

static const struct SpriteTemplate sSpriteTemplate_Progress = {
    .tileTag = GFXTAG_PIKACHU_SURF,
    .paletteTag = PALTAG_PIKACHU_SURF,
    .oam = &sOam_Progress,
    .anims = sAnimTable_Progress,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static void VBlankCB_SurfMinigame(void);
static bool32 LoadSpriteGraphicsAndAllocateManager(void);
static void SetupSkyBg(void);

bool8 SurfTask_InitGraphics(u8 *state, SurfMinigameSetupTaskData *ptr)
{
    s32 spriteId;
    struct SurfGame *game = GetSurfGame();

    switch (*state)
    {
    case 0:
        BlendPalettes(PALETTES_ALL, 16, RGB_WHITE);
        (*state)++;
        break;
    case 1:
        SetVBlankCallback(NULL);
        ScanlineEffect_Stop();
        ScanlineEffect_Clear();
        ResetSpriteData();
        FreeAllSpritePalettes();
        RequestDma3Fill(0, (void *)OAM, OAM_SIZE, DMA3_32BIT);
        RequestDma3Fill(0, (void *)VRAM, 0x20, DMA3_32BIT);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgPositions();
        ResetBgsAndClearDma3BusyFlags(FALSE);
        InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
        InitWindows(sWindowTemplates);
        DeactivateAllTextPrinters();

        SetBgTilemapBuffer(0, ptr->bg0TilemapBuffer);
        SetBgTilemapBuffer(1, ptr->bg1TilemapBuffer);
        SetBgTilemapBuffer(2, ptr->bg2TilemapBuffer);
        SetBgTilemapBuffer(3, ptr->bg3TilemapBuffer);

        LoadPalette(sOceanPal, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        Menu_LoadStdPalAt(BG_PLTT_ID(15));
        DecompressAndCopyTileDataToVram(SURF_BG_OCEAN, sBg_Tiles, 0, 0, 0);

        FillBgTilemapBufferRect_Palette0(SURF_BG_HUD, 0, 0, 0, 32, 32);
        FillBgTilemapBufferRect_Palette0(1, 0, 0, 0, 32, 32);
        SetupSkyBg();
        (*state)++;
        break;
    case 2:
        if (!FreeTempTileDataBuffersIfPossible())
        {
            SurfMinigame_InitOcean();
            CopyBgTilemapBufferToVram(SURF_BG_OCEAN);
            CopyBgTilemapBufferToVram(SURF_BG_SKY);
            SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
            if (LoadSpriteGraphicsAndAllocateManager())
            {

            spriteId = CreateSprite_Pikachu_Paddle(SURF_PIKA_X, SURF_WATERLINE_DEFAULT + SURF_Y_OFFSET);
            if (spriteId != MAX_SPRITES)
            {
                sSurfMinigameGfxManager->pikachuPaddleSprite = &gSprites[spriteId];
                gSprites[spriteId].hFlip = TRUE;
                game->paddleSpriteId = spriteId;
            }

            spriteId = CreateSprite_Pikachu_Surf(SURF_PIKA_X, SURF_WATERLINE_DEFAULT + SURF_Y_OFFSET);
            if (spriteId != MAX_SPRITES)
            {
                sSurfMinigameGfxManager->pikachuSurfSprite = &gSprites[spriteId];
                gSprites[spriteId].invisible = TRUE;
                gSprites[spriteId].hFlip = TRUE;
                game->surfSpriteId = spriteId;
            }

            spriteId = CreateSprite(&sSpriteTemplate_Progress, 220, 12, 0);
            if (spriteId != MAX_SPRITES)
            {
                sSurfMinigameGfxManager->progressSprite = &gSprites[spriteId];
                gSprites[spriteId].invisible = TRUE;
                game->progressSpriteId = spriteId;
            }

            }

            SetVBlankCallback(VBlankCB_SurfMinigame);
            ShowBg(SURF_BG_HUD);
            HideBg(1);
            ShowBg(SURF_BG_OCEAN);
            ShowBg(SURF_BG_SKY);
            BlendPalettes(PALETTES_ALL, 16, RGB_WHITE);
            BeginNormalPaletteFade(PALETTES_ALL, -1, 16, 0, RGB_WHITE);
            EnableInterrupts(INTR_FLAG_VBLANK);
            (*state)++;
        }
        break;
    case 3:
        UpdatePaletteFade();
        if (!gPaletteFade.active)
            return FALSE;
        break;
    }
    return TRUE;
}

bool8 SurfTask_FadeOutExit(u8 *state, SurfMinigameSetupTaskData *ptr)
{
    (void)ptr;
    switch (*state)
    {
    case 0:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_WHITE);
        SurfMinigame_StopBgm();
        (*state)++;
        break;
    case 1:
        if (!gPaletteFade.active)
            return FALSE;
        break;
    }
    return TRUE;
}

static void SetupSkyBg(void)
{
    FillBgTilemapBufferRect_Palette0(SURF_BG_SKY, 0, 0, 0, 32, 32);
    CopyBgTilemapBufferToVram(SURF_BG_SKY);
}

static bool32 LoadSpriteGraphicsAndAllocateManager(void)
{
    s32 i;

    for (i = 0; i < (int)ARRAY_COUNT(sSpriteSheets); i++)
        LoadCompressedSpriteSheet(&sSpriteSheets[i]);
    LoadSpritePalettes(sSpritePalettes);
    sSurfMinigameGfxManager = AllocZeroed(sizeof(*sSurfMinigameGfxManager));
    if (sSurfMinigameGfxManager == NULL)
        return FALSE;
    return TRUE;
}

static void VBlankCB_SurfMinigame(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void SurfMinigame_ApplyScroll(void)
{
    struct SurfGame *game = GetSurfGame();
    ChangeBgX(SURF_BG_OCEAN, game->scx, BG_COORD_SET);
}

void SurfMinigame_DrawHud(void)
{
    struct SurfGame *game = GetSurfGame();
    u8 *s;

    FillWindowPixelBuffer(WIN_SURF_HUD, PIXEL_FILL(1));
    s = gStringVar1;
    s = StringCopy(s, sText_Hp);
    s = ConvertIntToDecimalStringN(s, game->timer, STR_CONV_MODE_LEADING_ZEROS, 4);
    s = StringAppend(s, sText_Rad);
    ConvertIntToDecimalStringN(s, game->radnessScore, STR_CONV_MODE_LEADING_ZEROS, 4);
    AddTextPrinterParameterized3(WIN_SURF_HUD, FONT_SMALL, 4, 1, sTextColors, TEXT_SKIP_DRAW, gStringVar1);
    PutWindowTilemap(WIN_SURF_HUD);
    CopyWindowToVram(WIN_SURF_HUD, COPYWIN_FULL);
}

void SurfMinigame_HideGameplayHud(void)
{
    struct SurfGame *game = GetSurfGame();

    FillWindowPixelBuffer(WIN_SURF_HUD, PIXEL_FILL(1));
    CopyWindowToVram(WIN_SURF_HUD, COPYWIN_GFX);
    if (game->progressSpriteId != SPRITE_NONE)
        gSprites[game->progressSpriteId].invisible = TRUE;
}

void SurfMinigame_DrawResultsBox(void)
{
    FillWindowPixelBuffer(WIN_SURF_RESULTS, PIXEL_FILL(1));
    PutWindowTilemap(WIN_SURF_RESULTS);
    CopyWindowToVram(WIN_SURF_RESULTS, COPYWIN_FULL);
}

void SurfMinigame_PrintResultsLine(u8 line, const u8 *label, u16 value, bool32 pts)
{
    u8 *s = gStringVar1;
    s16 y = 8 + line * 16;

    s = StringCopy(s, label);
    s = StringAppend(s, sText_Spacer);
    s = ConvertIntToDecimalStringN(s, value, STR_CONV_MODE_LEADING_ZEROS, 4);
    if (pts)
        StringAppend(s, sText_Pts);
    AddTextPrinterParameterized3(WIN_SURF_RESULTS, FONT_NORMAL, 8, y, sTextColors, 0, gStringVar1);
    CopyWindowToVram(WIN_SURF_RESULTS, COPYWIN_GFX);
}

void SurfMinigame_PrintHiScore(void)
{
    AddTextPrinterParameterized3(WIN_SURF_RESULTS, FONT_NORMAL, 24, 56, sTextColors, 0, sText_HiScore);
    CopyWindowToVram(WIN_SURF_RESULTS, COPYWIN_GFX);
}

void SurfMinigame_UpdatePikachuSprites(void)
{
    struct SurfGame *game = GetSurfGame();
    s16 y;
    bool32 airborne = (game->pikaAction == SURF_ACTION_AIRBORNE
                    || game->pikaAction == SURF_ACTION_CRASH
                    || game->pikaAction == SURF_ACTION_END_JUMP
                    || game->pikaAction == SURF_ACTION_END_JUMP_INIT);
    struct Sprite *paddle;
    struct Sprite *surf;
    u8 anim;

    if (sSurfMinigameGfxManager == NULL)
        return;

    if (game->pikaAction == SURF_ACTION_JUMP_END
     || game->pikaAction == SURF_ACTION_END_JUMP
     || game->pikaAction == SURF_ACTION_END_JUMP_INIT)
        y = game->pikaY + SURF_Y_OFFSET + (s8)game->pikaYFrac;
    else
        y = game->pikaY + SURF_Y_OFFSET;

    paddle = sSurfMinigameGfxManager->pikachuPaddleSprite;
    surf = sSurfMinigameGfxManager->pikachuSurfSprite;
    if (paddle != NULL)
    {
        paddle->x = SURF_PIKA_X;
        paddle->y = y;
        paddle->hFlip = TRUE;
        paddle->invisible = airborne;
        if (!airborne)
        {
            if (game->pikaAction == SURF_ACTION_ON_WAVE)
                anim = SURF_ANIM_PADDLE_LOOP;
            else if (game->frameset <= 3)
                anim = SURF_ANIM_PADDLE_0 + (game->frameset % 4);
            else
                anim = SURF_ANIM_PADDLE_LOOP;
            StartSpriteAnimIfDifferent(paddle, anim);
        }
    }
    if (surf != NULL)
    {
        surf->x = SURF_PIKA_X;
        surf->y = y;
        surf->hFlip = TRUE;
        surf->invisible = !airborne;
        if (airborne)
        {
            if (game->pikaAction == SURF_ACTION_CRASH)
                anim = SURF_ANIM_SURF_CRASH;
            else if (game->pikaAction == SURF_ACTION_AIRBORNE && game->radnessMeter != 0)
                anim = SURF_ANIM_SURF_FLIP;
            else
                anim = SURF_ANIM_SURF_AIR;
            StartSpriteAnimIfDifferent(surf, anim);
        }
    }
}

void SurfMinigame_MoveClouds(void)
{
    struct SurfGame *game = GetSurfGame();
    u16 sum = (u8)game->cloudAccum + game->speed;
    u8 pixels = sum >> 8;

    game->cloudAccum = sum & 0xFF;
    if (pixels != 0)
        ChangeBgX(SURF_BG_SKY, pixels << 8, BG_COORD_ADD);
}

void SurfMinigame_SpawnSplash(void)
{
}

void SurfMinigame_UpdateProgressMarker(s32 dx)
{
    struct Sprite *sprite;

    if (sSurfMinigameGfxManager == NULL || sSurfMinigameGfxManager->progressSprite == NULL)
        return;
    sprite = sSurfMinigameGfxManager->progressSprite;
    sprite->x += dx;
}

void DestroyGfxManager(void)
{
    if (sSurfMinigameGfxManager != NULL)
    {
        if (sSurfMinigameGfxManager->pikachuPaddleSprite != NULL)
            DestroySprite(sSurfMinigameGfxManager->pikachuPaddleSprite);
        if (sSurfMinigameGfxManager->pikachuSurfSprite != NULL)
            DestroySprite(sSurfMinigameGfxManager->pikachuSurfSprite);
        if (sSurfMinigameGfxManager->progressSprite != NULL)
            DestroySprite(sSurfMinigameGfxManager->progressSprite);
        Free(sSurfMinigameGfxManager);
        sSurfMinigameGfxManager = NULL;
    }
    FreeSpriteTilesByTag(GFXTAG_PIKACHU_PADDLE);
    FreeSpriteTilesByTag(GFXTAG_PIKACHU_SURF);
    FreeSpritePaletteByTag(PALTAG_PIKACHU_PADDLE);
    FreeSpritePaletteByTag(PALTAG_PIKACHU_SURF);
    ResetSpriteData();
    FreeAllSpritePalettes();
}
