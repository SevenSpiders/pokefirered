#include "global.h"
#include "gflib.h"
#include "random.h"
#include "trig.h"
#include "event_data.h"
#include "constants/vars.h"
#include "minigames/pikachu_beach_internal.h"

static void RunPikaOnWave(struct SurfGame *game);
static void RunPikaAirborne(struct SurfGame *game);
static void RunPikaJumpEnd(struct SurfGame *game);
static void RunPikaCrash(struct SurfGame *game);
static void RunPikaEndWait(struct SurfGame *game);
static void RunPikaEndJumpInit(struct SurfGame *game);
static void RunPikaEndJump(struct SurfGame *game);
static void HandleDPadFlips(struct SurfGame *game);
static u8 GetLandAction(struct SurfGame *game);
static void ApplyLandAction(struct SurfGame *game, u8 action);
static void AddRadnessFromStunt(struct SurfGame *game);
static bool32 UpdatePikachuHeight(struct SurfGame *game);
static bool32 CheckAutoWaveJump(struct SurfGame *game);
static void SpeedUpPikachu(struct SurfGame *game);
static void ReduceSpeed(struct SurfGame *game, u16 amount);
static void UpdatePikachuTilt(struct SurfGame *game);
static void UpdateOnWaveSprite(struct SurfGame *game);
static bool32 RunDelayTimer(struct SurfGame *game);
static void AdvanceRoutine(struct SurfGame *game, u8 delay);
static u16 AddScores(u16 a, u16 b);

void SurfMinigame_InitGameState(struct SurfGame *game)
{
    s32 i;

    CpuFill16(0, game, sizeof(*game));
    game->speed = SURF_SPEED_START;
    game->timer = SURF_HP_START;
    game->waveY = SURF_WATERLINE_DEFAULT;
    game->pikaY = SURF_WATERLINE_DEFAULT;
    game->frameset = 4;
    game->paddleSpriteId = SPRITE_NONE;
    game->surfSpriteId = SPRITE_NONE;
    game->progressSpriteId = SPRITE_NONE;
    for (i = 0; i < SURF_WAVE_COLS; i++)
        game->waveHeight[i] = SURF_WATERLINE_DEFAULT;
}

void SurfMinigame_UpdateDistance(void)
{
    struct SurfGame *game = GetSurfGame();
    u32 acc = (game->distance[1] << 8) | game->distance[2];

    acc += game->speed;
    game->distance[1] = acc >> 8;
    game->distance[2] = acc;
    if (acc > 0xFFFF)
    {
        game->distance[0]++;
        SurfMinigame_UpdateProgressMarker(-2);
    }
}

static void RoutineSpawn(struct SurfGame *game)
{
    game->pikaAction = SURF_ACTION_ON_WAVE;
    game->pikaY = game->waveY;
    game->frameset = 0x11;
    SurfMinigame_PlayBgm();
    game->routineNumber = SURF_ROUTINE_PLAY;
}

static void RoutinePlay(struct SurfGame *game)
{
    if (game->distance[0] >= SURF_DISTANCE_GOAL)
    {
        AdvanceRoutine(game, 192);
        return;
    }
    if (game->timer == 0)
    {
        game->timeOverFlag = TRUE;
        game->routineNumber = SURF_ROUTINE_GAME_OVER;
        game->gameOverDelay = 0x80;
        game->pikaAction = SURF_ACTION_END_WAIT;
        SurfMinigame_StopBgm();
        return;
    }

    game->waveType = Random();
    SurfMinigame_SetPikachuHeight();
    SurfMinigame_ReadBGMapBuffer();
    game->xOffset = 0xA0;
    SurfMinigame_ScrollAndGenerateBG(SURF_SCROLL_PLAY);
    SurfMinigame_UpdateDistance();
    if (game->timer > 0)
        game->timer--;
}

