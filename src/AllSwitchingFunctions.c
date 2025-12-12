

#pragma region Batte_main.c
EWRAM_DATA struct BattlePokemon gBattleMons[MAX_BATTLERS_COUNT] = {0};

#pragma endregion


#pragma region BattleScripts.c
static void Cmd_switchindataupdate(void)
{
    struct BattlePokemon oldData;
    s32 i;
    u8 *monData;

    if (gBattleControllerExecFlags)
        return;

    gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    oldData = gBattleMons[gActiveBattler];
    monData = (u8 *)(&gBattleMons[gActiveBattler]);

    for (i = 0; i < sizeof(struct BattlePokemon); i++)
        monData[i] = gBattleBufferB[gActiveBattler][4 + i];

    gBattleMons[gActiveBattler].type1 = gSpeciesInfo[gBattleMons[gActiveBattler].species].types[0];
    gBattleMons[gActiveBattler].type2 = gSpeciesInfo[gBattleMons[gActiveBattler].species].types[1];
    gBattleMons[gActiveBattler].ability = GetAbilityBySpecies(gBattleMons[gActiveBattler].species, gBattleMons[gActiveBattler].abilityNum);

    // check knocked off item
    i = GetBattlerSide(gActiveBattler);
    if (gWishFutureKnock.knockedOffMons[i] & gBitTable[gBattlerPartyIndexes[gActiveBattler]])
    {
        gBattleMons[gActiveBattler].item = ITEM_NONE;
    }

    if (gBattleMoves[gCurrentMove].effect == EFFECT_BATON_PASS)
    {
        for (i = 0; i < NUM_BATTLE_STATS; i++)
        {
            gBattleMons[gActiveBattler].statStages[i] = oldData.statStages[i];
        }
        gBattleMons[gActiveBattler].status2 = oldData.status2;
    }

    SwitchInClearSetData();

    gBattleScripting.battler = gActiveBattler;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gActiveBattler, gBattlerPartyIndexes[gActiveBattler]);

    gBattlescriptCurrInstr += 2;
}
#pragma endregion BattleScripts.c

#pragma region  BattleControler.c
void InitBattleControllers(void)
{
    s32 i;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        InitLinkBtlControllers();
    else
        InitSinglePlayerBtlControllers();

    SetBattlePartyIds();

    if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
    {
        for (i = 0; i < gBattlersCount; i++)
            BufferBattlePartyCurrentOrderBySide(i, 0);
    }
}

static void PlayerHandleChooseItem(void)
{
    s32 i;

    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
    gBattlerControllerFuncs[gActiveBattler] = OpenBagAndChooseItem;
    gBattlerInMenuId = gActiveBattler;
    for (i = 0; i < 3; ++i)
        gBattlePartyCurrentOrder[i] = gBattleBufferA[gActiveBattler][1 + i];
}

static void PlayerHandleChoosePokemon(void)
{
    s32 i;

    gBattleControllerData[gActiveBattler] = CreateTask(TaskDummy, 0xFF);
    gTasks[gBattleControllerData[gActiveBattler]].data[0] = gBattleBufferA[gActiveBattler][1] & 0xF;
    *(&gBattleStruct->battlerPreventingSwitchout) = gBattleBufferA[gActiveBattler][1] >> 4;
    *(&gBattleStruct->playerPartyIdx) = gBattleBufferA[gActiveBattler][2];
    *(&gBattleStruct->abilityPreventingSwitchout) = gBattleBufferA[gActiveBattler][3];
    for (i = 0; i < 3; ++i)
        gBattlePartyCurrentOrder[i] = gBattleBufferA[gActiveBattler][4 + i];
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
    gBattlerControllerFuncs[gActiveBattler] = OpenPartyMenuToChooseMon;
    gBattlerInMenuId = gActiveBattler;
}

static void OpenPartyMenuToChooseMon(void)
{
    if (!gPaletteFade.active)
    {
        u8 caseId;

        gBattlerControllerFuncs[gActiveBattler] = WaitForMonSelection;
        caseId = gTasks[gBattleControllerData[gActiveBattler]].data[0];
        DestroyTask(gBattleControllerData[gActiveBattler]);
        FreeAllWindowBuffers();
        OpenPartyMenuInBattle(caseId);
    }
}

