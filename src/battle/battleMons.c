#include "global.h"
#include "battle/battleMons.h"
#include "gflib.h" // stringcopy


EWRAM_DATA BattlePokemon gBattleMons[PARTY_SIZE + PARTY_SIZE] = {0};
// EWRAM_DATA BattlePokemon gBattleMons[4] = {0};

static BattlePokemon CreateBattleMon(Pokemon *mon);


void BattleMons_Init()
{
    u32 i, move;
    Pokemon mon;

    for(i=0; i< PARTY_SIZE; i++) // PARTY_SIZE
    {   
        gBattleMons[i*2] = CreateBattleMon(&gPlayerParty[i]);
        gBattleMons[i*2+1] = CreateBattleMon(&gEnemyParty[i]);
    }

}


static BattlePokemon CreateBattleMon(Pokemon *mon)
{
    u32 i;
    u8 nickname[20];
    BattlePokemon battleMon = {0};
    u32 species = mon->box.struct0.species;
    // DebugPrintf("Create mon: species %d, level %d", species, mon->level);
    
    battleMon.species = species;
    if (species == SPECIES_NONE)
        return battleMon;

    battleMon.item = GetMonData(mon, MON_DATA_HELD_ITEM);
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        battleMon.moves[i] = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
        battleMon.pp[i] = GetMonData(mon, MON_DATA_PP1 + i, NULL);
    }
    battleMon.ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES, NULL);
    battleMon.friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, NULL);
    battleMon.experience = GetMonData(mon, MON_DATA_EXP, NULL);
    battleMon.hpIV = GetMonData(mon, MON_DATA_HP_IV, NULL);
    battleMon.attackIV = GetMonData(mon, MON_DATA_ATK_IV, NULL);
    battleMon.defenseIV = GetMonData(mon, MON_DATA_DEF_IV, NULL);
    battleMon.speedIV = GetMonData(mon, MON_DATA_SPEED_IV, NULL);
    battleMon.spAttackIV = GetMonData(mon, MON_DATA_SPATK_IV, NULL);
    battleMon.spDefenseIV = GetMonData(mon, MON_DATA_SPDEF_IV, NULL);
    battleMon.personality = GetMonData(mon, MON_DATA_PERSONALITY, NULL);
    battleMon.status1 = GetMonData(mon, MON_DATA_STATUS, NULL);
    battleMon.level = GetMonData(mon, MON_DATA_LEVEL, NULL);
    battleMon.hp = GetMonData(mon, MON_DATA_HP, NULL);
    battleMon.maxHP = GetMonData(mon, MON_DATA_MAX_HP, NULL);
    battleMon.attack = GetMonData(mon, MON_DATA_ATK, NULL);
    battleMon.defense = GetMonData(mon, MON_DATA_DEF, NULL);
    battleMon.speed = GetMonData(mon, MON_DATA_SPEED, NULL);
    battleMon.spAttack = GetMonData(mon, MON_DATA_SPATK, NULL);
    battleMon.spDefense = GetMonData(mon, MON_DATA_SPDEF, NULL);
    battleMon.isEgg = GetMonData(mon, MON_DATA_IS_EGG, NULL);
    battleMon.abilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM, NULL);
    battleMon.otId = GetMonData(mon, MON_DATA_OT_ID, NULL);
    GetMonData(mon, MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(battleMon.nickname, nickname);
    GetMonData(mon, MON_DATA_OT_NAME, battleMon.otName);

    battleMon.type1 = gSpeciesInfo[species].types[0];
    battleMon.type2 = gSpeciesInfo[species].types[1];
    battleMon.ability = GetAbilityBySpecies(species, battleMon.abilityNum);

    DebugPrintf("Init battlemon: mon%d, type=%d, ability=%d move0=%d", species, battleMon.type1, battleMon.ability, battleMon.moves[0]);
    return battleMon;
}