#ifndef GUARD_BATTLEMONS_H
#define GUARD_BATTLEMONS_H


void BattleMons_Init(void);
void BattleMons_Switch(u32 indexA, u32 indexB);
bool8 BattleMon_HasStatus(BattlePokemon *mon, u32 status);
bool8 BattleMon_AddStatus(BattlePokemon *mon, u32 status);
bool8 BattleMon_RemoveStatus(BattlePokemon *mon, u32 status);


#endif // GUARD_BATTLEMONS_H