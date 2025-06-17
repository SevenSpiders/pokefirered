// pikachu_beach_subtasks.c
#include "global.h"
#include "gflib.h"
#include "task.h"
#include "menu.h"
#include "minigames/pikachu_beach_internal.h" // Include the new header

// Array of minigame setup subtasks (commented out in original, but structure implies task functions)
static bool8 (*const  sSurfMinigameSetupTasks[])(u8 *, struct SurfMinigameSetupTaskData *) = {
    [MG0TASK_GFX_INIT] =  MG0Task_InitGraphics, // implemented in pikachu_beach_graphics
    // [MG0TASK_FADEOUT_EXIT] = SlotsTask_FadeOut,
    // [MG0TASK_UPDATE_LINE_LIGHTS] = SlotsTask_UpdateLineStates,
    // [MG0TASK_CLEFAIRY_BOUNCE] = SlotsTask_ClefairyUpdateOnReelsStart,
    // [MG0TASK_ANIM_WIN] = SlotsTask_StartClefairyDanceAndWinningLineFlash,
    // [MG0TASK_END_ANIM_WIN] = SlotsTask_StopWinningLineFlashTask,
    // [MG0TASK_ANIM_LOSE] = SlotsTask_ClefairyFainted,
    // [MG0TASK_ANIM_BETTING] = SlotsTask_ClefairyNeutral,
    // [MG0TASK_SHOW_AMOUNTS] = SlotsTask_UpdateCoinsDisplay,
    // [MG0TASK_MSG_NO_COINS] = SlotsTask_MessageOutOfCoins,
    // [MG0TASK_ASK_QUIT] = SlotsTask_AskQuitPlaying,
    // [MG0TASK_DESTROY_YESNO] = SlotsTask_DestroyYesNoMenu,
    // [MG0TASK_PRESS_BUTTON] = SlotsTask_PressReelButton,
    // [MG0TASK_RELEASE_BUTTONS] = SlotsTask_ReleaseReelButtons,
    // [MG0TASK_SHOWHELP] = SlotsTask_ShowHelp,
    // [MG0TASK_HIDEHELP] = SlotsTask_HideHelp
};

// Global EWRAM_DATA variable (declared as extern in the header, defined in main.c)
// extern EWRAM_DATA struct SurfMinigameState * sSurfMinigameState;


void SetSurfMinigameSetupTask(u16 funcno, u8 taskId)
{
    struct SurfMinigameSetupTaskData * data = GetSurfMinigameSetupTaskDataPtr();
    data->subtasks[taskId].funcno = funcno;
    data->subtasks[taskId].state = 0;
    data->subtasks[taskId].active =  sSurfMinigameSetupTasks[funcno](&data->subtasks[taskId].state, data);
}

