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
#include "ui/statusIcons.h"

#define TRAY_PLAYER1 0
#define TRAY_OPPONENT1 1
// #define TRAY_PLAYER2 2 
// #define TRAY_OPPONENT2 3
#define TRAY_ZONE 2

typedef struct {
    u32 windowId;
    u32 offsetX;
    u32 offsetY;
    u32 tilemapTop;
} IconTray;



static void Init();
static void ClearIconTray(u32 trayIndex);
static void AddIcon(IconTray *iconTray, u32 iconIndex);
static void UpdateIconTray(u32 trayIndex);
static void SetStatusIcons(u32 monIndex);
static void SetStatus2Icons(u32 monIndex);
static void SetStatIcons(u32 trayIndex);
static void SetStatIcon(u32 monIndex, u8 stat);


static bool32 sInitialized;
static u32 sBGOffsetY;
static IconTray sIconTrays[3]; // 0: player, 1: enemy, 2: zone



static void Init()
{
    Icons_Init();
    sInitialized = TRUE;

    sIconTrays[TRAY_PLAYER1].windowId = B_WIN_ICONS_PLAYER1; // player
    sIconTrays[TRAY_PLAYER1].tilemapTop = 12; // y offset for player icons
    sIconTrays[TRAY_PLAYER1].offsetY = 3; // 3 pixel from top

    sIconTrays[TRAY_OPPONENT1].windowId = B_WIN_ICONS_OPPONENT1; // enemy
    sIconTrays[TRAY_OPPONENT1].tilemapTop = 5; // y offset for enemy icons
    sIconTrays[TRAY_OPPONENT1].offsetY = 0;

    sIconTrays[TRAY_ZONE].windowId = B_WIN_ICONS_ZONE; // player
    sIconTrays[TRAY_ZONE].tilemapTop = 0; // y offset for player icons
    sIconTrays[TRAY_ZONE].offsetY = 2; // 3 pixel from top
}

static void ClearIconTray(u32 trayIndex)
{
    FillWindowPixelBuffer(sIconTrays[trayIndex].windowId, PIXEL_FILL(0));
    sIconTrays[trayIndex].offsetX = 0;
}

static void AddIcon(IconTray *iconTray, u32 iconIndex)
{
    BlitIcon(iconTray->windowId, sIcons_Gfx, icons_status[iconIndex], iconTray->offsetX + 2, iconTray->offsetY);
    iconTray->offsetX += icons_status[iconIndex].width;
}

static void AddZoneIcon(u32 iconIndex, bool32 side)
{
    u32 width = icons_status[iconIndex].width;
    u32 offsetX = 104 - width - sIconTrays[TRAY_ZONE].offsetX - 4;
    BlitIcon(sIconTrays[TRAY_ZONE].windowId, sIcons_Gfx, icons_zone[side], offsetX, sIconTrays[TRAY_ZONE].offsetY);
    BlitIcon(sIconTrays[TRAY_ZONE].windowId, sIcons_Gfx, icons_zone[iconIndex], offsetX+1, sIconTrays[TRAY_ZONE].offsetY+1);
    sIconTrays[TRAY_ZONE].offsetX += width + 3;
}

static void UpdateIconTray(u32 trayIndex)
{
    u32 tilemapTop =  sBGOffsetY / 8 + sIconTrays[trayIndex].tilemapTop;
    SetWindowAttribute(sIconTrays[trayIndex].windowId, WINDOW_TILEMAP_TOP, tilemapTop);
    ClearIconTray(trayIndex);
    SetStatusIcons(trayIndex);
    SetStatus2Icons(trayIndex);
    if (sIconTrays[trayIndex].offsetX > 5)
        sIconTrays[trayIndex].offsetX += 2;

    SetStatIcons(trayIndex);

    if (trayIndex == 0)
        Healthbox_ShowHPText(0, sIconTrays[trayIndex].offsetX < 50);

    PutWindowTilemap(sIconTrays[trayIndex].windowId);
    CopyWindowToVram(sIconTrays[trayIndex].windowId, COPYWIN_FULL);
}

static void UpdateZoneIconTray()
{
    u32 i;
    u32 tilemapTop =  sBGOffsetY / 8 + sIconTrays[TRAY_ZONE].tilemapTop;
    // for zone effects like weather, spikes, light screen etc.
    // u32 side = 0;
    // gSideStatuses[side] & SIDE_STATUS_REFLECT
    SetWindowAttribute(sIconTrays[TRAY_ZONE].windowId, WINDOW_TILEMAP_TOP, tilemapTop);
    for (i=0; i < 6; i++)
    {
        // AddZoneIcon(i+2, i%2);
    }
    PutWindowTilemap(sIconTrays[TRAY_ZONE].windowId);
    CopyWindowToVram(sIconTrays[TRAY_ZONE].windowId, COPYWIN_FULL);
}

