#include "minigames/pikachu_beach_internal.h"

#define t_idle 10

static const union AnimCmd sAnimCmd_Pikachu_paddle[] = {
    ANIMCMD_FRAME(0x00, t_idle),
    ANIMCMD_FRAME(0x10, t_idle),
    ANIMCMD_FRAME(0x20, t_idle),
    ANIMCMD_FRAME(0x30, t_idle),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnimTable_Pikachu_paddle[] = {
    sAnimCmd_Pikachu_paddle
};

static const struct OamData sOamData_Pikachu_paddle = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .x = 0,
    .matrixNum = 0,
    .size = ST_OAM_SIZE_2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct SpriteTemplate sSpriteTemplate_Pikachu_Paddle = {
    .tileTag = GFXTAG_PIKACHU_PADDLE,
    .paletteTag = PALTAG_PIKACHU_PADDLE,
    .oam = &sOamData_Pikachu_paddle,
    .anims = sAnimTable_Pikachu_paddle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static s32 CreateSprite_Pikachu_Paddle(void)
{
    s32 spriteId = CreateSprite(&sSpriteTemplate_Pikachu_Paddle, 16, 136, 1);
    StartSpriteAnim(&gSprites[spriteId], 0);
    return spriteId;
}
