// pikachu_beach_gfx.c
#include "global.h"
#include "gflib.h"
#include "decompress.h" // loadCompressedSpritesheets
#include "task.h"
#include "strings.h"
#include "menu.h" // AddTextParameterized
#include "new_menu_helpers.h" // ResetBgPositions, ResetTempDataBuffers, Decompr...
#include "text_window.h" // LoadUserWindowGfx2
#include "minigames/pikachu_beach_internal.h"


enum {
    PALSLOT_LINE_NORMAL = 4, // Loaded as part of sBg_Pal
    PALSLOT_LINE_BET,
    PALSLOT_LINE_MATCH,
};

enum {
    GFXTAG_REEL_ICONS,
    GFXTAG_CLEFAIRY,
    GFXTAG_DIGITS,
};


static EWRAM_DATA struct SurfMinigameGfxManager * sSurfMinigameGfxManager = NULL;
static const u16 sBg_Pal[][16]             = INCBIN_U16("graphics/slot_machine/firered/bg.gbapal");
static const u32 sBg_Tiles[]               = INCBIN_U32("graphics/slot_machine/firered/bg.4bpp.lz");
static const u32 sBg_Tilemap[]             = INCBIN_U32("graphics/slot_machine/firered/bg.bin.lz");
static const u16 sBgPal_MatchLines[]       = INCBIN_U16("graphics/slot_machine/firered/match_lines.gbapal");
static const u16 sBgPal_PayoutLight[][16]  = INCBIN_U16("graphics/slot_machine/firered/payout_lights.gbapal");
static const u32 sButtonPressed_Tiles[]    = INCBIN_U32("graphics/slot_machine/firered/button_pressed.4bpp.lz");
static const u16 sCombosWindow_Pal[]       = INCBIN_U16("graphics/slot_machine/firered/combos_window.gbapal");
static const u32 sCombosWindow_Tiles[]     = INCBIN_U32("graphics/slot_machine/firered/combos_window.4bpp.lz");
static const u32 sCombosWindow_Tilemap[]   = INCBIN_U32("graphics/slot_machine/firered/combos_window.bin.lz");


static const struct CompressedSpriteSheet sSpriteSheets[] = {
    // {.data = sReelIcons_Tiles, .size = 0xe00, .tag = GFXTAG_REEL_ICONS},
    // {.data = sClefairy_Tiles,  .size = 0xc00, .tag = GFXTAG_CLEFAIRY},
    // {.data = sDigits_Tiles,    .size = 0x280, .tag = GFXTAG_DIGITS},
};

static const struct SpritePalette sSpritePalettes[] = {
    // {.data = sReelIcons_Pal[0], .tag = PALTAG_REEL_ICONS_0},
    // {.data = sReelIcons_Pal[1], .tag = PALTAG_REEL_ICONS_1},
    // {.data = sReelIcons_Pal[2], .tag = PALTAG_REEL_ICONS_2},
    // {.data = sReelIcons_Pal[3], .tag = PALTAG_REEL_ICONS_3},
    // {.data = sReelIcons_Pal[4], .tag = PALTAG_REEL_ICONS_4},
    // {.data = sClefairy_Pal,     .tag = PALTAG_CLEFAIRY},
    // {.data = sDigits_Pal,       .tag = PALTAG_DIGITS},
    // {}
};

static const u16 sReelIconPaletteTags[] = {
    // [ICON_7]         = PALTAG_REEL_ICONS_2,
    // [ICON_ROCKET]    = PALTAG_REEL_ICONS_2,
    // [ICON_PIKACHU]   = PALTAG_REEL_ICONS_0,
    // [ICON_PSYDUCK]   = PALTAG_REEL_ICONS_0,
    // [ICON_CHERRIES]  = PALTAG_REEL_ICONS_2,
    // [ICON_MAGNEMITE] = PALTAG_REEL_ICONS_4,
    // [ICON_SHELLDER]  = PALTAG_REEL_ICONS_3,
};



