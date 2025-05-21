#ifndef GUARD_SUMMARY_MOVECHANGER
#define GUARD_SUMMARY_MOVECHANGER

void MoveChanger_SetPokemon(struct Pokemon * pokemon);
void MoveChanger_SwapMonMoveSlots(void);
void MoveChanger_SetCursor(u32 cursorIndex);

u16 MoveChanger_GetCategory(u8 i);
u16 MoveChanger_GetType(u8 i);
u16 MoveChanger_GetMove(u8 i);

void MoveChanger_SelectMove(void);
void MoveChanger_DeselectMove(void);

const u8 * MoveChanger_GetDescription();

#endif