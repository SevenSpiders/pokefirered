#include "global.h"
#include "gflib.h"
#include "sound.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "minigames/pikachu_beach_internal.h"

void SurfMinigame_PlayBgm(void)
{
    PlayBGM(MUS_SURF);
}

void SurfMinigame_StopBgm(void)
{
    FadeOutBGM(1);
}

void SurfMinigame_PlayJumpSe(void)
{
    PlaySE(SE_M_SURF);
}

void SurfMinigame_PlayFlipSe(void)
{
    PlaySE(SE_LEDGE);
}

void SurfMinigame_PlayCrashSe(void)
{
    PlaySE(SE_M_DIVE);
}

void SurfMinigame_PlayLandSe(void)
{
    PlaySE(SE_PUDDLE);
}

void SurfMinigame_PlayTallySe(void)
{
    PlaySE(SE_CLICK);
}

void SurfMinigame_PlayHighScoreFanfare(void)
{
    PlayCry_Normal(SPECIES_PIKACHU, 0);
    PlaySE(SE_SUCCESS);
}
