#include "global.h"
#include "gflib.h"
#include "strings.h"
#include "battle_ui.h"
#include "battle_message.h" // BattlePutTextOnWindow
#include "battle_controllers.h" // ChooseMoveStruct
#include "graphics.h" // gMenuInfoElements_Gfx


typedef struct {
    u32 windowId;
    s32 offsetX;
    s32 offsetY;
    // u8 row;
    u16 tilemapTop;
    bool8 isVisible; 
} IconTray;

struct IconData
{
    u8 width;
    u8 height;
    u16 tile;
};

static const u16 sBattleIcons_Pal[] = INCBIN_U16("graphics/battle_interface/status_icons.gbapal");
static const u8 sBattleIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/status_icons.4bpp");
static IconTray sIconTrays[2]; // 0: player, 1: enemy
static bool8 sLoaded;

#define ICON_NONE { 0, 0, 0 }
#define ICON_TYPE(tile) { 32, 12, tile }
#define ICON_NUMBER(tile) { 8, 7, tile }
#define ICON_STAT(tile) { 14, 8, tile }
#define ICON_STATUS(tile) { 14, 8, tile }
#define STAT_NEGATIVE NUM_BATTLE_STATS-1
#define ICON_DISTANCE 2

static const struct IconData gTypeIconData[] = {
    [TYPE_NORMAL]   = ICON_TYPE(0x20),
    [TYPE_FIRE]     = ICON_TYPE(0x24),
    [TYPE_WATER]    = ICON_TYPE(0x28),
    [TYPE_GRASS]    = ICON_TYPE(0x2C),
    [TYPE_ELECTRIC] = ICON_TYPE(0x40),
    [TYPE_ROCK]     = ICON_TYPE(0x44),
    [TYPE_GROUND]   = ICON_TYPE(0x48),
    [TYPE_ICE]      = ICON_TYPE(0x4C),
    [TYPE_FLYING]   = ICON_TYPE(0x60),
    [TYPE_FIGHTING] = ICON_TYPE(0x64),
    [TYPE_GHOST]    = ICON_TYPE(0x68),
    [TYPE_BUG]      = ICON_TYPE(0x6C),
    [TYPE_POISON]   = ICON_TYPE(0x80),
    [TYPE_PSYCHIC]  = ICON_TYPE(0x84),
    [TYPE_STEEL]    = ICON_TYPE(0x88),
    [TYPE_DARK]     = ICON_TYPE(0x8C),
    [TYPE_DRAGON]   = ICON_TYPE(0xA0),
    [TYPE_MYSTERY]  = ICON_TYPE(0xA4), 
    [MOVE_CATEGORY_PHYSICAL + NUMBER_OF_MON_TYPES] = ICON_TYPE(0x04),
    [MOVE_CATEGORY_SPECIAL + NUMBER_OF_MON_TYPES] = ICON_TYPE(0x06),
    [MOVE_CATEGORY_STATUS + NUMBER_OF_MON_TYPES] = ICON_TYPE(0x08),
};

static const struct IconData gStatIconData[] = {
    [STAT_HP] =                     ICON_NONE, // no HP stat
    [STAT_ATK]   =                  ICON_STAT(0x00),
    [STAT_DEF]  =                   ICON_STAT(0x02),
    [STAT_SPATK]  =                 ICON_STAT(0x04),
    [STAT_SPDEF]  =                 ICON_STAT(0x06),
    [STAT_EVASION] =                ICON_STAT(0x08),
    [STAT_SPEED]  =                 ICON_STAT(0x0A),
    [STAT_ACC] =                    ICON_STAT(0x0C),

    [STAT_ATK + STAT_NEGATIVE]   =  ICON_STAT(0x10),
    [STAT_DEF + STAT_NEGATIVE]  =   ICON_STAT(0x12),
    [STAT_SPATK + STAT_NEGATIVE]  = ICON_STAT(0x14),
    [STAT_SPDEF + STAT_NEGATIVE]  = ICON_STAT(0x16),
    [STAT_EVASION + STAT_NEGATIVE] = ICON_STAT(0x18),
    [STAT_SPEED + STAT_NEGATIVE]  = ICON_STAT(0x1A),
    [STAT_ACC + STAT_NEGATIVE] =    ICON_STAT(0x1C),
};


