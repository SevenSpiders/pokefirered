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

// TASKS
void SetSurfMinigameSetupTask(u16 funcno, u8 taskId); // From tasks.c, but needed here.
bool32 IsSurfMinigameSetupTaskActive(u8 taskId); // From tasks.c, but needed here.
struct SurfMinigameSetupTaskData * GetSurfMinigameSetupTaskDataPtr(void); // From tasks.c, but needed here.
void MainTask_SurfMinigameLoop(u8 taskId); // From tasks.c, but needed here.
void Task_SurfMinigame(u8 taskId); // From tasks.c, but needed here.

// GFX
void DestroyGfxManager(void); // From gfx.c, but needed here.

#endif // GUARD_PIKACHU_BEACH_H