static void SetStatusIcons(u32 monIndex)
{
    IconTray *iconTray = &sIconTrays[monIndex];
    BattlePokemon *battleMon = &gBattleMons[monIndex];
    u32 status = battleMon->status1;
    u32 iconIndex = 0;

    // DebugPrintf("SetStatusIcons: mon%d, status=0x%08x", monIndex, status);

    if (status == STATUS1_NONE)
        return;

    if (status & STATUS1_PSN_ANY)
        AddIcon(iconTray, 20);
    else if (status & STATUS1_PARALYSIS)
        AddIcon(iconTray, 21);
    else if (status & STATUS1_SLEEP)
        AddIcon(iconTray, 22);
    else if (status & STATUS1_FREEZE)
        AddIcon(iconTray, 23);
    else if (status & STATUS1_BURN)
        AddIcon(iconTray, 24);
    else
        DebugPrintf("unknown status1 %d", status);
}

static void SetStatus2Icons(u32 monIndex)
{
    IconTray *iconTray = &sIconTrays[monIndex];
    struct BattlePokemon *battleMon = &gBattleMons[monIndex];
    u32 status2 = battleMon->status2;


    if (status2 == 0)
        return;

    if (status2 & STATUS2_CONFUSION)
        AddIcon(iconTray, 17); // Confusion
    if (status2 & STATUS2_WRAPPED)
        AddIcon(iconTray, 19); // Wrapped
    if (status2 & STATUS2_FLINCHED)
        AddIcon(iconTray, 16); // Flinched
    if (status2 & STATUS2_INFATUATION)
        AddIcon(iconTray, 15); // Infatuation (Critical Hit icon)
    if (status2 & STATUS2_BIDE)
        AddIcon(iconTray, 0); // Bide (no icon, but placeholder for consistency)
    if (status2 & STATUS2_FOCUS_ENERGY)
        AddIcon(iconTray, 0); // Focus Energy (no icon, but placeholder for consistency)    
}

static void SetStatus3Icons(u32 monIndex)
{
    // IconTray *iconTray = &sIconTrays[monIndex];
    // struct BattlePokemon *battleMon = &gBattleMons[monIndex];
    // u32 status3 = battleMon->status3;

    // if (status3 == 0)
    //     return;

    // if (status3 & STATUS3_LEECH_SEED)
    //     AddIcon(iconTray, 0); // Leech Seed (no icon, but placeholder for consistency)
    // if (status3 & STATUS3_CURSE)
    //     AddIcon(iconTray, 0); // Curse (no icon, but placeholder for consistency)
}

static void SetStatIcons(u32 trayIndex)
{
    u32 i;
    for (i=1; i< NUM_BATTLE_STATS; i++)
    {
        SetStatIcon(trayIndex, i); // STAT_ATK, STAT_DEF, STAT_SPEED, STAT_SPATK, STAT_SPDEF, STAT_ACC, STAT_EVASION
    }
}

static void SetStatIcon(u32 monIndex, u8 stat)
{
    IconTray *iconTray = &sIconTrays[monIndex];
    struct BattlePokemon *battleMon = &gBattleMons[monIndex];
    s8 statStage = battleMon->statStages[stat] - 6;
    u32 iconIndex = statStage > 0 ? stat : stat + STAT_NEGATE;
    IconData iconData = icons_status[iconIndex];

    // DebugPrintf("SetStatIcon: mon%d, stat=%d, stage=%d", monIndex, stat, statStage);

    if (statStage == 0)
        return; // No icon for neutral stat stage
    
    statStage = abs(statStage);
    BlitIcon(iconTray->windowId, sIcons_Gfx, iconData, iconTray->offsetX + 2, iconTray->offsetY);
    iconTray->offsetX += iconData.width;

    if (statStage > 1)
    {
        iconData = icons_numbers[statStage]; // x1, x2, x3, x4, x5, x6
        BlitIcon(iconTray->windowId, sIcons_Gfx, iconData, iconTray->offsetX + 2, iconTray->offsetY);
        iconTray->offsetX += iconData.width;
    }
}


void HealthbarUpdateStatus(s16 _unused)
{
    if (!sInitialized)
        Init();

    UpdateIconTray(TRAY_PLAYER1);
    UpdateIconTray(TRAY_OPPONENT1);
    UpdateZoneIconTray();
}



void StatusIcons_SetOffset(u16 x, u16 y)
{
    sBGOffsetY = y;
    HealthbarUpdateStatus(0);
}