static void RoutineGoalInit(struct SurfGame *game)
{
    if (!RunDelayTimer(game))
    {
        game->waveType = 0;
        SurfMinigame_SetPikachuHeight();
        SurfMinigame_ReadBGMapBuffer();
        game->xOffset = 0xA0;
        SurfMinigame_ScrollAndGenerateBG(SURF_SCROLL_GOAL);
        return;
    }

    game->routineNumber++;
    game->scx = 0x9000;
    game->waveFunctionNumber = 0x72;
    game->pikaAction = SURF_ACTION_END_WAIT;
    game->scx2 = 0;
    game->scxHi = 0;
}

static void RoutineGoalPan(struct SurfGame *game)
{
    u8 scxPixel = game->scx >> 8;

    if (scxPixel == 0)
    {
        game->speed = 0;
        game->routineNumber++;
        game->pikaAction = SURF_ACTION_END_JUMP_INIT;
        return;
    }

    SurfMinigame_SetPikachuHeight();
    SurfMinigame_ReadBGMapBuffer();
    if (scxPixel >= 4)
        game->scx -= (4 << 8);
    else
        game->scx = 0;
    game->xOffset = 0xE0;
    SurfMinigame_GenerateBGMap();
}

static void RoutineResults(struct SurfGame *game)
{
    SurfMinigame_HideGameplayHud();
    SurfMinigame_DrawResultsBox();
    AdvanceRoutine(game, 32);
}

static void RoutineHpLeft(struct SurfGame *game)
{
    static const u8 sLabel[] = _("HP LEFT");

    if (!RunDelayTimer(game))
        return;
    SurfMinigame_PrintResultsLine(0, sLabel, game->timer, TRUE);
    AdvanceRoutine(game, 64);
}

static void RoutineRadness(struct SurfGame *game)
{
    static const u8 sLabel[] = _("RADNESS");

    if (!RunDelayTimer(game))
        return;
    SurfMinigame_PrintResultsLine(1, sLabel, game->radnessScore, TRUE);
    AdvanceRoutine(game, 64);
}

static void RoutineTotal(struct SurfGame *game)
{
    static const u8 sLabel[] = _("TOTAL");

    if (!RunDelayTimer(game))
        return;
    SurfMinigame_PrintResultsLine(2, sLabel, game->totalScore, TRUE);
    AdvanceRoutine(game, 64);
}

static void RoutineAddHp(struct SurfGame *game)
{
    static const u8 sLabel[] = _("TOTAL");
    s32 i;

    if (!RunDelayTimer(game))
        return;

    for (i = 0; i < 99; i++)
    {
        if (game->timer == 0)
            break;
        game->timer--;
        game->totalScore = AddScores(game->totalScore, 1);
    }
    SurfMinigame_PrintResultsLine(2, sLabel, game->totalScore, TRUE);
    if (game->timer == 0)
        AdvanceRoutine(game, 64);
    else
        SurfMinigame_PlayTallySe();
}

static void RoutineAddRadness(struct SurfGame *game)
{
    static const u8 sLabel[] = _("TOTAL");
    s32 i;

    if (!RunDelayTimer(game))
        return;

    for (i = 0; i < 99; i++)
    {
        if (game->radnessScore == 0)
            break;
        game->radnessScore--;
        game->totalScore = AddScores(game->totalScore, 1);
    }
    SurfMinigame_PrintResultsLine(2, sLabel, game->totalScore, TRUE);
    if (game->radnessScore != 0)
    {
        SurfMinigame_PlayTallySe();
        return;
    }

    AdvanceRoutine(game, 128);
    if (game->totalScore >= VarGet(VAR_PIKACHU_BEACH_HI_SCORE))
    {
        VarSet(VAR_PIKACHU_BEACH_HI_SCORE, game->totalScore);
        SurfMinigame_PlayHighScoreFanfare();
        SurfMinigame_PrintHiScore();
        game->pikaAction = SURF_ACTION_END_JUMP;
    }
}

static void RoutineWaitLast(struct SurfGame *game)
{
    if (RunDelayTimer(game))
        game->routineNumber++;
}

static void RoutineExitA(struct SurfGame *game)
{
    if (JOY_NEW(A_BUTTON))
        game->routineNumber |= SURF_ROUTINE_EXIT_FLAG;
}

