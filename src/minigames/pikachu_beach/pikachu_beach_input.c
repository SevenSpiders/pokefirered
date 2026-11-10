#include "global.h"
#include "gflib.h"
#include "minigames/pikachu_beach_internal.h"

void SurfMinigame_HandleInput(void)
{
    struct SurfGame *game = GetSurfGame();

    if (JOY_NEW(START_BUTTON))
    {
        SetMainTask(MainTask_ConfirmExitGame);
        return;
    }

    if (game->inputDelay == 0)
    {
        game->joyBuffered = gMain.heldKeys;
        game->inputDelay = 2;
    }
    else
    {
        game->joyBuffered = 0;
        game->inputDelay--;
    }
}