static const struct IconData gNumberIconData[] = {
    [1] = ICON_NUMBER(0x20),
    [2] = ICON_NUMBER(0x21),
    [3] = ICON_NUMBER(0x22),
    [4] = ICON_NUMBER(0x23),
    [5] = ICON_NUMBER(0x24),
    [6] = ICON_NUMBER(0x25),
    [7] = ICON_NUMBER(0x26),
    [8] = ICON_NUMBER(0x27),
    [9] = ICON_NUMBER(0x28),
    [0] = ICON_NUMBER(0x2A),
    [10] = ICON_NUMBER(0x2B),
};

static const struct IconData gStatusIconData[] = {
    [STATUS_NONE] = ICON_NONE,

    /* Tiles start at 0x40 and increment by 2 for each entry */
    [STATUS_SLEEP]       = ICON_STATUS(0x40),
    [STATUS_POISON]      = ICON_STATUS(0x42),
    [STATUS_BURN]        = ICON_STATUS(0x44),
    [STATUS_FREEZE]      = ICON_STATUS(0x46),
    [STATUS_PARALYSIS]   = ICON_STATUS(0x48),
    [STATUS_TOXIC]       = ICON_STATUS(0x4A),

    [STATUS_CONFUSED]    = ICON_STATUS(0x4C),
    [STATUS_FLINCHED]    = ICON_STATUS(0x4E),
    [STATUS_UPROAR]      = ICON_STATUS(0x50),
    [STATUS_BIDE]        = ICON_STATUS(0x52),
    [STATUS_MULTI_TURN]  = ICON_NONE,
    [STATUS_THRASH]      = ICON_STATUS(0x56),
    [STATUS_WRAPPED]     = ICON_STATUS(0x58),
    [STATUS_INFATUATION] = ICON_STATUS(0x5A),
    [STATUS_FOCUS]       = ICON_STATUS(0x5C),
    [STATUS_TRANSFORMED] = ICON_STATUS(0x5E),
    [STATUS_RECHARGE]    = ICON_STATUS(0x60),
    [STATUS_RAGE]        = ICON_STATUS(0x62),
    [STATUS_SUBSTITUTE]  = ICON_STATUS(0x64),
    [STATUS_DESTINY_BOND]= ICON_STATUS(0x66),
    [STATUS_NO_ESCAPE]   = ICON_STATUS(0x68),
    [STATUS_NIGHTMARE]   = ICON_STATUS(0x6A),
    [STATUS_CURSED]      = ICON_STATUS(0x6C),
    [STATUS_FORESIGHT]   = ICON_STATUS(0x6E),
    [STATUS_DEFENSE_CURL]= ICON_STATUS(0x70),
    [STATUS_TORMENT]     = ICON_STATUS(0x72),

    /* Status3 / miscellaneous */
    [STATUS_LEECHSEED]   = ICON_STATUS(0x74),
    [STATUS_ALWAYS_HITS] = ICON_STATUS(0x76),
    [STATUS_PERISH_SONG] = ICON_STATUS(0x78),
    [STATUS_IN_AIR]      = ICON_STATUS(0x7A),
    [STATUS_UNDERGROUND] = ICON_STATUS(0x7C),
    [STATUS_MINIMIZED]   = ICON_STATUS(0x7E),
    [STATUS_CHARGED_UP]  = ICON_STATUS(0x80),
    [STATUS_ROOTED]      = ICON_STATUS(0x82),
    [STATUS_YAWN]        = ICON_STATUS(0x84),
    [STATUS_IMPRISONED]  = ICON_STATUS(0x86),
    [STATUS_GRUDGE]      = ICON_STATUS(0x88),
    [STATUS_NO_CRIT]     = ICON_STATUS(0x8A),
    [STATUS_MUDSPORT]    = ICON_STATUS(0x8C),
    [STATUS_WATERSPORT]  = ICON_STATUS(0x8E),
    [STATUS_UNDERWATER]  = ICON_STATUS(0x90),
    [STATUS_INTIMIDATE]  = ICON_STATUS(0x92),
    [STATUS_TRACE]       = ICON_STATUS(0x94),
};


