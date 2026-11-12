#include "minigames/pikachu_beach_internal.h"

#define T_ANGLE 8

static const union AnimCmd sAnim_Angle1[] = {
    ANIMCMD_FRAME(0x00, T_ANGLE),
    ANIMCMD_FRAME(0x10, T_ANGLE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Angle2[] = {
    ANIMCMD_FRAME(0x20, T_ANGLE),
    ANIMCMD_FRAME(0x30, T_ANGLE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Angle3[] = {
    ANIMCMD_FRAME(0x40, T_ANGLE),
    ANIMCMD_FRAME(0x50, T_ANGLE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Angle4[] = {
    ANIMCMD_FRAME(0x60, T_ANGLE),
    ANIMCMD_FRAME(0x70, T_ANGLE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Angle5[] = {
    ANIMCMD_FRAME(0x80, T_ANGLE),
    ANIMCMD_FRAME(0x90, T_ANGLE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Angle6[] = {
    ANIMCMD_FRAME(0xA0, T_ANGLE),
    ANIMCMD_FRAME(0xB0, T_ANGLE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Angle7[] = {
    ANIMCMD_FRAME(0xC0, T_ANGLE),
    ANIMCMD_FRAME(0xD0, T_ANGLE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnimTable_Paddle[] = {
    sAnim_Angle1,
    sAnim_Angle2,
    sAnim_Angle3,
    sAnim_Angle4,
    sAnim_Angle5,
    sAnim_Angle6,
    sAnim_Angle7,
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
    StartSpriteAnim(&gSprites[spriteId], SURF_ANIM_PADDLE_ANGLE_4);
    return spriteId;
}
