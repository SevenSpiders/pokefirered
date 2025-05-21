#include "main.h"
#define MEMSET_ARR(arr, val) memset((arr), val, sizeof(arr))
#define NUM_MOVEDATAS 25

extern const u8 *const gMoveDescriptionPointers[];

typedef struct {
    u16 moveId;
    u16 moveType;
    u8 moveCategory;
    u8 movePower;
    u8 moveName[MOVE_NAME_LENGTH + 1];
} MoveData;

static u16 sSelectedMoveIndex;
static MoveData sMoveData[NUM_MOVEDATAS];

static u8 numMoves;
static u32 sCursorIndex;
static u32 sMoveIndex;
static struct Pokemon * mon;

static void ClearData()
{
    u32 i;
    MEMSET_ARR(sMoveData, 0);
    
    sSelectedMoveIndex = -1;
    numMoves = 0;
    sCursorIndex = 0;
    sMoveIndex = 0;
    mon = NULL;
}

u16 MoveChanger_GetType(u8 i)
{
    return sMoveData[i].moveType;
}

u16 MoveChanger_GetCategory(u8 i)
{
    return sMoveData[i].moveCategory;
}

u16 MoveChanger_GetMove(u8 i)
{
    return sMoveData[i].moveId;
}

void MoveChanger_SelectMove()
{
    sSelectedMoveIndex = sCursorIndex;
}

void MoveChanger_DeselectMove()
{
    sSelectedMoveIndex = 0;
}

const u8 *MoveChanger_GetDescription() {
    return gMoveDescriptionPointers[sCursorIndex];
}

void MoveChanger_SetPokemon(struct Pokemon * pokemon) 
{
    u32 i;
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    u8 level = GetMonData(mon, MON_DATA_LEVEL, NULL);
    u16 moveId;

    ClearData();
    mon = pokemon;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        moveId = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
        sMoveData[i].moveId = moveId;
        if (moveId != 0) 
        {
            sMoveData[i].moveType = gBattleMoves[moveId].type;
            sMoveData[i].moveCategory = gBattleMoves[moveId].category;
        }
    }

    for (i = MAX_MON_MOVES; i < MAX_LEVEL_UP_MOVES; i++)
    {   
        moveId = gLevelUpLearnsets[species][i] & LEVEL_UP_MOVE_ID;
        sMoveData[i].moveId = moveId; 
        if (moveId == 0)
        {
            numMoves = i;
            break;
        }
        sMoveData[i].moveType = gBattleMoves[moveId].type;
        sMoveData[i].moveCategory = gBattleMoves[moveId].category;
    }


}

void MoveChanger_SetCursor(u32 cursorIndex)
{
    u16 move;
    sCursorIndex = cursorIndex;
    move = sMoveData[cursorIndex].moveId;
    // DebugPrintf("cursor: %d move: %d",cursorIndex, sMoveIds[cursorIndex]);
}


void MoveChanger_SwapMonMoveSlots(void)
{
    u32 i;
    u16 move;
    move = sMoveData[sCursorIndex].moveId;
    SetMonData(mon, MON_DATA_MOVE1 + sCursorIndex, (u8 *)&move);
    // DebugPrintf("pokemon num moves: %d", numMoves);
    for(i=0; i < 25; i++) 
    {
        // DebugPrintf("moveId: %d, type: %d, category: %d", sMoveIds[i], sMoveTypes[i], sMoveCategories[i] );
    }
}