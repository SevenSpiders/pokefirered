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
static const u16 sHudSprite_Pal[] = INCBIN_U16("graphics/pikachu_beach/spritesheet_hud.gbapal");
static const u32 sHudSpriteTiles[] = INCBIN_U32("graphics/pikachu_beach/spritesheet_hud.4bpp.lz");

static const u16 sHudBgPal[] = {
    RGB(0, 0, 0),
    RGB(0, 0, 0),
    RGB(10, 10, 10),
    RGB(21, 21, 21),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
};

static const struct CompressedSpriteSheet sSpriteSheets[] = {
    {.data = sPikachuPaddleTiles, .size = 512 * 4, .tag = GFXTAG_PIKACHU_PADDLE},
    {.data = sPikachuSurfTiles, .size = 512 * 18, .tag = GFXTAG_PIKACHU_SURF},
    {.data = sHudSpriteTiles, .size = 32 * 24, .tag = GFXTAG_SURF_HUD},
};

static const struct SpritePalette sSpritePalettes[] = {
    {.data = sPikachuPaddle_Pal, .tag = PALTAG_PIKACHU_PADDLE},
    {.data = sPikachuSurf_Pal, .tag = PALTAG_PIKACHU_SURF},
    {.data = sHudSprite_Pal, .tag = PALTAG_SURF_HUD},
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
        .charBaseIndex = 2,
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
static const u8 sText_Spacer[] = _("  ");
static const u8 sText_Pts[] = _(" PTS");
static const u8 sText_HiScore[] = _("HI-SCORE!!");

static const u8 sCloudStartX[SURF_NUM_CLOUDS] = {
    32, 40, 48, 56, 64, 128, 136, 144, 152
};
static const u8 sCloudStartY[SURF_NUM_CLOUDS] = {
    16, 16, 16, 16, 16, 32, 32, 32, 32
};
static const u8 sCloudAnim[SURF_NUM_CLOUDS] = {
    SURF_ANIM_CLOUD_0, SURF_ANIM_CLOUD_1, SURF_ANIM_CLOUD_1, SURF_ANIM_CLOUD_2, SURF_ANIM_CLOUD_3,
    SURF_ANIM_CLOUD_0, SURF_ANIM_CLOUD_1, SURF_ANIM_CLOUD_2, SURF_ANIM_CLOUD_3
};

static const struct OamData sOam_Hud = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .size = ST_OAM_SIZE_0,
    .priority = 0,
};

