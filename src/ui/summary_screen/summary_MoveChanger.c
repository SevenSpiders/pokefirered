#include "main.h"
#include "ui/summary_screen/summary_MoveChanger.h"
#include "string_util.h"
#include "strings.h"
#include "characters.h"
#include "constants/moves.h"

#define MEMSET_ARR(arr, val) memset((arr), val, sizeof(arr))
#define MAX_LEARNABLE_MOVES 48 /* Reduced for GBA memory constraints */

extern const u8 *const gMoveDescriptionPointers[];

// /* Improved MoveData structure */
// typedef struct {
//     u16 moveId;
//     u8 learnLevel;
//     u8 learnMethod; /* 0=level up, 1=TM/HM, 2=tutor, 3=egg, etc. */
//     u8 isEquipped;  /* bool8 -> u8 for C89 */
//     u8 equipSlot;   /* 0-3 if equipped, 255 if not */
//     u8 type;
//     u8 category;
//     u8 power;
//     u8 accuracy;
//     u8 powerStr[4];
//     u8 accuracyStr[5];
// } LearnableMoveData;

static LearnableMoveData sLearnableMoves[MAX_LEARNABLE_MOVES];
static u8 sNumLearnableMoves;
static struct Pokemon *sPokemon;

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
    for (i = 0; i < sNumLearnableMoves - 1; i++)
    {
        swapped = FALSE;
        for (j = 0; j < sNumLearnableMoves - i - 1; j++)
        {
            /* Sort by: equipped status (equipped first), then learn method, then level/ID */
            if ((sLearnableMoves[j].isEquipped < sLearnableMoves[j+1].isEquipped) ||
                (sLearnableMoves[j].isEquipped == sLearnableMoves[j+1].isEquipped && 
                 sLearnableMoves[j].learnMethod > sLearnableMoves[j+1].learnMethod) ||
                (sLearnableMoves[j].isEquipped == sLearnableMoves[j+1].isEquipped && 
                 sLearnableMoves[j].learnMethod == sLearnableMoves[j+1].learnMethod &&
                 ((sLearnableMoves[j].learnMethod == 0 && sLearnableMoves[j].learnLevel > sLearnableMoves[j+1].learnLevel) ||
                  (sLearnableMoves[j].learnMethod != 0 && sLearnableMoves[j].moveId > sLearnableMoves[j+1].moveId))))
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

static void FormatMoveStats(LearnableMoveData *moveData)
{
    if (moveData->moveId == MOVE_NONE)
        return;
    
    /* Format power */
    if (moveData->power > 1) /* 1 represents 0 power */
        ConvertIntToDecimalStringN(moveData->powerStr, moveData->power, STR_CONV_MODE_RIGHT_ALIGN, 3);
    else
        StringCopy(moveData->powerStr, gText_ThreeHyphens);
    
    /* Format accuracy - Fixed bug: was using 'power' instead of 'accuracy' */
    if (moveData->accuracy > 1) /* 1 represents 0 accuracy (never misses) */
        ConvertIntToPercentageString(moveData->accuracyStr, moveData->accuracy, STR_CONV_MODE_RIGHT_ALIGN, 4);
    else
        StringCopy(moveData->accuracyStr, gText_ThreeHyphens);
}

static u8 IsMoveEquipped(u16 moveId)
{
    u32 i;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (GetMonData(sPokemon, MON_DATA_MOVE1 + i, NULL) == moveId)
            return TRUE;
    }
    return FALSE;
}

static u8 GetEquippedSlot(u16 moveId)
{
    u32 i;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (GetMonData(sPokemon, MON_DATA_MOVE1 + i, NULL) == moveId)
            return i;
    }
    return 255; /* Not equipped */
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
    return 255; /* Not found */
}

static void AddLearnableMove(u16 moveId, u8 learnLevel, u8 learnMethod)
{
    LearnableMoveData *move;
    u8 existingIndex;
    
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
    move->isEquipped = IsMoveEquipped(moveId);
    move->equipSlot = move->isEquipped ? GetEquippedSlot(moveId) : 255;
    
    /* Set move stats from battle data */
    move->type = gBattleMoves[moveId].type;
    move->category = gBattleMoves[moveId].category;
    move->power = gBattleMoves[moveId].power;
    move->accuracy = gBattleMoves[moveId].accuracy;
    
    FormatMoveStats(move);
    sNumLearnableMoves++;
}

static void PopulateLevelUpMoves(u16 species, u8 level)
{
    u32 i;
    u16 moveId;
    u8 moveLevel;
    u16 learnsetEntry;
    
    for (i = 0; i < MAX_LEVEL_UP_MOVES; i++)
    {
        learnsetEntry = gLevelUpLearnsets[species][i];
        if (learnsetEntry == LEVEL_UP_END)
            break;
            
        moveLevel = learnsetEntry >> 8; // 0xFE00
        moveId = learnsetEntry & LEVEL_UP_MOVE_ID; // 0x01FF
        
        /* Only include moves the Pokemon can learn at its current level or lower */
        // if (moveLevel <= level)
            AddLearnableMove(moveId, moveLevel, 0); /* 0 = level up method */
    }
}

