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




typedef struct MinigameState
{
    MainCallback savedCallback;
    u8 taskId;
} MinigameState;

struct MinigameSetupTaskDataSub_0000
{
    u16 funcno;
    u8 state;
    bool8 active;
};

enum {
    BEACHTASK_GFX_INIT,
    BEACHTASK_FADEOUT_EXIT,
    BEACHTASK_UPDATE_LINE_LIGHTS,
    BEACHTASK_CLEFAIRY_BOUNCE,
    BEACHTASK_ANIM_WIN,
    BEACHTASK_END_ANIM_WIN,
    BEACHTASK_ANIM_LOSE,
    BEACHTASK_ANIM_BETTING,
    BEACHTASK_SHOW_AMOUNTS,
    BEACHTASK_MSG_NO_COINS,
    BEACHTASK_ASK_QUIT,
    BEACHTASK_DESTROY_YESNO,
    BEACHTASK_PRESS_BUTTON,
    BEACHTASK_RELEASE_BUTTONS,
    BEACHTASK_SHOWHELP,
    BEACHTASK_HIDEHELP,
};

typedef struct MinigameSetupTaskData
{
    struct MinigameSetupTaskDataSub_0000 tasks[8];
    u8 reelButtonToPress;
    // align 2
    s32 bg1X;
    bool32 yesNoMenuActive;
    u8 unusedBuffer[BG_SCREEN_SIZE];
    u8 bg0TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg1TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg2TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg3TilemapBuffer[BG_SCREEN_SIZE];
} MinigameSetupTaskData;

struct MinigameGfxManager
{
    // struct Sprite *reelIconSprites[NUM_REELS][REEL_LOAD_LENGTH];
    // struct Sprite *creditDigitSprites[NUM_DIGIT_SPRITES];
    // struct Sprite *payoutDigitSprites[NUM_DIGIT_SPRITES];
    // struct Sprite *clefairySprites[2];
    // vu16 * reelIconAffineParamPtr;
};

static EWRAM_DATA struct MinigameState * sMinigameState = NULL;
static EWRAM_DATA struct MinigameGfxManager * sMinigameGfxManager = NULL;

bool8 (*const sBeachMinigameSetupTasks[])(u8 *, struct MinigameSetupTaskData *) = {
    // [BEACHTASK_GFX_INIT] = SlotsTask_GraphicsInit,
    // [BEACHTASK_FADEOUT_EXIT] = SlotsTask_FadeOut,
    // [BEACHTASK_UPDATE_LINE_LIGHTS] = SlotsTask_UpdateLineStates,
    // [BEACHTASK_CLEFAIRY_BOUNCE] = SlotsTask_ClefairyUpdateOnReelsStart,
    // [BEACHTASK_ANIM_WIN] = SlotsTask_StartClefairyDanceAndWinningLineFlash,
    // [BEACHTASK_END_ANIM_WIN] = SlotsTask_StopWinningLineFlashTask,
    // [BEACHTASK_ANIM_LOSE] = SlotsTask_ClefairyFainted,
    // [BEACHTASK_ANIM_BETTING] = SlotsTask_ClefairyNeutral,
    // [BEACHTASK_SHOW_AMOUNTS] = SlotsTask_UpdateCoinsDisplay,
    // [BEACHTASK_MSG_NO_COINS] = SlotsTask_MessageOutOfCoins,
    // [BEACHTASK_ASK_QUIT] = SlotsTask_AskQuitPlaying,
    // [BEACHTASK_DESTROY_YESNO] = SlotsTask_DestroyYesNoMenu,
    // [BEACHTASK_PRESS_BUTTON] = SlotsTask_PressReelButton,
    // [BEACHTASK_RELEASE_BUTTONS] = SlotsTask_ReleaseReelButtons,
    // [BEACHTASK_SHOWHELP] = SlotsTask_ShowHelp,
    // [BEACHTASK_HIDEHELP] = SlotsTask_HideHelp
};

// Function prototypes
static void CB2_InitMinigame(void);
static void InitMinigameState(struct MinigameState * state);
static bool32 TryCreateMinigame(void);
static void CleanUpMinigameState(void);
static void DestroyMinigame(void);
static void SetMinigameSetupTask(u16 funcno, u8 taskId);
static void MainTask_MinigameLoop(u8 taskId);
static void Task_Minigame(u8 taskId);
static void CB2_RunMinigame(void);
static bool32 IsMinigameSetupTaskActive(u8 taskId);
static struct MinigameSetupTaskData * GetMinigameSetupTaskDataPtr(void);
static void DestroyGfxManager(void);