static const union AnimCmd sAnim_Cloud0[] = { ANIMCMD_FRAME(0, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Cloud1[] = { ANIMCMD_FRAME(1, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Cloud2[] = { ANIMCMD_FRAME(2, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Cloud3[] = { ANIMCMD_FRAME(3, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_PikaIcon[] = { ANIMCMD_FRAME(4, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit0[] = { ANIMCMD_FRAME(8, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit1[] = { ANIMCMD_FRAME(9, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit2[] = { ANIMCMD_FRAME(10, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit3[] = { ANIMCMD_FRAME(11, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit4[] = { ANIMCMD_FRAME(12, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit5[] = { ANIMCMD_FRAME(13, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit6[] = { ANIMCMD_FRAME(14, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit7[] = { ANIMCMD_FRAME(15, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit8[] = { ANIMCMD_FRAME(16, 0xFF), ANIMCMD_END };
static const union AnimCmd sAnim_Digit9[] = { ANIMCMD_FRAME(17, 0xFF), ANIMCMD_END };

static const union AnimCmd *const sAnimTable_Hud[] = {
    sAnim_Cloud0,
    sAnim_Cloud1,
    sAnim_Cloud2,
    sAnim_Cloud3,
    sAnim_PikaIcon,
    sAnim_Digit0,
    sAnim_Digit1,
    sAnim_Digit2,
    sAnim_Digit3,
    sAnim_Digit4,
    sAnim_Digit5,
    sAnim_Digit6,
    sAnim_Digit7,
    sAnim_Digit8,
    sAnim_Digit9,
};

static const struct SpriteTemplate sSpriteTemplate_Hud = {
    .tileTag = GFXTAG_SURF_HUD,
    .paletteTag = PALTAG_SURF_HUD,
    .oam = &sOam_Hud,
    .anims = sAnimTable_Hud,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static void VBlankCB_SurfMinigame(void);
static bool32 LoadSpriteGraphicsAndAllocateManager(void);
static void SetupSkyBg(void);
static void SetupHudBg(void);
static void CreateHudSprites(struct SurfGame *game);

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
        ClearScheduledBgCopiesToVram();
        FreeAllWindowBuffers();
        InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
        InitWindows(sWindowTemplates);
        DeactivateAllTextPrinters();

        SetBgTilemapBuffer(0, ptr->bg0TilemapBuffer);
        SetBgTilemapBuffer(1, ptr->bg1TilemapBuffer);
        SetBgTilemapBuffer(2, ptr->bg2TilemapBuffer);
        SetBgTilemapBuffer(3, ptr->bg3TilemapBuffer);

        LoadPalette(sOceanPal, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(sHudBgPal, BG_PLTT_ID(1), PLTT_SIZE_4BPP);
        Menu_LoadStdPalAt(BG_PLTT_ID(15));
        DecompressAndCopyTileDataToVram(SURF_BG_OCEAN, sBg_Tiles, 0, 0, 0);

        FillBgTilemapBufferRect_Palette0(SURF_BG_HUD, 0, 0, 0, 32, 32);
        SetupSkyBg();
        (*state)++;
        break;
    case 2:
        if (!FreeTempTileDataBuffersIfPossible())
        {
            CpuFill16(0, (void *)((u8 *)BG_CHAR_ADDR(2) + SURF_TILE_TRANSPARENT * TILE_SIZE_4BPP), TILE_SIZE_4BPP);
            SetupHudBg();
            SurfMinigame_InitOcean();
            CopyBgTilemapBufferToVram(SURF_BG_HUD);
            CopyBgTilemapBufferToVram(SURF_BG_OCEAN);
            CopyBgTilemapBufferToVram(SURF_BG_SKY);
            CopyBgTilemapBufferToVram(SURF_BG_METER);
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
                gSprites[spriteId].hFlip = FALSE;
                game->surfSpriteId = spriteId;
            }

            CreateHudSprites(game);
            }

            SetVBlankCallback(VBlankCB_SurfMinigame);
            ShowBg(SURF_BG_HUD);
            ShowBg(SURF_BG_METER);
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
    FillBgTilemapBufferRect_Palette0(SURF_BG_SKY, SURF_TILE_BLANK, 0, 0, 32, 32);
    CopyBgTilemapBufferToVram(SURF_BG_SKY);
}

static void SetupHudBg(void)
{
    s32 i;

    FillBgTilemapBufferRect(SURF_BG_METER, SURF_TILE_TRANSPARENT, 0, 0, 32, 32, 1);
    FillBgTilemapBufferRect(SURF_BG_METER, SURF_TILE_BLANK, 0, 18, 32, 2, 1);
    FillBgTilemapBufferRect(SURF_BG_METER, SURF_TILE_ISLAND, 1, 18, 1, 1, 1);
    FillBgTilemapBufferRect(SURF_BG_METER, SURF_TILE_WAVE_L, 1, 19, 1, 1, 1);
    FillBgTilemapBufferRect(SURF_BG_METER, SURF_TILE_WAVE_M, 2, 19, 1, 1, 1);
    for (i = 3; i < 10; i++)
        FillBgTilemapBufferRect(SURF_BG_METER, SURF_TILE_WAVE_R, i, 19, 1, 1, 1);
    FillBgTilemapBufferRect(SURF_BG_METER, SURF_TILE_HP_H, 20, 19, 1, 1, 1);
    FillBgTilemapBufferRect(SURF_BG_METER, SURF_TILE_HP_P, 21, 19, 1, 1, 1);
    CopyBgTilemapBufferToVram(SURF_BG_METER);
}

static void CreateHudSprites(struct SurfGame *game)
{
    s32 i;
    s32 spriteId;

    for (i = 0; i < SURF_NUM_CLOUDS; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_Hud, sCloudStartX[i], sCloudStartY[i], 20);
        if (spriteId != MAX_SPRITES)
        {
            sSurfMinigameGfxManager->cloudSprites[i] = &gSprites[spriteId];
            StartSpriteAnim(&gSprites[spriteId], sCloudAnim[i]);
            gSprites[spriteId].oam.priority = 1;
        }
    }

    spriteId = CreateSprite(&sSpriteTemplate_Hud, SURF_PROGRESS_X, SURF_HUD_Y, 0);
    if (spriteId != MAX_SPRITES)
    {
        sSurfMinigameGfxManager->progressSprite = &gSprites[spriteId];
        StartSpriteAnim(&gSprites[spriteId], SURF_ANIM_PIKA_ICON);
        game->progressSpriteId = spriteId;
    }

    for (i = 0; i < SURF_NUM_DIGITS; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_Hud, SURF_DIGIT_X + i * 8, SURF_HUD_Y, 0);
        if (spriteId != MAX_SPRITES)
        {
            sSurfMinigameGfxManager->digitSprites[i] = &gSprites[spriteId];
            StartSpriteAnim(&gSprites[spriteId], SURF_ANIM_DIGIT_0);
        }
    }
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
    u16 n = game->timer;
    s32 i;
    u8 digit;

    if (sSurfMinigameGfxManager == NULL)
        return;

    for (i = SURF_NUM_DIGITS - 1; i >= 0; i--)
    {
        digit = n % 10;
        n /= 10;
        if (sSurfMinigameGfxManager->digitSprites[i] != NULL)
            StartSpriteAnimIfDifferent(sSurfMinigameGfxManager->digitSprites[i], SURF_ANIM_DIGIT_0 + digit);
    }
}

void SurfMinigame_HideGameplayHud(void)
{
    struct SurfGame *game = GetSurfGame();
    s32 i;

    HideBg(SURF_BG_METER);
    if (game->progressSpriteId != SPRITE_NONE)
        gSprites[game->progressSpriteId].invisible = TRUE;
    if (sSurfMinigameGfxManager == NULL)
        return;
    for (i = 0; i < SURF_NUM_DIGITS; i++)
    {
        if (sSurfMinigameGfxManager->digitSprites[i] != NULL)
            sSurfMinigameGfxManager->digitSprites[i]->invisible = TRUE;
    }
    for (i = 0; i < SURF_NUM_CLOUDS; i++)
    {
        if (sSurfMinigameGfxManager->cloudSprites[i] != NULL)
            sSurfMinigameGfxManager->cloudSprites[i]->invisible = TRUE;
    }
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
        surf->hFlip = FALSE;
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
    s32 i;
    struct Sprite *sprite;

    game->cloudAccum = sum & 0xFF;
    if (pixels == 0 || sSurfMinigameGfxManager == NULL)
        return;
    for (i = 0; i < SURF_NUM_CLOUDS; i++)
    {
        sprite = sSurfMinigameGfxManager->cloudSprites[i];
        if (sprite != NULL)
            sprite->x = (u8)(sprite->x + pixels);
    }
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
    s32 i;

    if (sSurfMinigameGfxManager != NULL)
    {
        if (sSurfMinigameGfxManager->pikachuPaddleSprite != NULL)
            DestroySprite(sSurfMinigameGfxManager->pikachuPaddleSprite);
        if (sSurfMinigameGfxManager->pikachuSurfSprite != NULL)
            DestroySprite(sSurfMinigameGfxManager->pikachuSurfSprite);
        if (sSurfMinigameGfxManager->progressSprite != NULL)
            DestroySprite(sSurfMinigameGfxManager->progressSprite);
        for (i = 0; i < SURF_NUM_CLOUDS; i++)
        {
            if (sSurfMinigameGfxManager->cloudSprites[i] != NULL)
                DestroySprite(sSurfMinigameGfxManager->cloudSprites[i]);
        }
        for (i = 0; i < SURF_NUM_DIGITS; i++)
        {
            if (sSurfMinigameGfxManager->digitSprites[i] != NULL)
                DestroySprite(sSurfMinigameGfxManager->digitSprites[i]);
        }
        Free(sSurfMinigameGfxManager);
        sSurfMinigameGfxManager = NULL;
    }
    FreeSpriteTilesByTag(GFXTAG_PIKACHU_PADDLE);
    FreeSpriteTilesByTag(GFXTAG_PIKACHU_SURF);
    FreeSpriteTilesByTag(GFXTAG_SURF_HUD);
    FreeSpritePaletteByTag(PALTAG_PIKACHU_PADDLE);
    FreeSpritePaletteByTag(PALTAG_PIKACHU_SURF);
    FreeSpritePaletteByTag(PALTAG_SURF_HUD);
    ResetSpriteData();
    FreeAllSpritePalettes();
}
