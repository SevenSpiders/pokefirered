#ifndef GUARD_BATTLE_UI_H
#define GUARD_BATTLE_UI_H

void BattleUI_LoadGfx(void);
void BattleUI_Unload(void);
void BattleUI_DisplayMoveInfo(void);
void BattleUI_UpdateStatusIcons(u8 battlerId);
void BattleUI_UpdateBG0Offset(u16 x, u16 y);
void BattleUI_ShowStatusIcons(u8 battlerId, bool8 shouldShow);
// void BattleUI_DisplayMoveDescription(void);

#endif