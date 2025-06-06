#include "global.h"
#include "battle_interface.h"

static bool8 sIsShowingInfo;

static void ShowInfo(void)
{
    Healthbox_Blank(gHealthboxSpriteIds[1]);
    // show info window
    DebugPrintf("Show info");
}

static void ResetHealthBar(void)
{
    Healthbox_HideInfo(gHealthboxSpriteIds[1]);
    UpdateHealthboxAttribute(gHealthboxSpriteIds[1], &gEnemyParty[gBattlerPartyIndexes[0]], HEALTHBOX_ALL);
    DebugPrintf("Hide info");
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
