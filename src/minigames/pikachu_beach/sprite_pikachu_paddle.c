#include "minigames/pikachu_beach_internal.h"

#define T_IDLE 10

static const union AnimCmd sAnim_PaddleLoop[] = {
    ANIMCMD_FRAME(0x00, T_IDLE),
    ANIMCMD_FRAME(0x10, T_IDLE),
    ANIMCMD_FRAME(0x20, T_IDLE),
    ANIMCMD_FRAME(0x30, T_IDLE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Paddle0[] = {
    ANIMCMD_FRAME(0x00, 0xFF),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Paddle1[] = {
    ANIMCMD_FRAME(0x10, 0xFF),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Paddle2[] = {
    ANIMCMD_FRAME(0x20, 0xFF),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Paddle3[] = {
    ANIMCMD_FRAME(0x30, 0xFF),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Paddle[] = {
    sAnim_PaddleLoop,
    sAnim_Paddle0,
    sAnim_Paddle1,
    sAnim_Paddle2,
    sAnim_Paddle3,
};

static const struct OamData sOamData_Paddle = {
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
    .oam = &sOamData_Paddle,
    .anims = sAnimTable_Paddle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

s32 CreateSprite_Pikachu_Paddle(s16 x, s16 y)
{
    s32 spriteId = CreateSprite(&sSpriteTemplate_Pikachu_Paddle, x, y, 1);
    StartSpriteAnim(&gSprites[spriteId], SURF_ANIM_PADDLE_LOOP);
    return spriteId;
}
