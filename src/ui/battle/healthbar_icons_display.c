#include "global.h"
#include "battle_controllers.h" // ChooseMoveStruct
#include "window.h" // blitBitmapRectToWindow
#include "palette.h" // LoadPalette
#include "strings.h" // gText_PokeSum_Power, gText_MoveInterfacePP, gText_TwoHyphens
#include "string_util.h" // ConvertIntToDecimalStringN, StringCopy
#include "battle_main.h" // gDisplayedStringBattle // maybe include "battle.h" instead
#include "battle_message.h" // BattlePutTextOnWindow
#include "battle_interface.h" // ShowHPText
#include "random.h" // Random
#include "pokemon.h" // BattlePokemon + stats

#define B_INTERFACE_GFX_STATUS_PSN_BATTLER0 21
#define PAL_STATUS_PSN 0
#define MULTIPLIER_ICON_INDEX 25 // Offset for multiplier icons in sSplitIcons_Gfx


typedef struct {
    u32 windowId;
    u32 offsetX;
    u32 offsetY;
    u32 tilemapTop;
} IconTray;

struct MoveMenuInfoIcon
{
    u8 width;
    u8 height;
    u16 offset;
};

static void BlitMenuInfoIcon(u8 windowId, const u8 *gfx, struct MoveMenuInfoIcon icon, u16 x, u16 y);
static void Init();
static void ClearIconTray(u32 trayIndex);
static void UpdateIconTray(u32 trayIndex);
static void SetStatusIcon(u32 monIndex);
static void SetStatus2Icon(u32 monIndex);
static void SetStatIcon(u32 monIndex, u8 stat);


static bool32 sInitialized;
static u32 sBGOffsetY;
static IconTray sIconTrays[3]; // 0: player, 1: enemy, 2: zone


static void BlitMenuInfoIcon(u8 windowId,const u8 *gfx, struct MoveMenuInfoIcon icon, u16 x, u16 y)
{
    BlitBitmapRectToWindow(windowId, &gfx[icon.offset * TILE_SIZE_4BPP], 0, 0, 128, 160, x, y, icon.width, icon.height);
}

#define STAT_NEGATE 7 // Offset for negative status icons in status_icons array


static const u16 sSplitIcons_Pal[] = INCBIN_U16("graphics/battle_interface/menu_info.gbapal");
static const u8 sSplitIcons_Gfx[] = INCBIN_U8("graphics/battle_interface/menu_info.4bpp");
static const struct MoveMenuInfoIcon status_icons[] = {
    // { width, height, offset }

    [STAT_ATK] =    { 16, 8,  0xc6}, // Attack
    [STAT_DEF] =    { 16, 8,  0xc8}, // Defense
    [STAT_SPEED] =  { 16, 8,  0xce}, // Speed
    [STAT_SPATK] =  { 16, 8,  0xca}, // Sp. Atk
    [STAT_SPDEF] =  { 16, 8,  0xcc}, // Sp. Def
    [STAT_ACC] =    { 16, 8,  0xe6}, // Accuracy
    [STAT_EVASION] =    { 16, 8, 0xe8}, // Evasion

    [STAT_ATK   + STAT_NEGATE] = { 16, 8, 0xd6}, // negative Attack
    [STAT_DEF   + STAT_NEGATE] = { 16, 8, 0xd8}, // negative Defense
    [STAT_SPEED + STAT_NEGATE] = { 16, 8, 0xda}, // negative Sp. Atk
    [STAT_SPATK + STAT_NEGATE] = { 16, 8, 0xdc}, // negative Sp. Def
    [STAT_SPDEF + STAT_NEGATE] = { 16, 8, 0xde}, // negative Speed
    [STAT_ACC   + STAT_NEGATE] = { 16, 8, 0xf6}, // negative Accuracy
    [STAT_EVASION + STAT_NEGATE] = { 16, 8, 0xf8}, // negative Evasion


    [15] = { 16, 8, 0xea}, // Critical Hit
    [16] = { 16, 8, 0xec}, // Flinch
    [17] = { 16, 8, 0xea}, // Confusion
    [0] = { 16, 8, 0xfa}, // negative Critical Hit
    [18] = { 16, 8, 0xfc}, // negative Flinch
    [19] = { 16, 8, 0xfe}, // wrapped


    [20] = { 16, 8, 0x120}, // Poison
    [21] = { 16, 8, 0x122}, // Paralysis
    [22] = { 16, 8, 0x124}, // Sleep
    [23] = { 16, 8, 0x126}, // Freeze
    [24] = { 16, 8, 0x128}, // Burn

    [MULTIPLIER_ICON_INDEX] = {}, // Placeholder for x1 multiplier
    [MULTIPLIER_ICON_INDEX + 1] = {11,8, 0x130}, // x1 multiplier
    [MULTIPLIER_ICON_INDEX + 2] = {11,8, 0x132}, // x2 multiplier
    [MULTIPLIER_ICON_INDEX + 3] = {11,8, 0x134}, // x3 multiplier
    [MULTIPLIER_ICON_INDEX + 4] = {11,8, 0x136}, // x4 multiplier
    [MULTIPLIER_ICON_INDEX + 5] = {11,8, 0x138}, // x5 multiplier
    [MULTIPLIER_ICON_INDEX + 6] = {11,8, 0x13a}, // x6 multiplier
};

static void Init()
{
    LoadPalette(sSplitIcons_Pal, 10 * 0x10, sizeof(sSplitIcons_Pal));
    sInitialized = TRUE;

    sIconTrays[0].windowId = B_WIN_ICONS_PLAYER1; // player
    sIconTrays[0].tilemapTop = 12; // y offset for player icons
    sIconTrays[0].offsetY = 3; // 3 pixel from top

    sIconTrays[1].windowId = B_WIN_ICONS_OPPONENT1; // enemy
    sIconTrays[1].tilemapTop = 5; // y offset for enemy icons
    sIconTrays[1].offsetY = 0;
}

