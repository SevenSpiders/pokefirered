// pikachu_beach_input.c
#include "global.h"
#include "gflib.h"
#include "minigames/pikachu_beach_internal.h" // Include the new header

// This file would contain functions related to input handling for the minigame.
// Based on the provided pikachu_beach.c, specific functions for input are integrated
// directly into MainTask_SurfMinigameLoop. If those were to be extracted, they would go here.
// For example:

#define pikachu_speed 1


void SurfMinigame_HandleInput(void)
{
    if (JOY_NEW(START_BUTTON))
    {
        SetMainTask(MainTask_ConfirmExitGame);
    }
    else if (JOY_NEW(DPAD_UP) || JOY_HELD(DPAD_UP))
    {
        MovePikachu(0, -pikachu_speed);
    }
    else if (JOY_NEW(DPAD_DOWN) || JOY_HELD(DPAD_DOWN))
    {
        MovePikachu(0, pikachu_speed);
    }
    else if (JOY_NEW(DPAD_RIGHT) || JOY_HELD(DPAD_RIGHT))
    {
        MovePikachu(pikachu_speed, 0);
    }
    else if (JOY_NEW(DPAD_LEFT) || JOY_HELD(DPAD_LEFT))
    {
        MovePikachu(-pikachu_speed,0);
    }
}