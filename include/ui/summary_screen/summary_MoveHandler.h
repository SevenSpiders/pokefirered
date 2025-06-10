#ifndef GUARD_SUMMARY_MOVE_CHANGER_H
#define GUARD_SUMMARY_MOVE_CHANGER_H

#include "pokemon.h"

/* Updated structure for better move management */
typedef struct {
    u16 moveId;
    u8 learnLevel;
    u8 learnMethod; /* 0=level up, 1=TM/HM, 2=tutor, 3=egg, etc. */
    u8 type;
    u8 category;
    u8 powerStr[4];
    u8 accuracyStr[5];
} LearnableMoveData;

/* Function prototypes */
u32 MoveHandler_SetPokemon(struct Pokemon *pokemon);
LearnableMoveData *MoveHandler_GetMoveData(u8 index);
u8 MoveHandler_GetNumMoves(void);
u32 MoveHandler_SwapMoves(struct Pokemon * pokemon, u8 newMoveIndex, u8 slotToReplace);

#endif