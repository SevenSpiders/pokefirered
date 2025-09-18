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
#define TRAY_ROW_HEIGHT 8
#define MAX_TRAY_WIDTH 12*8

typedef struct {
    u32 windowId;
    s32 offsetX;
    s32 offsetY;
    u32 row;
    u32 tilemapTop;
} IconTray;



static void Init();
static void ClearIconTray(u32 trayIndex);
static void AddStatusIcon(IconTray *iconTray, u32 iconIndex);
static void UpdateIconTray(u32 trayIndex);
static void SetStatusIcons(u32 monIndex);
static void SetStatus2Icons(u32 monIndex);
static void SetStatIcons(u32 trayIndex);
static void SetStatIcon(u32 monIndex, u8 stat);
static void IconTray_AddRow(IconTray *iconTray);
static void IconTray_Clear(IconTray *iconTray);

// static bool32 sInitialized;
static u32 sBGOffsetY;
static IconTray sIconTrays[3]; // 0: player, 1: enemy, 2: zone



static void Init()
{
    Icons_Init();
    // sInitialized = TRUE;

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
    IconTray_Clear(&sIconTrays[trayIndex]);
}

static void AddStatusIcon(IconTray *iconTray, u32 iconIndex)
{
    DebugPrintf("add icon %d", iconIndex);
    BlitIcon(iconTray->windowId, icons_status[iconIndex], iconTray->offsetX + 2, iconTray->offsetY);
    iconTray->offsetX += icons_status[iconIndex].width;
}

static void AddZoneIcon(u32 iconIndex, bool32 side)
{
    u32 width = icons_status[iconIndex].width;
    u32 offsetX = 104 - width - sIconTrays[TRAY_ZONE].offsetX - 4;
    BlitIcon(sIconTrays[TRAY_ZONE].windowId, icons_zone[side], offsetX, sIconTrays[TRAY_ZONE].offsetY);
    BlitIcon(sIconTrays[TRAY_ZONE].windowId, icons_zone[iconIndex], offsetX+1, sIconTrays[TRAY_ZONE].offsetY+1);
    sIconTrays[TRAY_ZONE].offsetX += width + 3;
}

static void UpdateIconTray(u32 trayIndex)
{
    u32 tilemapTop =  sBGOffsetY / 8 + sIconTrays[trayIndex].tilemapTop;
    SetWindowAttribute(sIconTrays[trayIndex].windowId, WINDOW_TILEMAP_TOP, tilemapTop);
    ClearIconTray(trayIndex);
    SetStatusIcons(trayIndex);

    if (sIconTrays[trayIndex].offsetX > 5) // seperation between status and stats
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
    struct BattlePokemon *battleMon = &gBattleMons[monIndex];
    u32 status2 = battleMon->status2;
    u32 status, j;

    for( status=1; status<STATUS_END; status++) // status 0 is STATUS_NONE
    {
        if (BattleMon_HasStatus(battleMon, status))
        {
            AddStatusIcon(iconTray, status);
        }
    }
}


static void SetStatIcons(u32 trayIndex)
{
    u32 i;
    for (i=1; i< NUM_BATTLE_STATS; i++) // STAT 0 is STAT_HP
    {
        SetStatIcon(trayIndex, i); // STAT_ATK, STAT_DEF, STAT_SPEED, STAT_SPATK, STAT_SPDEF, STAT_ACC, STAT_EVASION
    }
}

static void SetStatIcon(u32 monIndex, u8 stat)
{
    IconTray *iconTray = &sIconTrays[monIndex];
    BattlePokemon *battleMon = &gBattleMons[monIndex];
    s8 statStage = battleMon->statStages[stat] - 6; // -6
    u32 statIconIndex = statStage > 0 ? stat : stat + STAT_NEGATE;
    IconData statIcon, numberIcon;

    // DebugPrintf("SetStatIcon: mon%d, stat=%d, stage=%d", monIndex, stat, statStage);
    
    if (statStage == 0)
        return; // No icon for neutral stat stage
    
    statIcon = icons_stats[statIconIndex];
    statStage = abs(statStage);

    if (statStage > 1)
    {
        if (iconTray->offsetX + statIcon.width + numberIcon.width > MAX_TRAY_WIDTH)
            IconTray_AddRow(iconTray);
        numberIcon = icons_numbers[statStage]; // x1, x2, x3, x4, x5, x6
        BlitIcon(
            iconTray->windowId, 
            numberIcon, 
            iconTray->offsetX + statIcon.width + 1, 
            iconTray->offsetY + iconTray->row * TRAY_ROW_HEIGHT
        );
    }
    else
    {
        if (iconTray->offsetX + statIcon.width > MAX_TRAY_WIDTH)
            IconTray_AddRow(iconTray);
    }

    BlitIcon(
        iconTray->windowId, 
        statIcon, iconTray->offsetX + 2, 
        iconTray->offsetY + iconTray->row*TRAY_ROW_HEIGHT
    );
    iconTray->offsetX += statIcon.width;

    if (statStage > 1)
        iconTray->offsetX += numberIcon.width;
}

static void IconTray_AddRow(IconTray *iconTray)
{
    iconTray->row ++;
    iconTray->offsetX = -2 * iconTray->row;
}

static void IconTray_Clear(IconTray *iconTray)
{
    iconTray->row = 0;
    iconTray->offsetX = 0;
}


void HealthbarUpdateStatus(s16 _unused)
{
    // if (!sInitialized)
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