#include "main.h"


u16 sMoveList[25];
u8 numMoves;
static u32 sCursorIndex;
static u32 sMoveIndex;
static struct Pokemon * mon;

void MoveChanger_SetPokemon(struct Pokemon * pokemon) 
{
    u32 i;
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    u8 level = GetMonData(mon, MON_DATA_LEVEL, NULL);
    numMoves = GetMoveRelearnerMoves(pokemon, sMoveList);
    mon = pokemon;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        sMoveList[i] = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
    }

    // for (i = MAX_MON_MOVES; i < MAX_LEVEL_UP_MOVES; i++)
    // {
    //     sMoveList[i] = gLevelUpLearnsets[species][i] & LEVEL_UP_MOVE_ID;
    // }
}

void MoveChanger_SetCursor(u32 cursorIndex)
{
    u16 move;
    sCursorIndex = cursorIndex;
    move = sMoveList[cursorIndex];
    DebugPrintf("cursor: %d move: %d",cursorIndex, sMoveList[cursorIndex]);
}


void MoveChanger_SwapMonMoveSlots(void)
{
    u16 move;
    move = sMoveList[sCursorIndex];
    SetMonData(mon, MON_DATA_MOVE1 + sCursorIndex, (u8 *)&move);
    // DebugPrintf("pokemon num moves: %d", numMoves);
}