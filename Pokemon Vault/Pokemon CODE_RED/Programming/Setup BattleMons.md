

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


---

what is hpOnSwitchout ➔ for strings when switching


---

Switch

	switchoutabilities BS_ATTACKER

    waitstate

    returnatktoball

    waitstate

    drawpartystatussummary BS_ATTACKER

    switchhandleorder BS_ATTACKER, SWITCH_ORDER_UPDATE_OWNER

    getswitchedmondata BS_ATTACKER

    switchindataupdate BS_ATTACKER

    hpthresholds BS_ATTACKER

    printstring STRINGID_SWITCHINMON

    hidepartystatussummary BS_ATTACKER

    switchinanim BS_ATTACKER, FALSE

    waitstate

    switchineffects BS_ATTACKER


in party menu

CursorCB_SendMon➔ TrySwitchInPokemon

SwitchPartyMonSlots
SwapPartyPokemon