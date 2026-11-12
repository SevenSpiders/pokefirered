#include "minigames/pikachu_beach_internal.h"

#define T_IDLE 8

static const union AnimCmd sAnim_SurfRide[] = {
    ANIMCMD_FRAME(0x00, T_IDLE),
    ANIMCMD_FRAME(0x10, T_IDLE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_SurfAir[] = {
    ANIMCMD_FRAME(0x20, 6),
    ANIMCMD_FRAME(0x30, 6),
    ANIMCMD_FRAME(0x40, 6),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_SurfFlip[] = {
    ANIMCMD_FRAME(0x20, 4),
    ANIMCMD_FRAME(0x30, 4),
    ANIMCMD_FRAME(0x40, 4),
    ANIMCMD_FRAME(0x50, 4),
    ANIMCMD_FRAME(0x60, 4),
    ANIMCMD_FRAME(0x70, 4),
    ANIMCMD_FRAME(0x80, 4),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_SurfCrash[] = {
    ANIMCMD_FRAME(0x90, 8),
    ANIMCMD_FRAME(0xA0, 8),
    ANIMCMD_END
};

// Yellow Frameset_LargeSplash, with the large frame held through most of a crash.
static const union AnimCmd sAnim_BigSplash[] = {
    ANIMCMD_FRAME(0x130, 2),
    ANIMCMD_FRAME(0x140, 48),
    ANIMCMD_FRAME(0x150, 8),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Surf[] = {
    sAnim_SurfRide,
    sAnim_SurfAir,
    sAnim_SurfFlip,
    sAnim_SurfCrash,
    sAnim_BigSplash,
};

static const struct OamData sOamData_Surf = {
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

static const struct SpriteTemplate sSpriteTemplate_Pikachu_Surfing = {
    .tileTag = GFXTAG_PIKACHU_SURF,
    .paletteTag = PALTAG_PIKACHU_SURF,
    .oam = &sOamData_Surf,
    .anims = sAnimTable_Surf,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

s32 CreateSprite_Pikachu_Surf(s16 x, s16 y)
{
    s32 spriteId = CreateSprite(&sSpriteTemplate_Pikachu_Surfing, x, y, 1);
    StartSpriteAnim(&gSprites[spriteId], SURF_ANIM_SURF_RIDE);
    return spriteId;
}

s32 CreateSprite_SurfSplash(s16 x, s16 y)
{
    s32 spriteId = CreateSprite(&sSpriteTemplate_Pikachu_Surfing, x, y, 2);
    StartSpriteAnim(&gSprites[spriteId], SURF_ANIM_BIG_SPLASH);
    gSprites[spriteId].invisible = TRUE;
    return spriteId;
}