static void BlitTypeIcon(u8 windowId, u8 iconId, u16 x, u16 y)
{
    BlitBitmapRectToWindow(windowId, &gMenuInfoElements_Gfx[gTypeIconData[iconId].tile * TILE_SIZE_4BPP], 0, 0, 128, 128, 
        x, y, gTypeIconData[iconId].width, gTypeIconData[iconId].height);
}

static void BlitIcon(u8 windowId, struct IconData icon, u16 x, u16 y)
{
    BlitBitmapRectToWindow(windowId, &sBattleIcons_Gfx[icon.tile * TILE_SIZE_4BPP], 0, 0, 128, 160, x, y, icon.width, icon.height);
}

static void AddIcon(u8 battlerId, struct IconData icon)
{
    BlitIcon(sIconTrays[battlerId].windowId, icon, sIconTrays[battlerId].offsetX, sIconTrays[battlerId].offsetY);
    sIconTrays[battlerId].offsetX += icon.width + ICON_DISTANCE;
}

static void AddStatIcon(u8 battlerId, u8 iconId, s8 val)
{
    DebugPrintf("stat %d val %d", iconId, val);
    if (val < 0) iconId += STAT_NEGATIVE;
    AddIcon(battlerId, gStatIconData[iconId]);
    if (abs(val) == 1) return;
    sIconTrays[battlerId].offsetX -= ICON_DISTANCE;
    AddIcon(battlerId, gNumberIconData[abs(val)]);
}

static void AddStatusIcon(u8 battlerId, u16 status)
{
    if (status == STATUS_NONE) return;
    DebugPrintf("add status %d", status);
    AddIcon(battlerId, gStatusIconData[GET_STATUS_TYPE(status)]);
}


static void DisplayMoveType(u8 type)
{
    BlitTypeIcon(B_WIN_MOVE_TYPE, type, 0, 0);
}

static void DisplayMoveCategory(u8 category)
{
    BlitTypeIcon(B_WIN_MOVE_TYPE, category + NUMBER_OF_MON_TYPES, 34, 0);
}

static void DisplayMovePower(u8 power)
{
    // Pwr
    StringCopy(gDisplayedStringBattle, gText_MovePwr);
    BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_PP);    
    
    // Power number
    if (power == 0)
    {
        *gDisplayedStringBattle = CHAR_SPACE;
        StringCopy(gDisplayedStringBattle+1, gText_PokeSum_TwoHyphens);
    }
    else
        ConvertIntToDecimalStringN(gDisplayedStringBattle, power, STR_CONV_MODE_RIGHT_ALIGN, 3);
    BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_PP_REMAINING); // defined in battle_bg.c
}

static void CopyIconsToVRAM(u8 windowId)
{
    PutWindowTilemap(windowId);
	CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void LoadStatusWindow(u32 i)
{
    sIconTrays[i].windowId = B_WIN_STATUS1+i;
    sIconTrays[i].tilemapTop = GetWindowAttribute(B_WIN_STATUS1+i, WINDOW_TILEMAP_TOP);
    FillWindowPixelRect(B_WIN_STATUS1+i, 0, 0, 0, B_WIN_STATUS_W*8, B_WIN_STATUS_H*8);
    CopyIconsToVRAM(B_WIN_STATUS1+i);
}

void BattleUI_LoadGfx()
{
    LoadStatusWindow(0);
    LoadStatusWindow(1);

    LoadPalette(gMenuInfoElements2_Pal, BG_PLTT_ID(B_PLTT_TYPE), PLTT_SIZE_4BPP);
    LoadPalette(sBattleIcons_Pal, BG_PLTT_ID(B_PLTT_ICONS), PLTT_SIZE_4BPP);
    FillWindowPixelRect(B_WIN_MOVE_TYPE, 15, 0, 0, 64, 16);
    sLoaded = TRUE;
}

void BattleUI_Unload()
{
    sLoaded = FALSE;
}

void BattleUI_DisplayMoveInfo()
{
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleBufferA[gActiveBattler][4]);
    u32 move = moveInfo->moves[gMoveSelectionCursor[gActiveBattler]];

    DisplayMoveCategory(gBattleMoves[move].category);
    DisplayMoveType(gBattleMoves[move].type);
    DisplayMovePower(moveInfo->currentPower[gMoveSelectionCursor[gActiveBattler]]);
    PutWindowTilemap(B_WIN_MOVE_TYPE);
	CopyWindowToVram(B_WIN_MOVE_TYPE, COPYWIN_FULL);
}

