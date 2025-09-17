

CB2_InitBattle ➔ 
CB2_InitBattleInternal ➔ clear sprites, VRAM, init windows and bgs
CB2_HandleStartBattle ➔ step 15
- InitBattleControllers
- BeginBattleIntro ➔ 
- BattleIntroGetMonsData ➔ buffer mon data in BufferB
- BattleIntroPrepareBackgroundSlide
- BattleIntroDrawTrainersOrMonsSprites
- BattleInitAllSprites ➔ ui + mon sprites




CopyPlayerPartyMonToBattleData?



Bug Moves in pokemon not correct

TryGenerateWildMon