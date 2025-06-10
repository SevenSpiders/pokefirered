#include "main.h"
#include "ui/summary_screen/summary_MoveHandler.h"
#include "string_util.h"
#include "strings.h"
#include "characters.h"
#include "constants/moves.h"

#define MEMSET_ARR(arr, val) memset((arr), val, sizeof(arr))
#define MAX_LEARNABLE_MOVES 48 /* Reduced for GBA memory constraints */
#define EMTPY_SLOT 255
#define LEARN_METHOD_LEVEL  0
#define LEARN_METHOD_TM     1
#define LEARN_METHOD_EGG    2
#define LEARN_METHOD_TUTOR  3

extern const u8 *const gMoveDescriptionPointers[];

static LearnableMoveData sLearnableMoves[MAX_LEARNABLE_MOVES];
static u8 sNumLearnableMoves;
static struct Pokemon *sPokemon;

static void RefreshMoveSlots(void);
static void InvalidateMoveSlots(void);



/* Simple bubble sort optimized for small datasets */
static void SortMoves(void)
{
    u8 i, j;
    LearnableMoveData temp;
    u8 swapped;
    
    /* Early exit if no moves to sort */
    if (sNumLearnableMoves <= 1)
        return;
    
    /* Optimized bubble sort with early termination */
    for (i = MAX_MON_MOVES; i < sNumLearnableMoves - 1; i++)
    {
        swapped = FALSE;
        for (j = MAX_MON_MOVES; j < sNumLearnableMoves - i - 1; j++)
        {
            /* Sort by: learn method, then level/ID */
            if (sLearnableMoves[j].learnMethod > sLearnableMoves[j+1].learnMethod ||
                (sLearnableMoves[j].learnMethod == sLearnableMoves[j+1].learnMethod &&
                 sLearnableMoves[j].learnLevel > sLearnableMoves[j+1].learnLevel) ||
                (sLearnableMoves[j].learnMethod == sLearnableMoves[j+1].learnMethod &&
                 sLearnableMoves[j].learnLevel == sLearnableMoves[j+1].learnLevel &&
                 sLearnableMoves[j].moveId > sLearnableMoves[j+1].moveId))
            {
                temp = sLearnableMoves[j];
                sLearnableMoves[j] = sLearnableMoves[j+1];
                sLearnableMoves[j+1] = temp;
                swapped = TRUE;
            }
        }
        if (!swapped)
            break; /* Array is sorted */
    }
}

static void ClearData(void)
{
    MEMSET_ARR(sLearnableMoves, 0);
    sNumLearnableMoves = 0;
    sPokemon = NULL;
}

/* Check if move already exists in our list to avoid duplicates */
static u8 FindMoveInList(u16 moveId)
{
    u8 i;
    for (i = 0; i < sNumLearnableMoves; i++)
    {
        if (sLearnableMoves[i].moveId == moveId)
            return i;
    }
    return EMTPY_SLOT; /* Not found */
}

static void AddEmptyMove()
{
    LearnableMoveData *move;

    move = &sLearnableMoves[sNumLearnableMoves];
    move->moveId = MOVE_NONE;
    StringCopy(move->powerStr, gText_ThreeHyphens);
    sNumLearnableMoves++;
}

static void AddLearnableMove(u16 moveId, u8 learnLevel, u8 learnMethod)
{
    LearnableMoveData *move;
    u32 existingIndex, power, accuracy;
    
    if (sNumLearnableMoves >= MAX_LEARNABLE_MOVES || moveId == MOVE_NONE)
        return;
    
    /* Check for duplicates */
    existingIndex = FindMoveInList(moveId);
    if (existingIndex != 255)
    {
        /* Update existing entry with better learn method if needed */
        if (learnMethod < sLearnableMoves[existingIndex].learnMethod)
        {
            sLearnableMoves[existingIndex].learnMethod = learnMethod;
            sLearnableMoves[existingIndex].learnLevel = learnLevel;
        }
        return;
    }
    
    move = &sLearnableMoves[sNumLearnableMoves];
    
    move->moveId = moveId;
    move->learnLevel = learnLevel;
    move->learnMethod = learnMethod;
    
    /* Set move stats from battle data */
    move->type = gBattleMoves[moveId].type;
    move->category = gBattleMoves[moveId].category;
    power = gBattleMoves[moveId].power;
    accuracy = gBattleMoves[moveId].accuracy;

    if (power > 1) /* 1 represents 0 power */
        ConvertIntToDecimalStringN(move->powerStr, power, STR_CONV_MODE_RIGHT_ALIGN, 3);
    else
        StringCopy(move->powerStr, gText_ThreeHyphens);
    
    /* Format accuracy - Fixed bug: was using 'power' instead of 'accuracy' */
    if (accuracy > 1) /* 1 represents 0 accuracy (never misses) */
        ConvertIntToPercentageString(move->accuracyStr, accuracy, STR_CONV_MODE_RIGHT_ALIGN, 4);
    else
        StringCopy(move->accuracyStr, gText_ThreeHyphens);
    
    sNumLearnableMoves++;
}