static void ClearTray(u8 battlerId)
{
    FillWindowPixelBuffer(sIconTrays[battlerId].windowId, 0);
    sIconTrays[battlerId].offsetX = 0;
    sIconTrays[battlerId].offsetY = 0;
}

void BattleUI_UpdateStatusIcons(u8 battlerId)
{
    u32 i;
    s8 val;
    u32 windowId = sIconTrays[battlerId].windowId;

    DebugPrintf("update status");
    if (!sLoaded) return;

    ClearTray(battlerId);

    if (!sIconTrays[battlerId].isVisible) return;

    for (i=1; i<NUM_BATTLE_STATS;i++) //start at 1 to skip STAT_HP
    {
        val = gBattleMons[battlerId].statStages[i] - DEFAULT_STAT_STAGE;
        if (val != 0)
            AddStatIcon(battlerId, i, val);
    }

    for(i=0; i<MAX_MON_STATUSES; i++)
    {
        AddStatusIcon(battlerId, gBattleMons[battlerId].statuses[i]);
    }
    CopyIconsToVRAM(windowId);
}

void BattleUI_UpdateBG0Offset(u16 x, u16 y)
{
    u32 i;
    for (i=0; i<2; i++)
    {
        SetWindowAttribute(sIconTrays[i].windowId, WINDOW_TILEMAP_TOP, y/8 + sIconTrays[i].tilemapTop);
        CopyIconsToVRAM(sIconTrays[i].windowId);
    }
}

void BattleUI_ShowStatusIcons(u8 battlerId, bool8 shouldShow)
{
    if (sIconTrays[battlerId].isVisible == shouldShow) return;
    DebugPrintf("Show status? battler %d: %d", battlerId, shouldShow);
    sIconTrays[battlerId].isVisible = shouldShow;
    BattleUI_UpdateStatusIcons(battlerId);
}

// static bool8 sIsShowingInfo;

// #define PALETTE_ICONS (1 << 10)
// #define PALETTE_TEXT_BOX (1 << 0)
// #define PALETTE_HP (1 << 1)
// #define PALETTE_TEXT (1 << 5)
// #define PALETTES_TO_FADE PALETTES_ALL - PALETTE_ICONS - PALETTE_TEXT_BOX - PALETTE_HP - PALETTE_TEXT
// // #define PALETTES_TO_FADE PALETTES_OBJECTS

// static void ShowInfo(void)
// {
//     Healthbox_Blank(gHealthboxSpriteIds[1]);
//     // show info window
//     DebugPrintf("Show info");
//     BeginNormalPaletteFade(PALETTES_TO_FADE, 0, 0, 8, RGB_BLACK);
// }

// static void ResetHealthBar(void)
// {
//     Healthbox_HideInfo(gHealthboxSpriteIds[1]);
//     UpdateHealthboxAttribute(gHealthboxSpriteIds[1], &gEnemyParty[gBattlerPartyIndexes[0]], HEALTHBOX_ALL);
//     DebugPrintf("Hide info");
//     BeginNormalPaletteFade(PALETTES_TO_FADE, 0, 8, 0, RGB_BLACK);
// }
