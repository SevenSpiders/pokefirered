#include "global.h"
#include "gflib.h"
#include "task.h"
#include "scanline_effect.h"
#include "minigames/pikachu_beach_internal.h"

static bool8 (*const sSurfMinigameSetupTasks[])(u8 *, struct SurfMinigameSetupTaskData *) = {
    [SURF_TASK_GFX_INIT] = SurfTask_InitGraphics,
    [SURF_TASK_FADEOUT_EXIT] = SurfTask_FadeOutExit,
};

void SetMainTask(TaskFunc taskFunc)
{
    gTasks[sSurfMinigameState->taskId].func = taskFunc;
    gTasks[sSurfMinigameState->taskId].data[0] = 0;
}

void SetSurfMinigameSetupTask(u16 funcno, u8 taskId)
{
    struct SurfMinigameSetupTaskData *data = GetSurfMinigameSetupTaskDataPtr();

    data->subtasks[taskId].funcno = funcno;
    data->subtasks[taskId].state = 0;
    data->subtasks[taskId].active = sSurfMinigameSetupTasks[funcno](&data->subtasks[taskId].state, data);
}

void MainTask_SurfMinigameLoop(u8 taskId)
{
    struct SurfGame *game = GetSurfGame();

    SurfMinigame_HandleInput();
    if (gTasks[taskId].func != MainTask_SurfMinigameLoop)
        return;

    SurfMinigame_RunRoutine();
    SurfMinigame_UpdatePikachu();
    SurfMinigame_MoveClouds();
    SurfMinigame_UpdatePikachuSprites();
    SurfMinigame_ApplyScroll();

    if (game->routineNumber == SURF_ROUTINE_PLAY)
        SurfMinigame_DrawHud();

    if (game->routineNumber & SURF_ROUTINE_EXIT_FLAG)
        SetMainTask(MainTask_ExitMinigame);
}

void MainTask_ConfirmExitGame(u8 taskId)
{
    SetMainTask(MainTask_ExitMinigame);
}

void MainTask_ExitMinigame(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        SetSurfMinigameSetupTask(SURF_TASK_FADEOUT_EXIT, 0);
        data[0]++;
        break;
    case 1:
        if (!IsSurfMinigameSetupTaskActive(0))
        {
            SetMainCallback2(sSurfMinigameState->savedCallback);
            CleanUpSurfMinigameState();
        }
        break;
    }
}

void Task_SurfMinigame(u8 taskId)
{
    SurfMinigameSetupTaskData *data = (void *)GetWordTaskArg(taskId, 0);
    s32 i;

    for (i = 0; i < (int)ARRAY_COUNT(data->subtasks); i++)
    {
        if (data->subtasks[i].active)
            data->subtasks[i].active = sSurfMinigameSetupTasks[data->subtasks[i].funcno](&data->subtasks[i].state, data);
    }
}

struct SurfMinigameSetupTaskData *GetSurfMinigameSetupTaskDataPtr(void)
{
    return (void *)GetWordTaskArg(FindTaskIdByFunc(Task_SurfMinigame), 0);
}

bool32 IsSurfMinigameSetupTaskActive(u8 taskId)
{
    return GetSurfMinigameSetupTaskDataPtr()->subtasks[taskId].active;
}
