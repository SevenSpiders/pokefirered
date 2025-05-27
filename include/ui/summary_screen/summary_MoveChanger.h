/* summary_MoveChanger.h */

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
u32 MoveChanger_SetPokemon(struct Pokemon *pokemon);
LearnableMoveData *MoveChanger_GetMoveData(u8 index);
u8 MoveChanger_GetNumMoves(void);
// u8 MoveChanger_GetNumEquippedMoves(void);
// u8 MoveChanger_GetNumUnequippedMoves(void);
u32 MoveChanger_SwapMoves(struct Pokemon * pokemon, u8 newMoveIndex, u8 slotToReplace);
// u32 MoveChanger_ForgetMove(u8 slotToForget);
// void MoveChanger_RefreshEquippedStatus(void);

/* Efficient slot access functions */
// u16 MoveChanger_GetSlot(u8 slot);
// u8 MoveChanger_IsSlotEmpty(u8 slot);
// u8 MoveChanger_GetFirstEmptySlot(void);
// u8 MoveChanger_GetNumOccupiedSlots(void);

#endif