/* Public API functions */
u32 MoveChanger_SetPokemon(struct Pokemon *pokemon)
{
    u16 species;
    u8 level;
    
    if (pokemon == NULL)
        return 0;
    
    species = GetMonData(pokemon, MON_DATA_SPECIES, NULL);
    level = GetMonData(pokemon, MON_DATA_LEVEL, NULL);
    
    ClearData();
    sPokemon = pokemon;
    
    /* Populate level-up moves */
    PopulateLevelUpMoves(species, level);
    
    /* TODO: Add TM/HM moves, tutor moves, etc. */
    /* PopulateTMMoves(species); */
    /* PopulateTutorMoves(species); */
    
    /* Sort moves for better organization */
    SortMoves();
    
    return sNumLearnableMoves;
}

LearnableMoveData *MoveChanger_GetMoveData(u8 index)
{
    if (index >= sNumLearnableMoves)
        return NULL;
    return &sLearnableMoves[index];
}

u8 MoveChanger_GetNumMoves(void)
{
    return sNumLearnableMoves;
}

u8 MoveChanger_GetNumEquippedMoves(void)
{
    u8 count;
    u32 i;
    
    count = 0;
    for (i = 0; i < sNumLearnableMoves; i++)
    {
        if (sLearnableMoves[i].isEquipped)
            count++;
    }
    return count;
}

u8 MoveChanger_GetNumUnequippedMoves(void)
{
    return sNumLearnableMoves - MoveChanger_GetNumEquippedMoves();
}

u32 MoveChanger_SwapMoves(u8 newMoveIndex, u8 slotToReplace)
{
    LearnableMoveData *moveToTeach;
    u16 oldMoveId;
    u32 i;
    
    if (newMoveIndex >= sNumLearnableMoves || slotToReplace >= MAX_MON_MOVES)
        return FALSE;
    
    moveToTeach = &sLearnableMoves[newMoveIndex];
    
    /* Can't teach a move that's already equipped */
    if (moveToTeach->isEquipped)
        return FALSE;
    
    /* Get the move currently in the slot we're replacing */
    oldMoveId = GetMonData(sPokemon, MON_DATA_MOVE1 + slotToReplace, NULL);
    
    /* Atomic swap - update Pokemon data first */
    SetMonData(sPokemon, MON_DATA_MOVE1 + slotToReplace, &moveToTeach->moveId);
    
    /* Update new move status */
    moveToTeach->isEquipped = TRUE;
    moveToTeach->equipSlot = slotToReplace;
    
    /* Update old move status in a single pass */
    for (i = 0; i < sNumLearnableMoves; i++)
    {
        if (sLearnableMoves[i].moveId == oldMoveId && sLearnableMoves[i].equipSlot == slotToReplace)
        {
            sLearnableMoves[i].isEquipped = FALSE;
            sLearnableMoves[i].equipSlot = 255;
            break;
        }
    }
    
    /* Single sort after both changes */
    SortMoves();
    
    return TRUE;
}

/* Separate function for just forgetting a move (if still needed) */
u32 MoveChanger_ForgetMove(u8 slotToForget)
{
    u16 moveToForget;
    u16 noMove;
    u32 i;
    
    if (slotToForget >= MAX_MON_MOVES)
        return FALSE;
    
    moveToForget = GetMonData(sPokemon, MON_DATA_MOVE1 + slotToForget, NULL);
    if (moveToForget == MOVE_NONE)
        return FALSE;
    
    /* Clear the move slot */
    noMove = MOVE_NONE;
    SetMonData(sPokemon, MON_DATA_MOVE1 + slotToForget, &noMove);
    
    /* Update data structure */
    for (i = 0; i < sNumLearnableMoves; i++)
    {
        if (sLearnableMoves[i].moveId == moveToForget && sLearnableMoves[i].equipSlot == slotToForget)
        {
            sLearnableMoves[i].isEquipped = FALSE;
            sLearnableMoves[i].equipSlot = 255;
            break;
        }
    }
    
    SortMoves();
    
    return TRUE;
}

/* Lightweight function to refresh equipped status without full rebuild */
void MoveChanger_RefreshEquippedStatus(void)
{
    u32 i;
    
    for (i = 0; i < sNumLearnableMoves; i++)
    {
        sLearnableMoves[i].isEquipped = IsMoveEquipped(sLearnableMoves[i].moveId);
        sLearnableMoves[i].equipSlot = sLearnableMoves[i].isEquipped ? 
            GetEquippedSlot(sLearnableMoves[i].moveId) : 255;
    }
    
    SortMoves();
}