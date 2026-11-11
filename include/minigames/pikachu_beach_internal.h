#ifndef GUARD_PIKACHU_BEACH_INTERNAL_H
#define GUARD_PIKACHU_BEACH_INTERNAL_H

#include "global.h"
#include "gflib.h"
#include "task.h"

#define SURF_BG_OCEAN           2
#define SURF_BG_SKY             3
#define SURF_BG_HUD             0
#define SURF_BG_METER           1

#define SURF_TILE_BLANK         0x00
#define SURF_TILE_TRANSPARENT   0x41
#define SURF_TILE_ISLAND        0x15
#define SURF_TILE_WAVE_L        0x17
#define SURF_TILE_WAVE_M        0x18
#define SURF_TILE_WAVE_R        0x19
#define SURF_TILE_HP_H          0x1B
#define SURF_TILE_HP_P          0x1C

#define SURF_NUM_CLOUDS         9
#define SURF_NUM_DIGITS         4
#define SURF_HUD_Y              152
#define SURF_PROGRESS_X         80
#define SURF_DIGIT_X            180

#define SURF_WAVE_COLS          32
#define SURF_WAVE_ROWS          20
#define SURF_Y_OFFSET           16
#define SURF_PIKA_X             108
#define SURF_PIKA_X_GB          0x48
#define SURF_WATERLINE_DEFAULT  0x74
#define SURF_DISTANCE_GOAL      0x18
#define SURF_DISTANCE_SHORE     0x16
#define SURF_HP_START           6000
#define SURF_SCROLL_PLAY        0x180
#define SURF_SCROLL_GOAL        0x900
#define SURF_SPEED_START        0x0040
#define SURF_SPEED_CAP_HI       2
#define SURF_CRASH_TIME         0x60
#define SURF_LAND_Y_LIMIT       0x90
#define SURF_COLUMN_XOFFSET     DISPLAY_WIDTH

#define WIN_SURF_HUD            0
#define WIN_SURF_RESULTS        1

enum {
    SURF_ROUTINE_SPAWN = 0,
    SURF_ROUTINE_PLAY,
    SURF_ROUTINE_GOAL_INIT,
    SURF_ROUTINE_GOAL_PAN,
    SURF_ROUTINE_RESULTS,
    SURF_ROUTINE_HP_LEFT,
    SURF_ROUTINE_RADNESS,
    SURF_ROUTINE_TOTAL,
    SURF_ROUTINE_ADD_HP,
    SURF_ROUTINE_ADD_RADNESS,
    SURF_ROUTINE_WAIT_LAST,
    SURF_ROUTINE_EXIT_A,
    SURF_ROUTINE_GAME_OVER,
    SURF_ROUTINE_EXIT_FLAG = 0x80,
};

enum {
    SURF_ACTION_ON_WAVE = 0,
    SURF_ACTION_AIRBORNE,
    SURF_ACTION_JUMP_END,
    SURF_ACTION_CRASH,
    SURF_ACTION_END_WAIT,
    SURF_ACTION_END_JUMP_INIT,
    SURF_ACTION_END_JUMP,
};

enum {
    SURF_TASK_GFX_INIT,
    SURF_TASK_FADEOUT_EXIT,
};

enum {
    GFXTAG_PIKACHU_PADDLE = 0,
    GFXTAG_PIKACHU_SURF = 1,
    GFXTAG_SURF_HUD = 2,
};

enum {
    PALTAG_PIKACHU_PADDLE = 0,
    PALTAG_PIKACHU_SURF = 1,
    PALTAG_SURF_HUD = 2,
};

enum {
    SURF_ANIM_CLOUD_0 = 0,
    SURF_ANIM_CLOUD_1,
    SURF_ANIM_CLOUD_2,
    SURF_ANIM_CLOUD_3,
    SURF_ANIM_PIKA_ICON,
    SURF_ANIM_DIGIT_0,
};

enum {
    SURF_ANIM_PADDLE_LOOP = 0,
    SURF_ANIM_PADDLE_0,
    SURF_ANIM_PADDLE_1,
    SURF_ANIM_PADDLE_2,
    SURF_ANIM_PADDLE_3,
};

enum {
    SURF_ANIM_SURF_RIDE = 0,
    SURF_ANIM_SURF_AIR,
    SURF_ANIM_SURF_FLIP,
    SURF_ANIM_SURF_CRASH,
};

struct SurfMinigameSetupTaskDataSub
{
    u16 funcno;
    u8 state;
    bool8 active;
};

typedef struct SurfMinigameSetupTaskData
{
    struct SurfMinigameSetupTaskDataSub subtasks[8];
    u8 bg0TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg1TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg2TilemapBuffer[BG_SCREEN_SIZE];
    u8 bg3TilemapBuffer[BG_SCREEN_SIZE];
} SurfMinigameSetupTaskData;