static void ClearIconTray(u32 trayIndex)
{
    FillWindowPixelBuffer(sIconTrays[trayIndex].windowId, PIXEL_FILL(0));
    sIconTrays[trayIndex].offsetX = 0;
}

static void UpdateIconTray(u32 trayIndex)
{
    u32 i;
    u32 tilemapTop =  sBGOffsetY / 8 + sIconTrays[trayIndex].tilemapTop;
    SetWindowAttribute(sIconTrays[trayIndex].windowId, WINDOW_TILEMAP_TOP, tilemapTop);
    ClearIconTray(trayIndex);
    SetStatusIcon(trayIndex);
    SetStatus2Icon(trayIndex);

    for (i=1; i< NUM_BATTLE_STATS; i++)
    {
        SetStatIcon(trayIndex, i); // STAT_ATK, STAT_DEF, STAT_SPEED, STAT_SPATK, STAT_SPDEF, STAT_ACC, STAT_EVASION
    }
    // if trayIndex == 0 -> check if offset covers HP numbers -> hide or show them
    if (trayIndex == 0)
        Healthbox_ShowHPText(0, sIconTrays[trayIndex].offsetX < 50);

    PutWindowTilemap(sIconTrays[trayIndex].windowId);
    CopyWindowToVram(sIconTrays[trayIndex].windowId, COPYWIN_FULL);
}

static void UpdateZoneIconTray()
{
    // for zone effects like weather, spikes, light screen etc.
}

static void SetStatusIcon(u32 monIndex)
{
    IconTray *iconTray = &sIconTrays[monIndex];
    struct BattlePokemon *battleMon = &gBattleMons[monIndex];
    u32 status = battleMon->status1;
    u32 iconIndex = 0;

    // DebugPrintf("SetStatusIcon: mon%d, status=0x%08x", monIndex, status);

    if (status == STATUS1_NONE)
        return;

    if (status & STATUS1_PSN_ANY)
        iconIndex = 20; // Poison
    else if (status & STATUS1_PARALYSIS)
        iconIndex = 21; // Paralysis
    else if (status & STATUS1_SLEEP)
        iconIndex = 22; // Sleep
    else if (status & STATUS1_FREEZE)
        iconIndex = 23; // Freeze
    else if (status & STATUS1_BURN)
        iconIndex = 24; // Burn
    else
        return; // No icon for this status

    BlitMenuInfoIcon(iconTray->windowId, sSplitIcons_Gfx, status_icons[iconIndex], iconTray->offsetX + 2, iconTray->offsetY);
    iconTray->offsetX += status_icons[iconIndex].width + 2;
}

static void SetStatus2Icon(u32 monIndex)
{
    IconTray *iconTray = &sIconTrays[monIndex];
    struct BattlePokemon *battleMon = &gBattleMons[monIndex];
    u32 status2 = battleMon->status2;
    u32 iconIndex = 0;

    // DebugPrintf("SetStatus2Icon: mon%d, status2=0x%08x", monIndex, status2);

    if (status2 == 0)
        return;

    if (status2 & STATUS2_CONFUSION)
        iconIndex = 17; // Confusion
    else if (status2 & STATUS2_WRAPPED)
        iconIndex = 19; // Wrapped
    else if (status2 & STATUS2_FLINCHED)
        iconIndex = 16; // Flinched
    else if (status2 & STATUS2_INFATUATION)
        iconIndex = 18; // Infatuation
    else if (status2 & STATUS2_BIDE)
        iconIndex = 15; // Bide
    else if (status2 & STATUS2_FOCUS_ENERGY)
        iconIndex = 0; // Focus Energy (no icon, but placeholder for consistency)
    else
        return; // No icon for this status

    BlitMenuInfoIcon(iconTray->windowId, sSplitIcons_Gfx, status_icons[iconIndex], iconTray->offsetX + 2, iconTray->offsetY);
    iconTray->offsetX += status_icons[iconIndex].width + 2;
}

static void SetStatIcon(u32 monIndex, u8 stat)
{
    IconTray *iconTray = &sIconTrays[monIndex];
    struct BattlePokemon *battleMon = &gBattleMons[monIndex];
    s8 statStage = battleMon->statStages[stat] - 6;
    u32 iconIndex = statStage > 0 ? stat : stat + STAT_NEGATE;
    struct MoveMenuInfoIcon iconData = status_icons[iconIndex];

    // DebugPrintf("SetStatIcon: mon%d, stat=%d, stage=%d", monIndex, stat, statStage);

    if (statStage == 0)
        return; // No icon for neutral stat stage
    
    statStage = abs(statStage);
    
    BlitMenuInfoIcon(iconTray->windowId, sSplitIcons_Gfx, iconData, iconTray->offsetX + 2, iconTray->offsetY);
    iconTray->offsetX += iconData.width;

    if (statStage > 1)
    {
        iconData = status_icons[MULTIPLIER_ICON_INDEX + statStage]; // x1, x2, x3, x4, x5, x6
        BlitMenuInfoIcon(iconTray->windowId, sSplitIcons_Gfx, iconData, iconTray->offsetX + 2, iconTray->offsetY);
        iconTray->offsetX += iconData.width;
    }
}


void HealthbarUpdateStatus(s16 _unused)
{
    if (!sInitialized)
        Init();

    UpdateIconTray(0);
    UpdateIconTray(1);
    UpdateZoneIconTray();
}



void StatusIcons_SetOffset(u16 x, u16 y)
{
    sBGOffsetY = y;
    HealthbarUpdateStatus(0);
}