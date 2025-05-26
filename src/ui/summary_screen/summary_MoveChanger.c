#include "main.h"
#include "ui/summary_screen/summary_MoveChanger.h"
#include "string_util.h"
#include "strings.h"
#include "characters.h"
#include "constants/moves.h" // temp hyper beam

#define MEMSET_ARR(arr, val) memset((arr), val, sizeof(arr))
#define NUM_MOVEDATAS 25

extern const u8 *const gMoveDescriptionPointers[];

static MoveData sMoveData[NUM_MOVEDATAS];

static u8 numMoves;
static struct Pokemon * mon;

static void ClearData()
{
    MEMSET_ARR(sMoveData, 0);
    numMoves = 0;
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
    bool32 isEquipped;

    ClearData();
    mon = pokemon;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        id = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
        SetData(i, id);
        sMoveData[i].moveSlot = i;
        numMoves = i;
    }

    for (i = 0; i < MAX_LEVEL_UP_MOVES; i++)
    {   
        id = gLevelUpLearnsets[species][i] & LEVEL_UP_MOVE_ID; // remove level encoding in the first 8 bits
        if (id == 0 || id >= 511) // end of learnset
            return numMoves;

        isEquipped = FALSE;
        numMoves = i;
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            if (id == GetMonData(mon, MON_DATA_MOVE1 + j, NULL))
            {
                isEquipped = TRUE;
                break;
            }
        }
        if (isEquipped)
            continue; // already known
        
        SetData(i, id);
    }
    return numMoves;
}

bool32 MoveChanger_SwapMonMoveSlots(struct Pokemon *mon0, u32 indexA, u32 indexB)
{
    MoveData tempMove;

    if (indexA > 3 && indexB > 3)
        return FALSE;

    if (indexA == indexB)
        return FALSE;

    // Swap the MoveData structs
    tempMove = sMoveData[indexA];
    sMoveData[indexA] = sMoveData[indexB];
    sMoveData[indexB] = tempMove;

    // Swap their moveSlot fields back to preserve original slot identity
    {
        u16 tempSlot = sMoveData[indexA].moveSlot;
        sMoveData[indexA].moveSlot = sMoveData[indexB].moveSlot;
        sMoveData[indexB].moveSlot = tempSlot;
    }

    // Update Pokémon move list if needed
    if (indexA < 4)
        SetMonData(mon0, MON_DATA_MOVE1 + indexA, (u8 *)&sMoveData[indexA].id);

    if (indexB < 4)
        SetMonData(mon0, MON_DATA_MOVE1 + indexB, (u8 *)&sMoveData[indexB].id);

    DebugPrintf("pokemon swap moves: %d <-> %d", sMoveData[indexA].moveSlot, sMoveData[indexB].moveSlot);

    return TRUE;
}