static void RoutineGameOver(struct SurfGame *game)
{
    SurfMinigame_SetPikachuHeight();
    SurfMinigame_ReadBGMapBuffer();
    game->xOffset = 0xA0;
    SurfMinigame_ScrollAndGenerateBG(SURF_SCROLL_PLAY);
    if (game->gameOverDelay != 0)
    {
        game->gameOverDelay--;
        return;
    }
    if (JOY_NEW(A_BUTTON))
        game->routineNumber |= SURF_ROUTINE_EXIT_FLAG;
}

void SurfMinigame_RunRoutine(void)
{
    struct SurfGame *game = GetSurfGame();

    switch (game->routineNumber)
    {
    case SURF_ROUTINE_SPAWN:
        RoutineSpawn(game);
        break;
    case SURF_ROUTINE_PLAY:
        RoutinePlay(game);
        break;
    case SURF_ROUTINE_GOAL_INIT:
        RoutineGoalInit(game);
        break;
    case SURF_ROUTINE_GOAL_PAN:
        RoutineGoalPan(game);
        break;
    case SURF_ROUTINE_RESULTS:
        RoutineResults(game);
        break;
    case SURF_ROUTINE_HP_LEFT:
        RoutineHpLeft(game);
        break;
    case SURF_ROUTINE_RADNESS:
        RoutineRadness(game);
        break;
    case SURF_ROUTINE_TOTAL:
        RoutineTotal(game);
        break;
    case SURF_ROUTINE_ADD_HP:
        RoutineAddHp(game);
        break;
    case SURF_ROUTINE_ADD_RADNESS:
        RoutineAddRadness(game);
        break;
    case SURF_ROUTINE_WAIT_LAST:
        RoutineWaitLast(game);
        break;
    case SURF_ROUTINE_EXIT_A:
        RoutineExitA(game);
        break;
    case SURF_ROUTINE_GAME_OVER:
        RoutineGameOver(game);
        break;
    }
}

void SurfMinigame_UpdatePikachu(void)
{
    struct SurfGame *game = GetSurfGame();

    switch (game->pikaAction)
    {
    case SURF_ACTION_ON_WAVE:
        RunPikaOnWave(game);
        break;
    case SURF_ACTION_AIRBORNE:
        RunPikaAirborne(game);
        break;
    case SURF_ACTION_JUMP_END:
        RunPikaJumpEnd(game);
        break;
    case SURF_ACTION_CRASH:
        RunPikaCrash(game);
        break;
    case SURF_ACTION_END_WAIT:
        RunPikaEndWait(game);
        break;
    case SURF_ACTION_END_JUMP_INIT:
        RunPikaEndJumpInit(game);
        break;
    case SURF_ACTION_END_JUMP:
        RunPikaEndJump(game);
        break;
    }
}

static void RunPikaOnWave(struct SurfGame *game)
{
    if (game->timeOverFlag)
    {
        game->speed = 0;
        game->pikaAction = SURF_ACTION_END_WAIT;
        UpdateOnWaveSprite(game);
        return;
    }

    SurfMinigame_SpawnSplash();
    game->pikaY = game->waveY;
    if (CheckAutoWaveJump(game))
    {
        UpdateOnWaveSprite(game);
        game->pikaAction = SURF_ACTION_AIRBORNE;
        game->fieldC = 0;
        game->fieldD = 0;
        game->fieldE = 0;
        game->radnessMeter = 0;
        game->trickFlags = 0;
        game->risingFlag = 0;
        game->vertSpeed &= 0xFF00;
        SurfMinigame_PlayJumpSe();
        return;
    }

    UpdateOnWaveSprite(game);
    SpeedUpPikachu(game);
}

static void RunPikaAirborne(struct SurfGame *game)
{
    u8 action;

    HandleDPadFlips(game);
    if (!UpdatePikachuHeight(game))
        return;

    action = GetLandAction(game);
    if (action == 0)
    {
        game->pikaAction = SURF_ACTION_CRASH;
        game->pikaDead = SURF_CRASH_TIME;
        game->speed = SURF_SPEED_START;
        game->frameset = 0x10;
        SurfMinigame_PlayCrashSe();
        return;
    }

    ApplyLandAction(game, action);
    AddRadnessFromStunt(game);
    game->fieldC = 0;
    game->pikaAction = SURF_ACTION_JUMP_END;
}

