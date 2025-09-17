#include "global.h"
#include "battle/battleMons.h"
#include "gflib.h" // stringcopy


EWRAM_DATA struct BattlePokemon gBattleMons[PARTY_SIZE + PARTY_SIZE] = {0};

static BattlePokemon CreateBattleMon(Pokemon mon);


void BattleMons_Init()
{
    u32 i, species;
    Pokemon mon;

    for(i=0; i< PARTY_SIZE; i++)
    {
        gBattleMons[i*2] = CreateBattleMon(gPlayerParty[i]);
        gBattleMons[i*2+1] = CreateBattleMon(gEnemyParty[i]);
    }

}


static BattlePokemon CreateBattleMon(Pokemon mon)
{
    u32 i;
    u8 nickname[20];
    u32 species = GetMonData(mon, MON_DATA_SPECIES);
    BattlePokemon battleMon;
    DebugPrintf("Create mon: species %d, level %d", species, mon.level);
    
    battleMon.species = species;
    battleMon.item = GetMonData(mon, MON_DATA_HELD_ITEM);
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        battleMon.moves[i] = GetMonData(mon, MON_DATA_MOVE1 + i);
        battleMon.pp[i] = GetMonData(mon, MON_DATA_PP1 + i);
    }
    battleMon.ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
    battleMon.friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
    battleMon.experience = GetMonData(mon, MON_DATA_EXP);
    battleMon.hpIV = GetMonData(mon, MON_DATA_HP_IV);
    battleMon.attackIV = GetMonData(mon, MON_DATA_ATK_IV);
    battleMon.defenseIV = GetMonData(mon, MON_DATA_DEF_IV);
    battleMon.speedIV = GetMonData(mon, MON_DATA_SPEED_IV);
    battleMon.spAttackIV = GetMonData(mon, MON_DATA_SPATK_IV);
    battleMon.spDefenseIV = GetMonData(mon, MON_DATA_SPDEF_IV);
    battleMon.personality = GetMonData(mon, MON_DATA_PERSONALITY);
    battleMon.status1 = GetMonData(mon, MON_DATA_STATUS);
    battleMon.level = GetMonData(mon, MON_DATA_LEVEL);
    battleMon.hp = GetMonData(mon, MON_DATA_HP);
    battleMon.maxHP = GetMonData(mon, MON_DATA_MAX_HP);
    battleMon.attack = GetMonData(mon, MON_DATA_ATK);
    battleMon.defense = GetMonData(mon, MON_DATA_DEF);
    battleMon.speed = GetMonData(mon, MON_DATA_SPEED);
    battleMon.spAttack = GetMonData(mon, MON_DATA_SPATK);
    battleMon.spDefense = GetMonData(mon, MON_DATA_SPDEF);
    battleMon.isEgg = GetMonData(mon, MON_DATA_IS_EGG);
    battleMon.abilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM);
    battleMon.otId = GetMonData(mon, MON_DATA_OT_ID);
    GetMonData(mon, MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(battleMon.nickname, nickname);
    GetMonData(mon, MON_DATA_OT_NAME, battleMon.otName);

    battleMon.type1 = gSpeciesInfo[species].types[0];
    battleMon.type2 = gSpeciesInfo[species].types[1];
    battleMon.ability = GetAbilityBySpecies(species, battleMon.abilityNum);

    // DebugPrintf("Init battlemon: mon%d, type=%d, ability=%d", species, battleMon.type1, battleMon.ability);
    return battleMon;
}