#pragma endregion


#pragma region Party_Menu.c

#define PARTY_SIZE 6
EWRAM_DATA struct Pokemon gPlayerParty[PARTY_SIZE] = {};
ALIGNED(4) EWRAM_DATA u8 gBattlePartyCurrentOrder[PARTY_SIZE / 2] = {0}; // bits 0-3 are the current pos of Slot 1, 4-7 are Slot 2, and so on



static bool8 TrySwitchInPokemon(void)
{
    u8 slot = GetCursorSelectionMonId();
    u8 newSlot;
    u8 i;

    // In a multi battle, slots 1, 4, and 5 are the partner's pokemon
    if (IsMultiBattle() == TRUE && (slot == 1 || slot == 4 || slot == 5))
    {
        StringCopy(gStringVar1, GetTrainerPartnerName());
        StringExpandPlaceholders(gStringVar4, gText_CantSwitchWithAlly);
        return FALSE;
    }
    if (GetMonData(&gPlayerParty[slot], MON_DATA_HP) == 0)
    {
        GetMonNickname(&gPlayerParty[slot], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnHasNoEnergy);
        return FALSE;
    }
    for (i = 0; i < gBattlersCount; ++i)
    {
        if (GetBattlerSide(i) == B_SIDE_PLAYER && GetPartyIdFromBattleSlot(slot) == gBattlerPartyIndexes[i])
        {
            GetMonNickname(&gPlayerParty[slot], gStringVar1);
            StringExpandPlaceholders(gStringVar4, gText_PkmnAlreadyInBattle);
            return FALSE;
        }
    }
    if (GetMonData(&gPlayerParty[slot], MON_DATA_IS_EGG))
    {
        StringExpandPlaceholders(gStringVar4, gText_EggCantBattle);
        return FALSE;
    }
    if (GetPartyIdFromBattleSlot(slot) == gBattleStruct->playerPartyIdx)
    {
        GetMonNickname(&gPlayerParty[slot], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnAlreadySelected);
        return FALSE;
    }
    if (gPartyMenu.action == PARTY_ACTION_ABILITY_PREVENTS)
    {
        SetMonPreventsSwitchingString();
        return FALSE;
    }
    if (gPartyMenu.action == PARTY_ACTION_CANT_SWITCH)
    {
        u8 currBattler = gBattlerInMenuId;

        GetMonNickname(&gPlayerParty[GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[currBattler])], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnCantSwitchOut);
        return FALSE;
    }
    gSelectedMonPartyId = GetPartyIdFromBattleSlot(slot);
    gPartyMenuUseExitCallback = TRUE;
    newSlot = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[gBattlerInMenuId]);
    SwitchPartyMonSlots(newSlot, slot);
    SwapPartyPokemon(&gPlayerParty[newSlot], &gPlayerParty[slot]);
    return TRUE;
}

static void SetPartyIdAtBattleSlot(u8 slot, u8 setVal)
{
    bool32 modResult = slot & 1;

    slot /= 2;
    if (modResult != 0)
        gBattlePartyCurrentOrder[slot] = (gBattlePartyCurrentOrder[slot] & 0xF0) | setVal;
    else
        gBattlePartyCurrentOrder[slot] = (gBattlePartyCurrentOrder[slot] & 0xF) | (setVal << 4);
}

void SwitchPartyMonSlots(u8 slot, u8 slot2)
{
    u8 partyId = GetPartyIdFromBattleSlot(slot);

    SetPartyIdAtBattleSlot(slot, GetPartyIdFromBattleSlot(slot2));
    SetPartyIdAtBattleSlot(slot2, partyId);
}

static void SwapPartyPokemon(struct Pokemon *mon1, struct Pokemon *mon2)
{
    struct Pokemon *buffer = Alloc(sizeof(struct Pokemon));

    *buffer = *mon1;
    *mon1 = *mon2;
    *mon2 = *buffer;
    Free(buffer);
}