static void RunPikaJumpEnd(struct SurfGame *game)
{
    if (game->fieldC >= 0x20)
    {
        game->pikaYFrac = 0;
        game->pikaAction = SURF_ACTION_ON_WAVE;
        return;
    }

    game->fieldC += 4;
    game->pikaYFrac = Sin(game->fieldC, 4);
    SurfMinigame_SpawnSplash();
    game->pikaY = game->waveY;
}

static void RunPikaCrash(struct SurfGame *game)
{
    game->pikaY = game->waveY;
    if (game->pikaDead != 0)
    {
        game->pikaDead--;
        return;
    }
    game->pikaAction = SURF_ACTION_ON_WAVE;
    game->frameset = 4;
}

static void RunPikaEndWait(struct SurfGame *game)
{
    game->pikaY = game->waveY;
    UpdateOnWaveSprite(game);
}

static void RunPikaEndJumpInit(struct SurfGame *game)
{
    game->frameset = 0x0F;
    game->fieldC = 0;
    game->pikaAction = SURF_ACTION_END_JUMP;
}

static void RunPikaEndJump(struct SurfGame *game)
{
    u8 t = game->fieldC;

    game->fieldC += 2;
    if ((t & 0x3F) >= 0x20)
        game->pikaYFrac = 0;
    else
        game->pikaYFrac = Sin(t, 16);
}

static void HandleDPadFlips(struct SurfGame *game)
{
    if (game->joyBuffered & DPAD_LEFT)
    {
        game->fieldE = 0;
        if (game->fieldD >= 0x0B)
        {
            if (game->radnessMeter < 3)
                game->radnessMeter++;
            game->fieldD = 0;
            game->fieldE = 0;
            game->trickFlags |= 1;
            SurfMinigame_PlayFlipSe();
        }
        else
        {
            game->fieldD++;
        }
        if (game->frameset >= 0x0E)
            game->frameset = 1;
        else
            game->frameset++;
    }
    else if (game->joyBuffered & DPAD_RIGHT)
    {
        game->fieldD = 0;
        if (game->fieldE >= 0x0D)
        {
            if (game->radnessMeter < 3)
                game->radnessMeter++;
            game->fieldD = 0;
            game->fieldE = 0;
            game->trickFlags |= 2;
            SurfMinigame_PlayFlipSe();
        }
        else
        {
            game->fieldE++;
        }
        if (game->frameset == 1)
            game->frameset = 0x0E;
        else
            game->frameset--;
    }
}

static const u8 sLandActionDefault[] = {0, 0, 1, 2, 3, 2, 1, 0};
static const u8 sLandActionTile06[] = {0, 0, 0, 0, 1, 2, 3, 2};
static const u8 sLandActionTile07[] = {0, 2, 3, 2, 1, 0, 0, 0};
static const u8 sLandActionCrest[] = {0, 0, 1, 2, 3, 3, 2, 1};

static u8 GetLandAction(struct SurfGame *game)
{
    u8 tile = game->bgTileUnderBoard;
    u8 frame = game->frameset;

    if (frame < 1 || frame > 7)
        return 0;

    if (tile == 0x06)
        return sLandActionTile06[frame];
    if (tile == 0x07)
        return sLandActionTile07[frame];
    if (tile == 0x12 || tile == 0x14)
        return sLandActionCrest[frame];
    return sLandActionDefault[frame];
}

static void ApplyLandAction(struct SurfGame *game, u8 action)
{
    if (action == 1)
        ReduceSpeed(game, 0x80);
    else if (action == 2)
        ReduceSpeed(game, 0x40);
    SurfMinigame_PlayLandSe();
}

