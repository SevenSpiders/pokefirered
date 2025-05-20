#include "main.h"
#define MEMSET0(arr) memset((arr), 0, sizeof(arr))


static u16 sMoveIds[25];
static u16 sMoveTypes[25];
static u8 sMoveCategories[25];
static u8 sMoveNameStrBufs[25][MOVE_NAME_LENGTH + 1];
static u16 sSelectedMoveIndex;

static u8 numMoves;
static u32 sCursorIndex;
static u32 sMoveIndex;
static struct Pokemon * mon;

static void ClearData()
{
    MEMSET0(sMoveIds);
    MEMSET0(sMoveTypes);
    MEMSET0(sMoveCategories);
    MEMSET0(sMoveNameStrBufs);
    sSelectedMoveIndex = -1;
    numMoves = 0;
    sCursorIndex = 0;
    sMoveIndex = 0;
    mon = NULL;
}

u16 MoveChanger_GetType(u8 i)
{
    return sMoveTypes[10-i];
}

u16 MoveChanger_GetCategory(u8 i)
{
    return sMoveCategories[10-i];
}

u16 MoveChanger_GetMove(u8 i)
{
    return sMoveIds[10-i];
}

void MoveChanger_SelectMove()
{
    sSelectedMoveIndex = sCursorIndex;
}

void MoveChanger_DeselectMove()
{
    sSelectedMoveIndex = 0;
}

void MoveChanger_SetPokemon(struct Pokemon * pokemon) 
{
    u32 i;
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    u8 level = GetMonData(mon, MON_DATA_LEVEL, NULL);

    ClearData();
    numMoves = GetMoveRelearnerMoves(pokemon, sMoveIds);
    mon = pokemon;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        sMoveIds[i] = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
        if (sMoveIds[i] != 0) 
        {
            sMoveTypes[i] = gBattleMoves[sMoveIds[i]].type;
            sMoveCategories[i] = gBattleMoves[sMoveIds[i]].category;
        }
    }

    for (i = MAX_MON_MOVES; i < MAX_LEVEL_UP_MOVES; i++)
    {
        sMoveIds[i] = gLevelUpLearnsets[species][i] & LEVEL_UP_MOVE_ID;
        if (sMoveIds[i] == 0)
        {
            numMoves = i;
            break;
        }
        sMoveTypes[i] = gBattleMoves[sMoveIds[i]].type;
        sMoveCategories[i] = gBattleMoves[sMoveIds[i]].category;
    }


}

void MoveChanger_SetCursor(u32 cursorIndex)
{
    u16 move;
    sCursorIndex = cursorIndex;
    move = sMoveIds[cursorIndex];
    DebugPrintf("cursor: %d move: %d",cursorIndex, sMoveIds[cursorIndex]);
}


void MoveChanger_SwapMonMoveSlots(void)
{
    u32 i;
    u16 move;
    move = sMoveIds[sCursorIndex];
    SetMonData(mon, MON_DATA_MOVE1 + sCursorIndex, (u8 *)&move);
    // DebugPrintf("pokemon num moves: %d", numMoves);
    for(i=0; i < 25; i++) 
    {
        DebugPrintf("moveId: %d, type: %d, category: %d", sMoveIds[i], sMoveTypes[i], sMoveCategories[i] );
    }
}