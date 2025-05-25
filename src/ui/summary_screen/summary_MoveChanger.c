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
    u32 i;
    MEMSET_ARR(sMoveData, 0);
    
    sSelectedMoveIndex = -1;
    numMoves = 0;
    sCursorIndex = 0;
    sMoveIndex = 0;
    mon = NULL;
}

static void SortMoves()
{
    u32 i, j;
    MoveData temp; // Temporary variable for swapping structs
    u32 n = NUM_MOVEDATAS; // Get the number of elements

    // Outer loop for passes
    for (i = 0; i < n - 1; i++)
    {
        // Inner loop for comparisons and swaps
        for (j = 0; j < n - i - 1; j++)
        {
            /*
             * Comparison Logic:
             * 1. Compare by moveSlot first.
             * 2. If moveSlot values are equal, then compare by learnIndex.
             *
             * If the current element (sMoveData[j]) should come AFTER
             * the next element (sMoveData[j+1]) based on the sorting criteria,
             * then swap them.
             */
            if (sMoveData[j].moveSlot > sMoveData[j+1].moveSlot)
            {
                // Primary key (moveSlot) is out of order, so swap
                temp = sMoveData[j];
                sMoveData[j] = sMoveData[j+1];
                sMoveData[j+1] = temp;
            }
            else if (sMoveData[j].moveSlot == sMoveData[j+1].moveSlot)
            {
                // Primary key (moveSlot) is equal, compare secondary key (learnIndex)
                if (sMoveData[j].learnIndex > sMoveData[j+1].learnIndex)
                {
                    // Secondary key (learnIndex) is out of order, so swap
                    temp = sMoveData[j];
                    sMoveData[j] = sMoveData[j+1];
                    sMoveData[j+1] = temp;
                }
            }
        }
    }
}

MoveData *MoveChanger_GetMoveData(u8 i)
{
    if (i >= NELEMS(sMoveData)) return NULL;
    return &sMoveData[i];
}

// MoveData *MoveChanger_GetMoveDataAtCursor()
// {
//     return &sMoveData[sCursorIndex];
// }

// u16 MoveChanger_GetType(u8 i)
// {
//     return sMoveData[i].type;
// }

// u16 MoveChanger_GetCategory(u8 i)
// {
//     return sMoveData[i].category;
// }

// u16 MoveChanger_GetMove(u8 i)
// {
//     return sMoveData[i].id;
// }

// void MoveChanger_SelectMove()
// {
//     sSelectedMoveIndex = sCursorIndex;
// }

// void MoveChanger_DeselectMove()
// {
//     sSelectedMoveIndex = 0;
// }

// const u8 *MoveChanger_GetDescription() {
//     return gMoveDescriptionPointers[sCursorIndex];
// }

static void SetData(u32 index, u32 moveId)
{
    u8 power, accuracy;
    sMoveData[index].id = moveId;
    sMoveData[index].learnIndex = index;
    sMoveData[index].moveSlot = 255;
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
    u32 i, j, id;
    u16 species = GetMonData(pokemon, MON_DATA_SPECIES, NULL);
    u8 level = GetMonData(pokemon, MON_DATA_LEVEL, NULL);

    // ClearData();
    mon = pokemon;

    // for (i = 0; i < MAX_MON_MOVES; i++)
    // {
    //     id = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
    //     SetData(i, id);
    //     sMoveData[i].moveSlot = i;
    //     numMoves = i;
    // }

    for (i = 0; i < MAX_LEVEL_UP_MOVES; i++)
    {   
        id = gLevelUpLearnsets[species][i] & LEVEL_UP_MOVE_ID; // remove level encoding in the first 8 bits
        if (id == 0 || id >= 511)
            return numMoves;

        numMoves = i;
        SetData(i, id);

        for (j=0; j < MAX_MON_MOVES; j++)
        {
            if (id == GetMonData(mon, MON_DATA_MOVE1 + j, NULL))
            {
                sMoveData[i].moveSlot = j;
            }
        }
    }
    return numMoves;
}

// void MoveChanger_SetCursor(u32 cursorIndex)
// {
//     u16 move;
//     sCursorIndex = cursorIndex;
//     move = sMoveData[cursorIndex].id;
//     // DebugPrintf("cursor: %d move: %d",cursorIndex, sids[cursorIndex]);
// }


bool32 MoveChanger_SwapMonMoveSlots(u32 indexA, u32 indexB)
{
    u32 i;
    u16 moveA, moveB, temp;
    MoveData temp2;

    if (indexA > 3 && indexB > 3)
        return FALSE;
    
    if (indexA == indexB)
        return FALSE;

    moveA = sMoveData[indexA].id;
    moveB = sMoveData[indexB].id;
    if (indexA < indexB)
    {
        SetMonData(mon, MON_DATA_MOVE1 + indexA, &sMoveData[indexA].id);// (u8 *)&moveA);
        if (indexB < 4)
            SetMonData(mon, MON_DATA_MOVE1 + indexB, &sMoveData[indexB].id);
    }
    else
    {
        SetMonData(mon, MON_DATA_MOVE1 + indexB, &sMoveData[indexB].id);
        if (indexA < 4)
            SetMonData(mon, MON_DATA_MOVE1 + indexA, &sMoveData[indexA].id);
    }
        

    DebugPrintf("set mon data indexA %d -> indexB %d data? %d", indexA, indexB, GetMonData(mon, MON_DATA_MOVE1 + MIN(indexA, indexB), NULL));
    
    temp = sMoveData[indexA].moveSlot;
    temp2 = sMoveData[indexA];
    sMoveData[indexA].moveSlot = sMoveData[indexB].moveSlot;
    sMoveData[indexB].moveSlot = temp;

    sMoveData[indexA] = sMoveData[indexB];
    sMoveData[indexB] = temp2;
    // SortMoves();

    
    DebugPrintf("pokemon swap moves: %d <-> %d", sMoveData[indexA].moveSlot, sMoveData[indexB].moveSlot);
    return TRUE;
}