static void AddRadnessFromStunt(struct SurfGame *game)
{
    u16 add = 0;
    u8 mixed = (game->trickFlags & 3) == 3;
    u8 n = game->radnessMeter;
    u8 i;
    u8 amount;

    if (n == 0)
        return;

    if (mixed)
    {
        if (n >= 3)
            add = 500;
        else if (n == 2)
            add = 180;
    }
    else
    {
        amount = 0;
        for (i = 0; i < n; i++)
        {
            amount = (amount << 1) + 1;
        }
        add = amount * 50;
    }

    game->radnessScore = AddScores(game->radnessScore, add);
}

static bool32 UpdatePikachuHeight(struct SurfGame *game)
{
    s32 vel;
    s32 disp;
    s32 y16;
    u8 vHigh;

    if (game->risingFlag == 0)
    {
        if (game->vertSpeed == 0)
        {
            game->risingFlag = 1;
            return FALSE;
        }
        vel = game->vertSpeed - 0x80;
        if (vel < 0)
            vel = 0;
        game->vertSpeed = vel;
        vHigh = game->vertSpeed >> 8;
        disp = -(4 * vHigh * vHigh);
        y16 = (game->pikaY << 8) | game->pikaYFrac;
        y16 += disp;
        game->pikaY = y16 >> 8;
        game->pikaYFrac = y16;
        return FALSE;
    }

    if (game->pikaY < SURF_LAND_Y_LIMIT && game->pikaY >= game->waveY)
    {
        game->pikaY = game->waveY;
        game->pikaYFrac = 0;
        return TRUE;
    }

    vel = game->vertSpeed + 0x80;
    game->vertSpeed = vel;
    vHigh = game->vertSpeed >> 8;
    disp = 4 * vHigh * vHigh;
    y16 = (game->pikaY << 8) | game->pikaYFrac;
    y16 += disp;
    game->pikaY = y16 >> 8;
    game->pikaYFrac = y16;
    return FALSE;
}

static bool32 CheckAutoWaveJump(struct SurfGame *game)
{
    u8 scxLow = (game->scx >> 8) & 7;
    u8 speedH;

    if (scxLow < 3 || scxLow >= 5)
        return FALSE;
    if (game->bgTileUnderBoard != 0x14)
        return FALSE;

    speedH = game->speed >> 5;
    if (speedH < 0x0A)
        return FALSE;

    game->vertSpeed = speedH << 8;
    game->risingFlag = 0;
    return TRUE;
}

static void SpeedUpPikachu(struct SurfGame *game)
{
    if ((game->speed >> 8) >= SURF_SPEED_CAP_HI)
        return;
    game->speed += 2;
}

static void ReduceSpeed(struct SurfGame *game, u16 amount)
{
    if (game->speed < amount)
        game->speed = 0;
    else
        game->speed -= amount;
}

static void UpdatePikachuTilt(struct SurfGame *game)
{
    game->tiltTimer++;
    if ((game->tiltTimer & 7) != 0)
        return;

    if (game->tiltDirection == 0)
    {
        if (game->tiltFrame == 2)
            game->tiltDirection = 1;
        else
            game->tiltFrame++;
    }
    else
    {
        if (game->tiltFrame == 0)
            game->tiltDirection = 0;
        else
            game->tiltFrame--;
    }
}

static void UpdateOnWaveSprite(struct SurfGame *game)
{
    u8 scxLow = (game->scx >> 8) & 7;
    u8 tile = game->bgTileUnderBoard;

    if (scxLow < 3 || scxLow >= 5)
        return;

    if (tile == 0x06 || tile == 0x14)
        game->frameset = 6 + game->tiltFrame - 1;
    else if (tile == 0x07)
        game->frameset = 2 + game->tiltFrame - 1;
    else
    {
        UpdatePikachuTilt(game);
        game->frameset = 4;
    }
}

static bool32 RunDelayTimer(struct SurfGame *game)
{
    if (game->routineDelay == 0)
        return TRUE;
    game->routineDelay--;
    return FALSE;
}

static void AdvanceRoutine(struct SurfGame *game, u8 delay)
{
    game->routineNumber++;
    game->routineDelay = delay;
}

static u16 AddScores(u16 a, u16 b)
{
    u32 sum = a + b;
    if (sum > 9999)
        return 9999;
    return sum;
}