struct SurfGame
{
    u8 routineNumber;
    u8 pikaAction;
    u8 frameset;
    u8 fieldC;
    u8 fieldD;
    u8 fieldE;
    u8 tiltFrame;
    u8 tiltDirection;
    u8 tiltTimer;
    u8 risingFlag;
    u8 trickFlags;
    u8 radnessMeter;
    u8 waveType;
    u8 waveFunctionNumber;
    u8 timeOverFlag;
    u8 pikaDead;
    u8 splashTimer;
    u8 inputDelay;
    u8 routineDelay;
    u8 gameOverDelay;
    u8 introFinished;
    u8 scxHi;
    u8 scx2;
    u8 xOffset;
    u8 bgTileUnderBoard;
    u8 progressSpriteId;
    u8 paddleSpriteId;
    u8 surfSpriteId;
    u8 displayY;
    u16 speed;
    u16 scx;
    u16 cloudAccum;
    u16 vertSpeed;
    u16 timer;
    u16 radnessScore;
    u16 totalScore;
    u8 distance[3];
    u8 waveHeight[SURF_WAVE_COLS];
    u8 gbTiles[SURF_WAVE_COLS][SURF_WAVE_ROWS];
    u8 waveY;
    u8 pikaY;
    u8 pikaYFrac;
    u16 joyBuffered;
};

typedef struct SurfMinigameState
{
    MainCallback savedCallback;
    u8 taskId;
    struct SurfGame game;
} SurfMinigameState;

struct SurfMinigameGfxManager
{
    struct Sprite *pikachuPaddleSprite;
    struct Sprite *pikachuSurfSprite;
    struct Sprite *progressSprite;
    struct Sprite *cloudSprites[SURF_NUM_CLOUDS];
    struct Sprite *digitSprites[SURF_NUM_DIGITS];
};

extern EWRAM_DATA struct SurfMinigameState *sSurfMinigameState;

static inline struct SurfGame *GetSurfGame(void)
{
    return &sSurfMinigameState->game;
}

void SetMainTask(TaskFunc taskFunc);
void SetSurfMinigameSetupTask(u16 funcno, u8 taskId);
bool32 IsSurfMinigameSetupTaskActive(u8 taskId);
struct SurfMinigameSetupTaskData *GetSurfMinigameSetupTaskDataPtr(void);
void MainTask_SurfMinigameLoop(u8 taskId);
void Task_SurfMinigame(u8 taskId);
void MainTask_ConfirmExitGame(u8 taskId);
void MainTask_ExitMinigame(u8 taskId);
void CleanUpSurfMinigameState(void);

bool8 SurfTask_InitGraphics(u8 *state, SurfMinigameSetupTaskData *ptr);
bool8 SurfTask_FadeOutExit(u8 *state, SurfMinigameSetupTaskData *ptr);

void SurfMinigame_HandleInput(void);
void SurfMinigame_InitGameState(struct SurfGame *game);
void SurfMinigame_RunRoutine(void);
void SurfMinigame_UpdatePikachu(void);
void SurfMinigame_UpdateDistance(void);

void SurfMinigame_InitOcean(void);
void SurfMinigame_ScrollAndGenerateBG(u16 add);
void SurfMinigame_GenerateBGMap(void);
void SurfMinigame_ReadBGMapBuffer(void);
void SurfMinigame_SetPikachuHeight(void);

void SurfMinigame_ApplyScroll(void);
void SurfMinigame_DrawHud(void);
void SurfMinigame_DrawResultsBox(void);
void SurfMinigame_PrintResultsLine(u8 line, const u8 *label, u16 value, bool32 pts);
void SurfMinigame_PrintHiScore(void);
void SurfMinigame_UpdatePikachuSprites(void);
void SurfMinigame_MoveClouds(void);
void SurfMinigame_SpawnSplash(void);
void SurfMinigame_UpdateProgressMarker(s32 dx);
void SurfMinigame_HideGameplayHud(void);
void DestroyGfxManager(void);

void SurfMinigame_PlayBgm(void);
void SurfMinigame_StopBgm(void);
void SurfMinigame_PlayJumpSe(void);
void SurfMinigame_PlayFlipSe(void);
void SurfMinigame_PlayCrashSe(void);
void SurfMinigame_PlayLandSe(void);
void SurfMinigame_PlayTallySe(void);
void SurfMinigame_PlayHighScoreFanfare(void);

s32 CreateSprite_Pikachu_Paddle(s16 x, s16 y);
s32 CreateSprite_Pikachu_Surf(s16 x, s16 y);

#endif // GUARD_PIKACHU_BEACH_INTERNAL_H
