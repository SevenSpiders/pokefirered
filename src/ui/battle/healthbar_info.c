#include "global.h"
#include "battle_interface.h"
#include "palette.h"

static bool8 sIsShowingInfo;

#define PALETTE_ICONS (1 << 10)
#define PALETTE_TEXT_BOX (1 << 0)
#define PALETTE_HP (1 << 1)
#define PALETTE_TEXT (1 << 5)
#define PALETTES_TO_FADE PALETTES_ALL - PALETTE_ICONS - PALETTE_TEXT_BOX - PALETTE_HP - PALETTE_TEXT
// #define PALETTES_TO_FADE PALETTES_OBJECTS

static void ShowInfo(void)
{
    Healthbox_Blank(gHealthboxSpriteIds[1]);
    // show info window
    DebugPrintf("Show info");
    BeginNormalPaletteFade(PALETTES_TO_FADE, 0, 0, 8, RGB_BLACK);
}

static void ResetHealthBar(void)
{
    Healthbox_HideInfo(gHealthboxSpriteIds[1]);
    UpdateHealthboxAttribute(gHealthboxSpriteIds[1], &gEnemyParty[gBattlerPartyIndexes[0]], HEALTHBOX_ALL);
    DebugPrintf("Hide info");
    BeginNormalPaletteFade(PALETTES_TO_FADE, 0, 8, 0, RGB_BLACK);
}


void Healthbox_UpdateInfo(bool8 showInfo)
{
    if (showInfo == sIsShowingInfo)
        return;
    
    sIsShowingInfo = showInfo;

    if (showInfo)
        ShowInfo();
    else
        ResetHealthBar();
}