// prototype functions
static void VBlankCB_SlotMachine(void);
static void HBlankCB_SlotMachine(void);
static inline void SetBackdropColor(u16 color, u16 *pal);
static bool32 LoadSpriteGraphicsAndAllocateManager(void);
static void InitGfxManager(struct SurfMinigameGfxManager * manager);


// templates
static const struct BgTemplate sBgTemplates[] = {
    [0] = {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0x000
    }, 
    [1] = {
        .bg = 3,
        .charBaseIndex = 3,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0x000
    }, 
    [2] = {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0x000
    }, 
    [3] = {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0x000
    }
};

static const struct WindowTemplate sWindowTemplates[] = {
    [0] = {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 15,
        .width = 20,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x04f
    }, 
    [1] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 14,
        .baseBlock = 0x013
    },
    [2] = DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sYesNoWindowTemplate = {
    .bg = 0,
    .tilemapLeft = 19,
    .tilemapTop = 9,
    .width = 6,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x9F
};





bool8  MG0Task_InitGraphics(u8 * state, SurfMinigameSetupTaskData * ptr)
{
    u16 pal[2];
    u8 textColor[3];
    u32 x;

    switch (*state)
    {
    case 0:
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        (*state)++;
        break;
    case 1:
        SetVBlankCallback(NULL);
        ResetSpriteData();
        FreeAllSpritePalettes();
        RequestDma3Fill(0, (void *)OAM, OAM_SIZE, DMA3_32BIT);
        RequestDma3Fill(0, (void *)VRAM, 0x20, DMA3_32BIT);
        RequestDma3Fill(0, (void *)(VRAM + 0xC000), 0x20, DMA3_32BIT);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgPositions();
        ResetBgsAndClearDma3BusyFlags(FALSE);
        InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
        InitWindows(sWindowTemplates);

        SetBgTilemapBuffer(3, ptr->bg3TilemapBuffer);
        FillBgTilemapBufferRect_Palette0(3, 0, 0, 0, 32, 32);
        CopyBgTilemapBufferToVram(3);

        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(2, sBg_Tiles, 0, 0x00, 0);
        DecompressAndCopyTileDataToVram(2, sButtonPressed_Tiles, 0, 0xC0, 0);
        SetBgTilemapBuffer(2, ptr->bg2TilemapBuffer);
        CopyToBgTilemapBuffer(2, sBg_Tilemap, 0, 0x00);
        CopyBgTilemapBufferToVram(2);
        LoadPalette(sBg_Pal, BG_PLTT_ID(0), sizeof(sBg_Pal));
        LoadPalette(sBgPal_MatchLines, BG_PLTT_ID(PALSLOT_LINE_BET), sizeof(sBgPal_MatchLines));
        LoadPalette(sCombosWindow_Pal, BG_PLTT_ID(7), sizeof(sCombosWindow_Pal));
        SetBackdropColor(RGB(30, 30, 31), pal);
        LoadUserWindowGfx2(0, 0x00A, BG_PLTT_ID(13));
        LoadStdWindowGfxOnBg(0, 0x001, BG_PLTT_ID(15));

        SetBgTilemapBuffer(0, ptr->bg0TilemapBuffer);
        FillBgTilemapBufferRect_Palette0(0, 0, 0, 2, 32, 30);
        DecompressAndCopyTileDataToVram(1, sCombosWindow_Tiles, 0, 0, 0);
        DecompressAndCopyTileDataToVram(1, sCombosWindow_Tilemap, 0, 0, 1);
        CopyBgTilemapBufferToVram(1);

        LoadPalette(GetTextWindowPalette(2), BG_PLTT_ID(14), PLTT_SIZE_4BPP);
        FillWindowPixelBuffer(1, 0xFF);
        PutWindowTilemap(1);

        x = DISPLAY_WIDTH - 4 - GetStringWidth(FONT_SMALL, gString_SlotMachineControls, 0);
        textColor[0] = TEXT_DYNAMIC_COLOR_6;
        textColor[1] = TEXT_COLOR_WHITE;
        textColor[2] = TEXT_COLOR_DARK_GRAY;
        AddTextPrinterParameterized3(1, FONT_SMALL, x, 0, textColor, 0, gString_SlotMachineControls);
        CopyBgTilemapBufferToVram(0);

        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | 0x20 | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG3 | BLDCNT_TGT1_OBJ | BLDCNT_TGT1_BD | BLDCNT_EFFECT_DARKEN);
        LoadSpriteGraphicsAndAllocateManager();
        // CreateReelIconSprites();
        // CreateScoreDigitSprites();
        // CreateClefairySprites();
        // UpdateCoinsDisplay();
        BlendPalettes(PALETTES_ALL, 0x10, RGB_BLACK);
        SetVBlankCallback(VBlankCB_SlotMachine);
        SetHBlankCallback(HBlankCB_SlotMachine);
        (*state)++;
        break;
    case 2:
        if (!FreeTempTileDataBuffersIfPossible())
        {
            ShowBg(0);
            ShowBg(3);
            ShowBg(2);
            HideBg(1);
            // InitReelButtonTileMem();
            BlendPalettes(PALETTES_ALL, 0x10, RGB_BLACK);
            BeginNormalPaletteFade(PALETTES_ALL, -1, 16, 0, RGB_BLACK);
            EnableInterrupts(INTR_FLAG_VBLANK | INTR_FLAG_HBLANK);
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



static void VBlankCB_SlotMachine(void)
{
    TransferPlttBuffer();
    LoadOam();
    ProcessSpriteCopyRequests();
}

// creates barrel effect from hline 43 to 127
static void HBlankCB_SlotMachine(void)
{
    // s32 vcount = REG_VCOUNT - 43;
    // if (vcount < 84u)
    // {
    //     *sSlotMachineGfxManager->reelIconAffineParamPtr = sReelIconAffineParams[vcount];
    //     REG_BLDY = sReelIconBldY[vcount];
    // }
    // else
    // {
    //     *sSlotMachineGfxManager->reelIconAffineParamPtr = 256;
    //     REG_BLDY = 0;
    // }
}

// static void InitReelButtonTileMem(void)
// {
    // s32 i, j;
    // struct SlotMachineSetupTaskData * data = GetSurfMinigameSetupTaskDataPtr();
    // u16 * buffer = GetBgTilemapBuffer(2);

    // for (i = 0; i < 3; i++)
    // {
    //     for (j = 0; j < 4; j++)
    //     {
    //         u16 idx = 0x0229;
    //         data->buttonReleasedTiles[i][j] = buffer[idx];
    //         data->buttonPressedTiles[i][j] = j + 0xC0;
    //     }
    // }
// }

static bool32 LoadSpriteGraphicsAndAllocateManager(void)
{
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sSpriteSheets); i++)
        LoadCompressedSpriteSheet(&sSpriteSheets[i]);
    LoadSpritePalettes(sSpritePalettes);
    sSurfMinigameGfxManager = Alloc(sizeof(*sSurfMinigameGfxManager));
    if (sSurfMinigameGfxManager == NULL)
        return FALSE;
    InitGfxManager(sSurfMinigameGfxManager);
    return TRUE;
}

static inline void SetBackdropColor(u16 color, u16 *pal)
{
    *pal = color;
    SetBackdropFromPalette(pal);
}

static void InitGfxManager(struct SurfMinigameGfxManager * manager)
{
    // s32 i, j;

    // for (i = 0; i < NUM_REELS; i++)
    // {
    //     manager->field_00[i] = 0;
    //     for (j = 0; j < REEL_LOAD_LENGTH; j++)
    //         manager->reelIconSprites[i][j] = NULL;
    // }
}



void DestroyGfxManager(void)
{
    if (sSurfMinigameGfxManager != NULL)
    {
        Free(sSurfMinigameGfxManager);
        sSurfMinigameGfxManager = NULL;
    }
}