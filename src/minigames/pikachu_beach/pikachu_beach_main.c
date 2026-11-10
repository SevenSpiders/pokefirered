#include "global.h"
#include "gflib.h"
#include "decompress.h"
#include "task.h"
#include "overworld.h"
#include "scanline_effect.h"
#include "new_menu_helpers.h"
#include "minigames/pikachu_beach_internal.h"

EWRAM_DATA struct SurfMinigameState *sSurfMinigameState = NULL;

static void InitSurfMinigameState(struct SurfMinigameState *state);
static bool32 TryCreateMinigame(void);
static void DestroyMinigame(void);
static void CB2_InitMinigame(void);
static void CB2_RunMinigame(void);

void PlaySurfMinigame(MainCallback savedCallback)
{
    ResetTasks();
    sSurfMinigameState = AllocZeroed(sizeof(*sSurfMinigameState));
    if (sSurfMinigameState == NULL)
    {
        SetMainCallback2(savedCallback);
        return;
    }

    sSurfMinigameState->savedCallback = savedCallback;
    InitSurfMinigameState(sSurfMinigameState);
    SetMainCallback2(CB2_InitMinigame);
}

static void InitSurfMinigameState(struct SurfMinigameState *state)
{
    state->taskId = 0;
    SurfMinigame_InitGameState(&state->game);
}

static void CB2_InitMinigame(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();

    switch (gMain.state)
    {
    case 0:
        if (TryCreateMinigame() == FALSE)
        {
            SetMainCallback2(sSurfMinigameState->savedCallback);
            CleanUpSurfMinigameState();
        }
        else
        {
            SetSurfMinigameSetupTask(SURF_TASK_GFX_INIT, 0);
            gMain.state++;
        }
        break;
    case 1:
        if (IsSurfMinigameSetupTaskActive(0) == FALSE)
        {
            sSurfMinigameState->taskId = CreateTask(MainTask_SurfMinigameLoop, 0);
            SetMainCallback2(CB2_RunMinigame);
        }
        break;
    }
}

static bool32 TryCreateMinigame(void)
{
    s32 i;
    u8 taskId;
    struct SurfMinigameSetupTaskData *ptr_taskData = AllocZeroed(sizeof(struct SurfMinigameSetupTaskData));

    if (ptr_taskData == NULL)
        return FALSE;

    for (i = 0; i < (int)ARRAY_COUNT(ptr_taskData->subtasks); i++)
        ptr_taskData->subtasks[i].active = FALSE;

    taskId = CreateTask(Task_SurfMinigame, 2);
    SetWordTaskArg(taskId, 0, (uintptr_t)ptr_taskData);
    return TRUE;
}

void CleanUpSurfMinigameState(void)
{
    DestroyMinigame();
    if (sSurfMinigameState != NULL)
    {
        Free(sSurfMinigameState);
        sSurfMinigameState = NULL;
    }
}

static void DestroyMinigame(void)
{
    ScanlineEffect_Stop();
    ScanlineEffect_Clear();
    SurfMinigame_StopBgm();
    {
        u8 mainTaskId = FindTaskIdByFunc(MainTask_ExitMinigame);
        if (mainTaskId == TASK_NONE)
            mainTaskId = FindTaskIdByFunc(MainTask_SurfMinigameLoop);
        if (mainTaskId != TASK_NONE)
            DestroyTask(mainTaskId);
    }
    if (FuncIsActiveTask(Task_SurfMinigame))
    {
        Free(GetSurfMinigameSetupTaskDataPtr());
        DestroyTask(FindTaskIdByFunc(Task_SurfMinigame));
    }
    DestroyGfxManager();
    FreeAllWindowBuffers();
}

static void CB2_RunMinigame(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    DoScheduledBgTilemapCopiesToVram();
}
