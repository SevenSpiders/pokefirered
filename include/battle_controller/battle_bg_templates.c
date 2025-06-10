#ifndef GUARD_BG_TEMPLATES
#define GUARD_BG_TEMPLATES

#include "battle.h" // B_WIN_X
#include "bg.h" // BgTemplate
#include "window.h" // WindowTempate

const struct BgTemplate gBattleBgTemplates[4] = {
    [0] = {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 24,
        .screenSize = BG_SIZE_WIDE,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    }, 
    [1] = {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 28,
        .screenSize = BG_SIZE_WIDE,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    }, 
    [2] = {
        .bg = 2,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .screenSize = BG_SIZE_SMALL,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0x000
    }, 
    [3] = {
        .bg = 3,
        .charBaseIndex = 2,
        .mapBaseIndex = 26,
        .screenSize = BG_SIZE_TALL,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0x000
    }
};

static const struct WindowTemplate sStandardBattleWindowTemplates[] = {
    [B_WIN_MSG] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 15,
        .width = 28,
        .height = 4,
        .paletteNum = 0,
        .baseBlock = 144
    },
    [B_WIN_ACTION_PROMPT] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 35,
        .width = 14,
        .height = 4,
        .paletteNum = 0,
        .baseBlock = 448
    },
    [B_WIN_ACTION_MENU] = {
        .bg = 0,
        .tilemapLeft = 17,
        .tilemapTop = 35,
        .width = 12,
        .height = 4,
        .paletteNum = 5,
        .baseBlock = 400
    },
    [B_WIN_MOVE_NAME_1] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 55,
        .width = 8,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 768
    },
    [B_WIN_MOVE_NAME_2] = {
        .bg = 0,
        .tilemapLeft = 11,
        .tilemapTop = 55,
        .width = 8,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 784
    },
    [B_WIN_MOVE_NAME_3] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 57,
        .width = 8,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 800
    },
    [B_WIN_MOVE_NAME_4] = {
        .bg = 0,
        .tilemapLeft = 11,
        .tilemapTop = 57,
        .width = 8,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 816
    },
    [B_WIN_MOVE_PWR] = {
        .bg = 0,
        .tilemapLeft = 23,
        .tilemapTop = 55,
        .width = 2, // 2
        .height = 2, // 2
        .paletteNum = 5,
        .baseBlock = 256 // 0x290 -> 0x090 
    },
    [B_WIN_MOVE_POWER] = {
        .bg = 0,
        .tilemapLeft = 26,
        .tilemapTop = 55,
        .width = 3,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 260
    },
    [B_WIN_ICONS_OPPONENT1] = { // enemy
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 5,
        .width = 13, // 12
        .height = 2, // 2
        .paletteNum = 10,
        .baseBlock = 266 // 266
    },
    [B_WIN_ICONS_PLAYER1] = { // player
        .bg = 0,
        .tilemapLeft = 17,
        .tilemapTop = 12,
        .width = 13, // 12
        .height = 2, // 2
        .paletteNum = 10,
        .baseBlock = 292 // 266
    },
    [B_WIN_HEALTHBOX_OPPONENT1] = { // player
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 2,
        .width = 11, // 12
        .height = 3, // 2
        .paletteNum = 10,
        .baseBlock = 512 // 266
    },
    [B_WIN_HEALTHBOX_PLAYER1] = { // player
        .bg = 0,
        .tilemapLeft = 17,
        .tilemapTop = 12,
        .width = 11, // 12
        .height = 3, // 2
        .paletteNum = 10,
        .baseBlock = 512 + 33 // 266
    },
    [B_WIN_ICONS_ZONE] = { // Zone
        .bg = 0,
        .tilemapLeft = 17,
        .tilemapTop = 1,
        .width = 13, // 12
        .height = 2, // 2
        .paletteNum = 10,
        .baseBlock = 318 // 266
    },
    [B_WIN_MOVE_CATEGORY] = {
        .bg = 0,
        .tilemapLeft = 21,
        .tilemapTop = 57,
        .width = 8,
        .height = 2,
        .paletteNum = 10,
        .baseBlock = 688 // 0x0b0 0x090 -> 0x290
    },
    [B_WIN_SWITCH_PROMPT] = {
        .bg = 0,
        .tilemapLeft = 21,
        .tilemapTop = 55,
        .width = 8,
        .height = 4,
        .paletteNum = 5,
        .baseBlock = 688 // 0x2b0
    },
    [B_WIN_LEVEL_UP_BOX] = { // change baseblock
        .bg = 1,
        .tilemapLeft = 19,
        .tilemapTop = 8,
        .width = 10,
        .height = 11,
        .paletteNum = 5,
        .baseBlock = 504 // 256
    },
    [B_WIN_LEVEL_UP_BANNER] = {
        .bg = 2,
        .tilemapLeft = 18,
        .tilemapTop = 0,
        .width = 12,
        .height = 3,
        .paletteNum = 6,
        .baseBlock = 366
    },
    [B_WIN_YESNO] = { // change baseblock
        .bg = 0,
        .tilemapLeft = 25,
        .tilemapTop = 9,
        .width = 4,
        .height = 4,
        .paletteNum = 5,
        .baseBlock = 384 //256
    },
    [B_WIN_VS_PLAYER] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 3,
        .width = 7,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 32
    },
    [B_WIN_VS_OPPONENT] = {
        .bg = 2,
        .tilemapLeft = 2,
        .tilemapTop = 3,
        .width = 7,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 64
    },
    [B_WIN_VS_MULTI_PLAYER_1] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 2,
        .width = 7,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 32
    },
    [B_WIN_VS_MULTI_PLAYER_2] = {
        .bg = 2,
        .tilemapLeft = 2,
        .tilemapTop = 2,
        .width = 7,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 64
    },
    [B_WIN_VS_MULTI_PLAYER_3] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 6,
        .width = 7,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 96
    },
    [B_WIN_VS_MULTI_PLAYER_4] = {
        .bg = 2,
        .tilemapLeft = 2,
        .tilemapTop = 6,
        .width = 7,
        .height = 2,
        .paletteNum = 5,
        .baseBlock = 128
    },
    [B_WIN_VS_OUTCOME_DRAW] = {
        .bg = 0,
        .tilemapLeft = 11,
        .tilemapTop = 2,
        .width = 8,
        .height = 2,
        .paletteNum = 0,
        .baseBlock = 160
    },
    [B_WIN_VS_OUTCOME_LEFT] = {
        .bg = 0,
        .tilemapLeft = 4,
        .tilemapTop = 2,
        .width = 8,
        .height = 2,
        .paletteNum = 0,
        .baseBlock = 160
    },
    [B_WIN_VS_OUTCOME_RIGHT] = {
        .bg = 0,
        .tilemapLeft = 19,
        .tilemapTop = 2,
        .width = 8,
        .height = 2,
        .paletteNum = 0,
        .baseBlock = 176
    },
    [B_WIN_OAK_OLD_MAN] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 7,
        .baseBlock = 144
    },
    [B_WIN_DUMMY] = DUMMY_WIN_TEMPLATE
};

#endif