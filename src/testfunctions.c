#include "global.h"
#include "gflib.h"
#include "overworld.h"
#include "battle_setup.h"
#include "testfunctions.h"

extern u16 gTrainerBattleOpponent_A;

static u32 sCurrentTrainer;


void TestFunction32()
{
    DebugPrintf("TEST BATTLE");
    if (sCurrentTrainer == 0) sCurrentTrainer = TRAINER_YOUNGSTER_BEN;
    gTrainerBattleOpponent_A = sCurrentTrainer;
    sCurrentTrainer++;
    StartTrainerBattle();
}
