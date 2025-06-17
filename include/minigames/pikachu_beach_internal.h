// pikach_beach.h
#ifndef GUARD_PIKACHU_BEACH_MAIN
#define GUARD_PIKACHU_BEACH_MAIN

#include "global.h"
#include "gflib.h"

// Type definitions
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
    MG0TASK_GFX_INIT,
    MG0TASK_FADEOUT_EXIT,
    MG0TASK_UPDATE_LINE_LIGHTS,
    MG0TASK_CLEFAIRY_BOUNCE,
    MG0TASK_ANIM_WIN,
    MG0TASK_END_ANIM_WIN,
    MG0TASK_ANIM_LOSE,
    MG0TASK_ANIM_BETTING,
    MG0TASK_SHOW_AMOUNTS,
    MG0TASK_MSG_NO_COINS,
    MG0TASK_ASK_QUIT,
    MG0TASK_DESTROY_YESNO,
    MG0TASK_PRESS_BUTTON,
    MG0TASK_RELEASE_BUTTONS,
    MG0TASK_SHOWHELP,
    MG0TASK_HIDEHELP,
};

typedef struct SurfMinigameSetupTaskData
{
    struct SurfMinigameSetupTaskDataSub_0000 subtasks[8];
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

// TASKS
void SetSurfMinigameSetupTask(u16 funcno, u8 taskId); // From tasks.c, but needed here.
bool32 IsSurfMinigameSetupTaskActive(u8 taskId); // From tasks.c, but needed here.
struct SurfMinigameSetupTaskData * GetSurfMinigameSetupTaskDataPtr(void); // From tasks.c, but needed here.
void MainTask_SurfMinigameLoop(u8 taskId); // From tasks.c, but needed here.
void Task_SurfMinigame(u8 taskId); // From tasks.c, but needed here.
bool8  MG0Task_InitGraphics(u8 * state, SurfMinigameSetupTaskData * ptr);

// GFX
void DestroyGfxManager(void); // From gfx.c, but needed here.

#endif // GUARD_PIKACHU_BEACH_H