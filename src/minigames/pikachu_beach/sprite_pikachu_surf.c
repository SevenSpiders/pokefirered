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

// Yellow Frameset_LargeSplash: small 2, large 2, dorepeat 8 (hold large), empty board.
static const union AnimCmd sAnim_BigSplash[] = {
    ANIMCMD_FRAME(0x130, 2),
    ANIMCMD_FRAME(0x140, 11),
    ANIMCMD_FRAME(0x120, 2),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Surf[] = {
    sAnim_SurfRide,
    sAnim_SurfAir,
    sAnim_SurfFlip,
    sAnim_SurfCrash,
    sAnim_BigSplash,
};

static const union AnimCmd sAnim_Plus50[] = {
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Plus150[] = {
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Plus350[] = {
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Plus180[] = {
    ANIMCMD_FRAME(12, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Plus500[] = {
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_Score[] = {
    sAnim_Plus50,
    sAnim_Plus150,
    sAnim_Plus350,
    sAnim_Plus180,
    sAnim_Plus500,
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

static const struct OamData sOamData_Score = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_H_RECTANGLE,
    .x = 0,
    .matrixNum = 0,
    .size = ST_OAM_SIZE_1,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct SpriteTemplate sSpriteTemplate_SurfScore = {
    .tileTag = GFXTAG_SURF_SCORE,
    .paletteTag = PALTAG_PIKACHU_SURF,
    .oam = &sOamData_Score,
    .anims = sAnimTable_Score,
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
    s32 spriteId = CreateSprite(&sSpriteTemplate_Pikachu_Surfing, x, y, 3);
    StartSpriteAnim(&gSprites[spriteId], SURF_ANIM_BIG_SPLASH);
    gSprites[spriteId].invisible = TRUE;
    gSprites[spriteId].oam.priority = 2;
    return spriteId;
}

static void SpriteCB_ScorePopup(struct Sprite *sprite)
{
    static const u8 sFlash[] = { 5, 2, 4, 2, 3, 2, 2 };
    u8 phase = sprite->data[1];

    if (sprite->data[0] != 0)
    {
        sprite->data[0]--;
        return;
    }
    phase++;
    if (phase >= sizeof(sFlash))
    {
        sprite->invisible = TRUE;
        sprite->callback = SpriteCallbackDummy;
        return;
    }
    sprite->data[1] = phase;
    sprite->data[0] = sFlash[phase] - 1;
    sprite->invisible = (phase & 1) != 0;
}

s32 CreateSprite_SurfScore(s16 x, s16 y)
{
    s32 spriteId = CreateSprite(&sSpriteTemplate_SurfScore, x, y, 0);
    gSprites[spriteId].invisible = TRUE;
    return spriteId;
}

void SurfMinigame_StartScorePopupAnim(struct Sprite *sprite, u8 anim)
{
    StartSpriteAnim(sprite, anim);
    sprite->invisible = FALSE;
    sprite->data[0] = 4;
    sprite->data[1] = 0;
    sprite->callback = SpriteCB_ScorePopup;
}
