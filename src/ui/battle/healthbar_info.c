#include "global.h"
#include "battle_interface.h"

static bool8 sIsShowingInfo;

static void ShowInfo(void)
{
    Healthbox_ShowInfo(gHealthboxSpriteIds[1]);
    DebugPrintf("Show info");
    // sIsShowingInfo = TRUE;
}

static void ResetHealthBar(void)
{
    Healthbox_HideInfo(gHealthboxSpriteIds[1]);
    UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], HEALTHBOX_ALL);
    DebugPrintf("Hide info");
}


void Healthbar_UpdateInfo(bool8 showInfo)
{
    if (showInfo == sIsShowingInfo)
        return;
    
    sIsShowingInfo = showInfo;

    if (showInfo)
        ShowInfo();
    else
        ResetHealthBar();
}