void BufferBattlePartyCurrentOrder(void)
{
    BufferBattlePartyOrder(gBattlePartyCurrentOrder, GetPlayerFlankId());
}

static void BufferBattlePartyOrder(u8 *partyBattleOrder, u8 flankId)
{
    u8 partyIds[PARTY_SIZE];
    s32 i, j;

    if (IsMultiBattle() == TRUE)
    {
        // Party ids are packed in 4 bits at a time
        // i.e. the party id order below would be 0, 3, 5, 4, 2, 1, and the two parties would be 0,5,4 and 3,2,1
        if (flankId != 0)
        {
            partyBattleOrder[0] = 0 | (3 << 4);
            partyBattleOrder[1] = 5 | (4 << 4);
            partyBattleOrder[2] = 2 | (1 << 4);
        }
        else
        {
            partyBattleOrder[0] = 3 | (0 << 4);
            partyBattleOrder[1] = 2 | (1 << 4);
            partyBattleOrder[2] = 5 | (4 << 4);
        }
        return;
    }
    else if (IsDoubleBattle() == FALSE)
    {
        j = 1;
        partyIds[0] = gBattlerPartyIndexes[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)];
        for (i = 0; i < PARTY_SIZE; ++i)
        {
            if (i != partyIds[0])
            {
                partyIds[j] = i;
                ++j;
            }
        }
    }
    else
    {
        j = 2;
        partyIds[0] = gBattlerPartyIndexes[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)];
        partyIds[1] = gBattlerPartyIndexes[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)];
        for (i = 0; i < PARTY_SIZE; ++i)
        {
            if (i != partyIds[0] && i != partyIds[1])
            {
                partyIds[j] = i;
                ++j;
            }
        }
    }
    for (i = 0; i < (s32)NELEMS(gBattlePartyCurrentOrder); ++i)
        partyBattleOrder[i] = (partyIds[0 + (i * 2)] << 4) | partyIds[1 + (i * 2)];
}

void BufferBattlePartyCurrentOrderBySide(u8 battlerId, u8 flankId)
{
    BufferBattlePartyOrderBySide(gBattleStruct->battlerPartyOrders[battlerId], flankId, battlerId);
}

// when GetBattlerSide(battlerId) == B_SIDE_PLAYER, this function is identical the one above
static void BufferBattlePartyOrderBySide(u8 *partyBattleOrder, u8 flankId, u8 battlerId)
{
    u8 partyIndexes[PARTY_SIZE];
    s32 i, j;
    u8 leftBattler;
    u8 rightBattler;

    if (GetBattlerSide(battlerId) == B_SIDE_PLAYER)
    {
        leftBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        rightBattler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
    }
    else
    {
        leftBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        rightBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
    }
    if (IsMultiBattle() == TRUE)
    {
        if (flankId != 0)
        {
            partyBattleOrder[0] = 0 | (3 << 4);
            partyBattleOrder[1] = 5 | (4 << 4);
            partyBattleOrder[2] = 2 | (1 << 4);
        }
        else
        {
            partyBattleOrder[0] = 3 | (0 << 4);
            partyBattleOrder[1] = 2 | (1 << 4);
            partyBattleOrder[2] = 5 | (4 << 4);
        }
        return;
    }
    else if (IsDoubleBattle() == FALSE)
    {
        j = 1;
        partyIndexes[0] = gBattlerPartyIndexes[leftBattler];
        for (i = 0; i < PARTY_SIZE; ++i)
        {
            if (i != partyIndexes[0])
            {
                partyIndexes[j] = i;
                ++j;
            }
        }
    }
    else
    {
        j = 2;
        partyIndexes[0] = gBattlerPartyIndexes[leftBattler];
        partyIndexes[1] = gBattlerPartyIndexes[rightBattler];
        for (i = 0; i < PARTY_SIZE; ++i)
        {
            if (i != partyIndexes[0] && i != partyIndexes[1])
            {
                partyIndexes[j] = i;
                ++j;
            }
        }
    }
    for (i = 0; i < 3; ++i)
        partyBattleOrder[i] = (partyIndexes[0 + (i * 2)] << 4) | partyIndexes[1 + (i * 2)];
}

#pragma endregion