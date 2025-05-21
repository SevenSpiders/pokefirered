#ifndef GUARD_SUMMARY_MOVECHANGER
#define GUARD_SUMMARY_MOVECHANGER

typedef struct {
    u16 id;
    u16 type;
    u8 category;
    u8 powerStr[5];
    u8 accuracyStr[5];
    u8 name[MOVE_NAME_LENGTH + 1];
} MoveData;


void MoveChanger_SetPokemon(struct Pokemon * pokemon);
void MoveChanger_SwapMonMoveSlots(void);
void MoveChanger_SetCursor(u32 cursorIndex);

MoveData MoveChanger_GetMoveData(u8 i);
MoveData *MoveChanger_GetMoveDataAtCursor(void);
u16 MoveChanger_GetCategory(u8 i);
u16 MoveChanger_GetType(u8 i);
u16 MoveChanger_GetMove(u8 i);

void MoveChanger_SelectMove(void);
void MoveChanger_DeselectMove(void);

const u8 * MoveChanger_GetDescription();

#endif