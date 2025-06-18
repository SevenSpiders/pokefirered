// #include "minigames/pikachu_beach_internal.h"

// extern EWRAM_DATA struct SurfMinigameGfxManager * sSurfMinigameGfxManager;


// #define t_idle 10

// static const union AnimCmd sAnimCmd_Pikachu_idle[] = {
//     ANIMCMD_FRAME(0x20, t_idle),
//     ANIMCMD_FRAME(0x30, t_idle),
//     ANIMCMD_FRAME(0x40, t_idle),
//     ANIMCMD_FRAME(0x30, t_idle),
//     ANIMCMD_JUMP(0)
// };

// static const union AnimCmd sAnimCmd_Pikachu_1[] = {
//     ANIMCMD_FRAME(0, 10),
//     ANIMCMD_END
// };


// static const union AnimCmd *const sAnimTable_Pikachu[] = {
//     sAnimCmd_Pikachu_idle,
//     sAnimCmd_Pikachu_1,

// };

// static const struct OamData sOamData_Pikachu = {
//     .y = 0,
//     .affineMode = ST_OAM_AFFINE_OFF,
//     .objMode = ST_OAM_OBJ_NORMAL,
//     .mosaic = FALSE,
//     .bpp = ST_OAM_4BPP,
//     .shape = ST_OAM_SQUARE,
//     .x = 0,
//     .matrixNum = 0,
//     .size = ST_OAM_SIZE_2,
//     .tileNum = 0,
//     .priority = 1,
//     .paletteNum = 0,
//     .affineParam = 0
// };

// static const struct SpriteTemplate sSpriteTemplate_Pikachu_Surfing = {
//     .tileTag = GFXTAG_PIKACHU_SURF,
//     .paletteTag = PALTAG_PIKACHU_SURF,
//     .oam = &sOamData_Pikachu,
//     .anims = sAnimTable_Pikachu,
//     .images = NULL,
//     .affineAnims = gDummySpriteAffineAnimTable,
//     .callback = SpriteCallbackDummy
// };

// void CreateSprite_Pikachu_Surf(void)
// {
//     s32 spriteId = CreateSprite(&sSpriteTemplate_Pikachu_Surfing, 16, 136, 1);
//     StartSpriteAnim(&gSprites[spriteId], 0);
//     DebugPrintf("Pika?");
//     sSurfMinigameGfxManager->pikachuSprite = &gSprites[spriteId];
// }