void MainTask_SurfMinigameLoop(u8 taskId)
{
    // s16 * data = gTasks[taskId].data;

    // switch (data[0])
    // {
    // case 0:
    //     // Betting Phase
    //     if (GetCoins() == 0)
    //     {
    //         SetMainTask(MainTask_NoCoinsGameOver);
    //     }
    //     else if (JOY_NEW(DPAD_DOWN))
    //     {
    //         sSlotMachineState->bet++;
    //         RemoveCoins(1);
    //         PlaySE(SE_RS_SHOP);
    //         SetSurfMinigameSetupTask(  MG0TASK_SHOW_AMOUNTS, 0);
    //         SetSurfMinigameSetupTask(  MG0TASK_UPDATE_LINE_LIGHTS, 1);
    //         data[0] = 1;
    //     }
    //     else if (JOY_NEW(R_BUTTON))
    //     {
    //         s32 toAdd = 3 - sSlotMachineState->bet;
    //         if (GetCoins() >= toAdd)
    //         {
    //             sSlotMachineState->bet = 3;
    //             RemoveCoins(toAdd);
    //         }
    //         else
    //         {
    //             sSlotMachineState->bet += GetCoins();
    //             SetCoins(0);
    //         }
    //         PlaySE(SE_RS_SHOP);
    //         SetSurfMinigameSetupTask(  MG0TASK_SHOW_AMOUNTS, 0);
    //         SetSurfMinigameSetupTask(  MG0TASK_UPDATE_LINE_LIGHTS, 1);
    //         data[0] = 1;
    //     }
    //     else if (JOY_NEW(A_BUTTON) && sSlotMachineState->bet != 0)
    //     {
    //         data[0] = 2;
    //     }
    //     else if (JOY_NEW(B_BUTTON))
    //     {
    //         SetMainTask(MainTask_ConfirmExitGame);
    //     }
    //     else if (JOY_NEW(DPAD_RIGHT))
    //     {
    //         SetMainTask(MainTask_ShowHelp);
    //     }
    //     break;
    // case 1:
    //     if (!IsSurfMinigameSetupTaskActive(0) && !IsSurfMinigameSetupTaskActive(1))
    //     {
    //         if (sSlotMachineState->bet == 3 || GetCoins() == 0)
    //             data[0] = 2;
    //         else
    //             data[0] = 0;
    //     }
    //     break;
    // case 2:
    //     SetQLPlayedTheSlots();
    //     CalcSlotBias();
    //     StartReels();
    //     sSlotMachineState->currentReel = 0;
    //     SetSurfMinigameSetupTask(  MG0TASK_CLEFAIRY_BOUNCE, 0);
    //     data[0] = 3;
    //     break;
    // case 3:
    //     if (!IsSurfMinigameSetupTaskActive(0))
    //     {
    //         if (JOY_NEW(A_BUTTON))
    //         {
    //             PlaySE(SE_CONTEST_PLACE);
    //             StopCurrentReel(sSlotMachineState->currentReel, sSlotMachineState->currentReel);
    //             PressReelButton(sSlotMachineState->currentReel, 0);
    //             data[0] = 4;
    //         }
    //     }
    //     break;
    // case 4:
    //     if (IsReelSpinning(sSlotMachineState->currentReel) == 0 && !IsSurfMinigameSetupTaskActive(0))
    //     {
    //         sSlotMachineState->currentReel++;
    //         if (sSlotMachineState->currentReel >= NUM_REELS)
    //         {
    //             sSlotMachineState->slotRewardClass = CalcPayout();
    //             sSlotMachineState->bet = 0;
    //             sSlotMachineState->currentReel = 0;
    //             if (sSlotMachineState->slotRewardClass == PAYOUT_NONE)
    //                 SetMainTask(MainTask_DarnNoPayout);
    //             else
    //             {
    //                 if (sSlotMachineState->slotRewardClass == PAYOUT_7)
    //                     IncrementGameStat(GAME_STAT_SLOT_JACKPOTS);
    //                 ResetMachineBias();
    //                 SetMainTask(MainTask_WinHandlePayout);
    //             }
    //         }
    //         else
    //             data[0] = 3;
    //     }
    //     break;
    // }
}



void Task_SurfMinigame(u8 taskId)
{
    SurfMinigameSetupTaskData * data = (void *)GetWordTaskArg(taskId, 0);
    s32 i;

    for (i = 0; i < (int)ARRAY_COUNT(data->subtasks); i++)
    {
        if (data->subtasks[i].active)
            data->subtasks[i].active =  sSurfMinigameSetupTasks[data->subtasks[i].funcno](&data->subtasks[i].state, data);
    }
}

struct SurfMinigameSetupTaskData * GetSurfMinigameSetupTaskDataPtr(void)
{
    return (void *)GetWordTaskArg(FindTaskIdByFunc(Task_SurfMinigame), 0);
}

bool32 IsSurfMinigameSetupTaskActive(u8 taskId)
{
    return GetSurfMinigameSetupTaskDataPtr()->subtasks[taskId].active;
}