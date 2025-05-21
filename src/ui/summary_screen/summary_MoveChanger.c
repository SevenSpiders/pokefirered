#include "main.h"
#include "ui/summary_screen/summary_MoveChanger.h"
#include "string_util.h"
#include "strings.h"
#include "characters.h"

#define MEMSET_ARR(arr, val) memset((arr), val, sizeof(arr))
#define NUM_MOVEDATAS 25

extern const u8 *const gMoveDescriptionPointers[];

static u16 sSelectedMoveIndex;
static MoveData sMoveData[NUM_MOVEDATAS];

static u8 numMoves;
static u32 sCursorIndex;
static u32 sMoveIndex;
static struct Pokemon * mon;

static void ClearData()
{
    // u32 i;
    MEMSET_ARR(sMoveData, 0);
    
    // sSelectedMoveIndex = -1;
    // numMoves = 0;
    // sCursorIndex = 0;
    // sMoveIndex = 0;
    // mon = NULL;
}

MoveData *MoveChanger_GetMoveData(u8 i)
{
    return &sMoveData[i];
}

MoveData *MoveChanger_GetMoveDataAtCursor()
{
    return &sMoveData[sCursorIndex];
}

u16 MoveChanger_GetType(u8 i)
{
    return sMoveData[i].type;
}

u16 MoveChanger_GetCategory(u8 i)
{
    return sMoveData[i].category;
}

u16 MoveChanger_GetMove(u8 i)
{
    return sMoveData[i].id;
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

static void SetData(u32 index, u32 moveId)
{
    u8 power, accuracy;
    sMoveData[index].id = moveId;
    if (moveId == 0)
        return; 
    
    sMoveData[index].type = gBattleMoves[moveId].type;
    sMoveData[index].category = gBattleMoves[moveId].category;

    power = gBattleMoves[moveId].power;
    if (power > 1) // 1 is 0 
        ConvertIntToDecimalStringN( sMoveData[index].powerStr, power, STR_CONV_MODE_RIGHT_ALIGN, 3);
    else
        StringCopy( sMoveData[index].powerStr, gText_ThreeHyphens);

    accuracy = gBattleMoves[moveId].accuracy;
    if (accuracy > 1) // 1 is 0 
        ConvertIntToPercentageString( sMoveData[index].accuracyStr, power, STR_CONV_MODE_RIGHT_ALIGN, 4);
    else
        StringCopy( sMoveData[index].accuracyStr, gText_ThreeHyphens);
}

u32 MoveChanger_SetPokemon(struct Pokemon * pokemon) 
{
    u32 i, species, id;
    // u16 species;
    u8 level = GetMonData(mon, MON_DATA_LEVEL, NULL);
    u32 hp = GetMonData(mon, MON_DATA_HP, NULL);
    // u16 id;
    mon = pokemon;
    species = GetMonData(mon, MON_DATA_SPECIES, NULL); // this causes two bad memory access errors...
    DebugPrintf("set pokemon %d HP %d", level, species);

    ClearData();

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        id = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
        SetData(i, id);
    }

    for (i = MAX_MON_MOVES; i < MAX_LEVEL_UP_MOVES; i++)
    {   
        id = gLevelUpLearnsets[species][i] & LEVEL_UP_MOVE_ID;
        if (id == 0 || id == LEVEL_UP_END)
        {
            numMoves = i;
            break;
        }
        SetData(i, id);
    }

    return numMoves;
}

void MoveChanger_SetCursor(u32 cursorIndex)
{
    u16 move;
    sCursorIndex = cursorIndex;
    move = sMoveData[cursorIndex].id;
    // DebugPrintf("cursor: %d move: %d",cursorIndex, sids[cursorIndex]);
}


void MoveChanger_SwapMonMoveSlots(void)
{
    u32 i;
    u16 move;
    move = sMoveData[sCursorIndex].id;
    SetMonData(mon, MON_DATA_MOVE1 + sCursorIndex, (u8 *)&move);
    // DebugPrintf("pokemon num moves: %d", numMoves);
    for(i=0; i < 25; i++) 
    {
        // DebugPrintf("id: %d, type: %d, category: %d", sids[i], stypes[i], sMoveCategories[i] );
    }
}