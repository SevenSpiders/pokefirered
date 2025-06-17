/*

#include "global.h"
#include "gflib.h"
#include "decompress.h"
#include "task.h"
#include "overworld.h" // needed?
#include "menu.h"
#include "new_menu_helpers.h"
#include "text_window.h"
#include "random.h"
#include "trig.h"
#include "strings.h"
#include "constants/songs.h"




typedef struct SurfMinigameState
{
    MainCallback savedCallback;
    u8 taskId;
} SurfMinigameState;

struct SurfMinigameSetupTaskDataSub_0000
{
    u16 funcno;
    u8 state;
    bool8 active;
};

enum {
     SMGTASK_GFX_INIT,
     SMGTASK_FADEOUT_EXIT,
     SMGTASK_UPDATE_LINE_LIGHTS,
     SMGTASK_CLEFAIRY_BOUNCE,
     SMGTASK_ANIM_WIN,
     SMGTASK_END_ANIM_WIN,
     SMGTASK_ANIM_LOSE,
     SMGTASK_ANIM_BETTING,
     SMGTASK_SHOW_AMOUNTS,
     SMGTASK_MSG_NO_COINS,
     SMGTASK_ASK_QUIT,
     SMGTASK_DESTROY_YESNO,
     SMGTASK_PRESS_BUTTON,
     SMGTASK_RELEASE_BUTTONS,
     SMGTASK_SHOWHELP,
     SMGTASK_HIDEHELP,
};

typedef struct SurfMinigameSetupTaskData
{
    struct SurfMinigameSetupTaskDataSub_0000 tasks[8];
    u8 reelButtonToPress;
    // align 2
    s32 bg1X;
    bool32 yesNoMenuActive;
    u8 unusedBuffer[BG_SCREEN_SIZE];
    u8 bg0TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg1TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg2TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg3TilemapBuffer[BG_SCREEN_SIZE];
} SurfMinigameSetupTaskData;

struct SurfMinigameGfxManager
{
    // struct Sprite *reelIconSprites[NUM_REELS][REEL_LOAD_LENGTH];
    // struct Sprite *creditDigitSprites[NUM_DIGIT_SPRITES];
    // struct Sprite *payoutDigitSprites[NUM_DIGIT_SPRITES];
    // struct Sprite *clefairySprites[2];
    // vu16 * reelIconAffineParamPtr;
};

static EWRAM_DATA struct SurfMinigameState * sSurfMinigameState = NULL;
static EWRAM_DATA struct SurfMinigameGfxManager * sSurfMinigameGfxManager = NULL;

bool8 (*const  sSurfMinigameSetupTasks[])(u8 *, struct SurfMinigameSetupTaskData *) = {
    // [ SMGTASK_GFX_INIT] = SlotsTask_GraphicsInit,
    // [ SMGTASK_FADEOUT_EXIT] = SlotsTask_FadeOut,
    // [ SMGTASK_UPDATE_LINE_LIGHTS] = SlotsTask_UpdateLineStates,
    // [ SMGTASK_CLEFAIRY_BOUNCE] = SlotsTask_ClefairyUpdateOnReelsStart,
    // [ SMGTASK_ANIM_WIN] = SlotsTask_StartClefairyDanceAndWinningLineFlash,
    // [ SMGTASK_END_ANIM_WIN] = SlotsTask_StopWinningLineFlashTask,
    // [ SMGTASK_ANIM_LOSE] = SlotsTask_ClefairyFainted,
    // [ SMGTASK_ANIM_BETTING] = SlotsTask_ClefairyNeutral,
    // [ SMGTASK_SHOW_AMOUNTS] = SlotsTask_UpdateCoinsDisplay,
    // [ SMGTASK_MSG_NO_COINS] = SlotsTask_MessageOutOfCoins,
    // [ SMGTASK_ASK_QUIT] = SlotsTask_AskQuitPlaying,
    // [ SMGTASK_DESTROY_YESNO] = SlotsTask_DestroyYesNoMenu,
    // [ SMGTASK_PRESS_BUTTON] = SlotsTask_PressReelButton,
    // [ SMGTASK_RELEASE_BUTTONS] = SlotsTask_ReleaseReelButtons,
    // [ SMGTASK_SHOWHELP] = SlotsTask_ShowHelp,
    // [ SMGTASK_HIDEHELP] = SlotsTask_HideHelp
};

// Function prototypes
static void CB2_InitMinigame(void);
static void InitSurfMinigameState(struct SurfMinigameState * state);
static bool32 TryCreateMinigame(void);
static void CleanUpSurfMinigameState(void);
static void DestroyMinigame(void);
static void SetSurfMinigameSetupTask(u16 funcno, u8 taskId);
static void MainTask_SurfMinigameLoop(u8 taskId);
static void Task_SurfMinigame(u8 taskId);
static void CB2_RunMinigame(void);
static bool32 IsSurfMinigameSetupTaskActive(u8 taskId);
static struct SurfMinigameSetupTaskData * GetSurfMinigameSetupTaskDataPtr(void);
static void DestroyGfxManager(void);


void  PlaySurfMinigame(MainCallback savedCallback)
{
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


static void InitSurfMinigameState(struct SurfMinigameState * state)
{
    state->taskId = 0;
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
            SetSurfMinigameSetupTask( SMGTASK_GFX_INIT, 0);
            gMain.state++;
        }
        break;
    case 1:
        if (!IsSurfMinigameSetupTaskActive(0))
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

    struct SurfMinigameSetupTaskData * ptr = Alloc(sizeof(struct SurfMinigameSetupTaskData));
    if (ptr == NULL)
        return FALSE;
    for (i = 0; i < (int)ARRAY_COUNT(ptr->tasks); i++)
        ptr->tasks[i].active = 0;
    ptr->yesNoMenuActive = TRUE;
    SetWordTaskArg(CreateTask(Task_SurfMinigame, 2), 0, (uintptr_t)ptr);
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

static void DestroyGfxManager(void)
{
    if (sSurfMinigameGfxManager != NULL)
    {
        Free(sSurfMinigameGfxManager);
        sSurfMinigameGfxManager = NULL;
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


static void MainTask_SurfMinigameLoop(u8 taskId)
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
    //         SetSurfMinigameSetupTask( SMGTASK_SHOW_AMOUNTS, 0);
    //         SetSurfMinigameSetupTask( SMGTASK_UPDATE_LINE_LIGHTS, 1);
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
    //         SetSurfMinigameSetupTask( SMGTASK_SHOW_AMOUNTS, 0);
    //         SetSurfMinigameSetupTask( SMGTASK_UPDATE_LINE_LIGHTS, 1);
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
    //     SetSurfMinigameSetupTask( SMGTASK_CLEFAIRY_BOUNCE, 0);
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

static void CB2_RunMinigame(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void SetSurfMinigameSetupTask(u16 funcno, u8 taskId)
{
    struct SurfMinigameSetupTaskData * ptr = GetSurfMinigameSetupTaskDataPtr();
    ptr->tasks[taskId].funcno = funcno;
    ptr->tasks[taskId].state = 0;
    ptr->tasks[taskId].active =  sSurfMinigameSetupTasks[funcno](&ptr->tasks[taskId].state, ptr);
}

static void Task_SurfMinigame(u8 taskId)
{
    struct SurfMinigameSetupTaskData * ptr = (void *)GetWordTaskArg(taskId, 0);
    s32 i;

    for (i = 0; i < (int)ARRAY_COUNT(ptr->tasks); i++)
    {
        if (ptr->tasks[i].active)
            ptr->tasks[i].active =  sSurfMinigameSetupTasks[ptr->tasks[i].funcno](&ptr->tasks[i].state, ptr);
    }
}

static struct SurfMinigameSetupTaskData * GetSurfMinigameSetupTaskDataPtr(void)
{
    return (void *)GetWordTaskArg(FindTaskIdByFunc(Task_SurfMinigame), 0);
}

static bool32 IsSurfMinigameSetupTaskActive(u8 taskId)
{
    return GetSurfMinigameSetupTaskDataPtr()->tasks[taskId].active;
}

*/