void PlaySurfingPikachu(MainCallback savedCallback)
{
    ResetTasks();
    sMinigameState = Alloc(sizeof(*sMinigameState));
    if (sMinigameState == NULL)
        SetMainCallback2(savedCallback);
    else
    {
        sMinigameState->savedCallback = savedCallback;
        InitMinigameState(sMinigameState);
        SetMainCallback2(CB2_InitMinigame);
    }
}


static void InitMinigameState(struct MinigameState * state)
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
            SetMainCallback2(sMinigameState->savedCallback);
            CleanUpMinigameState();
        }
        else
        {
            SetMinigameSetupTask(BEACHTASK_GFX_INIT, 0);
            gMain.state++;
        }
        break;
    case 1:
        if (!IsMinigameSetupTaskActive(0))
        {
            sMinigameState->taskId = CreateTask(MainTask_MinigameLoop, 0);
            SetMainCallback2(CB2_RunMinigame);
        }
        break;
    }
}

static bool32 TryCreateMinigame(void)
{
    s32 i;

    struct MinigameSetupTaskData * ptr = Alloc(sizeof(struct MinigameSetupTaskData));
    if (ptr == NULL)
        return FALSE;
    for (i = 0; i < (int)ARRAY_COUNT(ptr->tasks); i++)
        ptr->tasks[i].active = 0;
    ptr->yesNoMenuActive = TRUE;
    SetWordTaskArg(CreateTask(Task_Minigame, 2), 0, (uintptr_t)ptr);
    return TRUE;
} 

static void CleanUpMinigameState()
{
    DestroyMinigame();
    if (sMinigameState != NULL)
    {
        Free(sMinigameState);
        sMinigameState = NULL;
    }
}

static void DestroyGfxManager(void)
{
    if (sMinigameGfxManager != NULL)
    {
        Free(sMinigameGfxManager);
        sMinigameGfxManager = NULL;
    }
}

static void DestroyMinigame(void)
{
    if (FuncIsActiveTask(Task_Minigame))
    {
        Free(GetMinigameSetupTaskDataPtr());
        DestroyTask(FindTaskIdByFunc(Task_Minigame));
    }
    DestroyGfxManager();
    FreeAllWindowBuffers();
}


static void MainTask_MinigameLoop(u8 taskId)
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
    //         SetMinigameSetupTask(BEACHTASK_SHOW_AMOUNTS, 0);
    //         SetMinigameSetupTask(BEACHTASK_UPDATE_LINE_LIGHTS, 1);
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
    //         SetMinigameSetupTask(BEACHTASK_SHOW_AMOUNTS, 0);
    //         SetMinigameSetupTask(BEACHTASK_UPDATE_LINE_LIGHTS, 1);
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
    //     if (!IsMinigameSetupTaskActive(0) && !IsMinigameSetupTaskActive(1))
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
    //     SetMinigameSetupTask(BEACHTASK_CLEFAIRY_BOUNCE, 0);
    //     data[0] = 3;
    //     break;
    // case 3:
    //     if (!IsMinigameSetupTaskActive(0))
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
    //     if (IsReelSpinning(sSlotMachineState->currentReel) == 0 && !IsMinigameSetupTaskActive(0))
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

static void SetMinigameSetupTask(u16 funcno, u8 taskId)
{
    struct MinigameSetupTaskData * ptr = GetMinigameSetupTaskDataPtr();
    ptr->tasks[taskId].funcno = funcno;
    ptr->tasks[taskId].state = 0;
    ptr->tasks[taskId].active = sBeachMinigameSetupTasks[funcno](&ptr->tasks[taskId].state, ptr);
}

static void Task_Minigame(u8 taskId)
{
    struct MinigameSetupTaskData * ptr = (void *)GetWordTaskArg(taskId, 0);
    s32 i;

    for (i = 0; i < (int)ARRAY_COUNT(ptr->tasks); i++)
    {
        if (ptr->tasks[i].active)
            ptr->tasks[i].active = sBeachMinigameSetupTasks[ptr->tasks[i].funcno](&ptr->tasks[i].state, ptr);
    }
}

static struct MinigameSetupTaskData * GetMinigameSetupTaskDataPtr(void)
{
    return (void *)GetWordTaskArg(FindTaskIdByFunc(Task_Minigame), 0);
}

static bool32 IsMinigameSetupTaskActive(u8 taskId)
{
    return GetMinigameSetupTaskDataPtr()->tasks[taskId].active;
}