// pikachu_beach_main.c
#include "global.h"
#include "gflib.h"
#include "decompress.h"
#include "task.h"
#include "overworld.h"
#include "menu.h"
#include "new_menu_helpers.h"
#include "text_window.h"
#include "random.h"
#include "trig.h"
#include "strings.h"
#include "constants/songs.h"
#include "minigames/pikachu_beach_internal.h" // Include the new header

// Global EWRAM_DATA variables
EWRAM_DATA struct SurfMinigameState * sSurfMinigameState = NULL;

// Function prototypes (local to this file)
static void InitSurfMinigameState(struct SurfMinigameState * state);
static bool32 TryCreateMinigame(void);
static void CleanUpSurfMinigameState(void);
static void DestroyMinigame(void);
static void CB2_InitMinigame(void);
static void CB2_RunMinigame(void);


void  PlaySurfMinigame(MainCallback savedCallback)
{
    DebugPrintf("Play Surf minigame");
    ResetTasks();
    sSurfMinigameState = Alloc(sizeof(*sSurfMinigameState));
    if (sSurfMinigameState == NULL)
        SetMainCallback2(savedCallback);
    else
    {
        sSurfMinigameState->savedCallback = savedCallback;
        InitSurfMinigameState(sSurfMinigameState);
        SetMainCallback2(CB2_InitMinigame);
    }
}

void MainTask_SurfMinigameLoop(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    

    switch (data[0])
    {
    case 0:
        SurfMinigame_HandleInput();
        SurfMinigame_ScrollBG();
        break;
    case 1:
        break;
    
    default:
        break;
    }
}

static void InitSurfMinigameState(struct SurfMinigameState * state)
{
    state->taskId = 0;
}

static void CB2_InitMinigame(void)
{
    DebugPrintf("cb2 init mini game");
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
            SetSurfMinigameSetupTask(MG0TASK_GFX_INIT, 0);
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

    struct SurfMinigameSetupTaskData * ptr_taskData = Alloc(sizeof(struct SurfMinigameSetupTaskData));
    if (ptr_taskData == NULL)
        return FALSE;
    for (i = 0; i < (int)ARRAY_COUNT(ptr_taskData->subtasks); i++)
        ptr_taskData->subtasks[i].active = 0;
    ptr_taskData->yesNoMenuActive = TRUE;
    taskId = CreateTask(Task_SurfMinigame, 2);
    // sets task data[0] and data[1] to taskData 
    SetWordTaskArg(taskId, 0, (uintptr_t)ptr_taskData); // pointers are 32 bit -> need two s16 data slots -> "word"
    return TRUE;
}

static void CleanUpSurfMinigameState()
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
}