static void PopulateLearnedMoves(struct Pokemon *pokemon)
{
    u32 i;
    u16 moveId;
    for (i =0; i< MAX_MON_MOVES; i++)
    {
        moveId = GetMonData(pokemon, MON_DATA_MOVE1 + i, NULL);
        if (moveId == MOVE_NONE)
            AddEmptyMove();
        else
            AddLearnableMove(moveId, 0, LEARN_METHOD_LEVEL);
    }
}

static void PopulateLevelUpMoves(u16 species, u8 level)
{
    u32 i, j;
    bool32 isEquipped;
    u16 moveId;
    u8 moveLevel;
    u16 learnsetEntry;
    
    for (i = 0; i < MAX_LEVEL_UP_MOVES; i++)
    {
        learnsetEntry = gLevelUpLearnsets[species][i];
        if (learnsetEntry == LEVEL_UP_END)
            break;
            
        moveLevel = learnsetEntry >> 8;
        moveId = learnsetEntry & LEVEL_UP_MOVE_ID;

        isEquipped = FALSE;
        for (j=0; j< MAX_MON_MOVES; j++)
        {
            if (moveId == sLearnableMoves[j].moveId)
            {
                isEquipped = TRUE;
                break; // Move already exists, no need to add again
            }
        }
        
        /* Only include moves the Pokemon can learn at its current level or lower */
        if (moveLevel <= level && isEquipped == FALSE)
            AddLearnableMove(moveId, moveLevel, LEARN_METHOD_LEVEL); /* 0 = level up method */
    }
}

/* Public API functions */
u32 MoveHandler_SetPokemon(struct Pokemon *pokemon)
{
    u16 species;
    u8 level;
    
    if (pokemon == NULL)
        return 0;
    
    species = GetMonData(pokemon, MON_DATA_SPECIES, NULL);
    level = 100;//GetMonData(pokemon, MON_DATA_LEVEL, NULL);
    
    ClearData();
    sPokemon = pokemon;
    
    PopulateLearnedMoves(pokemon);
    PopulateLevelUpMoves(species, level);
    
    /* TODO: Add TM/HM moves, tutor moves, etc. */
    /* PopulateTMMoves(species); */
    /* PopulateTutorMoves(species); */
    
    return sNumLearnableMoves;
}

LearnableMoveData *MoveHandler_GetMoveData(u8 index)
{
    if (index >= sNumLearnableMoves)
        return NULL;
    
    return &sLearnableMoves[index];
}

u32 MoveHandler_SwapMoves(struct Pokemon * mon, u8 indexA, u8 indexB)
{
    LearnableMoveData *moveToTeach;
    LearnableMoveData temp;
    u32 i, oldMoveIndex, newMoveIndex, slotToReplace;

    slotToReplace = MIN(indexA, indexB);
    newMoveIndex = MAX(indexA, indexB);
    
    if (newMoveIndex >= sNumLearnableMoves || slotToReplace >= MAX_MON_MOVES)
        return FALSE;
    
    moveToTeach = &sLearnableMoves[newMoveIndex];    
    SetMonData(mon, MON_DATA_MOVE1 + slotToReplace, &moveToTeach->moveId);

    if (newMoveIndex < MAX_MON_MOVES)
    {
        moveToTeach = &sLearnableMoves[newMoveIndex];
        SetMonData(mon, MON_DATA_MOVE1 + newMoveIndex, &moveToTeach->moveId);
    }
    
    temp = sLearnableMoves[indexA];
    sLearnableMoves[indexA] = sLearnableMoves[indexB];
    sLearnableMoves[indexB] = temp;
    SortMoves();
    
    return TRUE;
}

u8 MoveHandler_GetNumMoves(void)
{
    return sNumLearnableMoves;
}

// u32 MoveHandler_ForgetMove(u8 slotToForget)
// {
//     u16 moveToForget;
//     u16 noMove;
//     u32 i;
    
//     if (slotToForget >= MAX_MON_MOVES)
//         return FALSE;
    
//     moveToForget = MoveHandler_GetSlot(slotToForget);
//     if (moveToForget == MOVE_NONE)
//         return FALSE;
    
//     /* Clear the move slot */
//     noMove = MOVE_NONE;
//     SetMonData(sPokemon, MON_DATA_MOVE1 + slotToForget, &noMove);
    
//     /* Update cache immediately */
//     sSlotIndex[slotToForget] = MOVE_NONE;
    
//     /* Update data structure */
//     for (i = 0; i < sNumLearnableMoves; i++)
//     {
//         if (sLearnableMoves[i].moveId == moveToForget && sLearnableMoves[i].equipSlot == slotToForget)
//         {
//             sLearnableMoves[i].isEquipped = FALSE;
//             sLearnableMoves[i].equipSlot = 255;
//             break;
//         }
//     }
    
//     SortMoves();
    
//     return TRUE;
// }
