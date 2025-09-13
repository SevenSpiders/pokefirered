


### BUILD
make -j16
start pokefirered.gba


#### Mods

max 10 mods on a pokemon
64 differnt buffs 

1 bit stackable
5 bits enum -> 32 stacked buffs ( none, 6 stats: atk, sp atk, def, sp def, speed, acc, ) : 25 left
4 bits counters -> 16 stacks ( +/- 7)

u32 -> 3 buffs
-> 4 u32 -> 12 buffs



move task to moveinfo page class
➔ Task_HandleInput_SelectMove ( scroll)
➔ Task_InputHandler_Info ( non scroll)

CreateMoveSelectionCursorObjs // creat cursor
SpriteCB_MoveSelectionCursor // draws cursor
sMoveSelectionCursorObjs // cursor

why does bag interface print item description to battle window 9 ?


```
// Player controller
static void OpenBagAndChooseItem(void)
{
    if (!gPaletteFade.active)
    {
        gBattlerControllerFuncs[gActiveBattler] = CompleteWhenChoseItem;
        ReshowBattleScreenDummy();
        FreeAllWindowBuffers();
        CB2_BagMenuFromBattle();
    }
}

static void CompleteWhenChoseItem(void)
{
    if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active)
    {
        BtlController_EmitOneReturnValue(1, gSpecialVar_ItemId);
        PlayerBufferExecCompleted();
    }
}
```


sprite replacement ➔ gMonFrontPicTable


move swap summary screen

task_handleinput_selectMove
SwapMonMoveSlots()

what is this:
    u8 __attribute__((aligned(4))) inhibitPageFlipInput;



---

Override stat clearing

Battle_Main.c
➔  SwitchInClearSetData
BattleStartClearSetData

gStatuses3?
gDisableStructs



Task_PokeSum_FlipPages -> needs to include draw boxes + update


adding more sprites to display

ScrCmd_showcontestpainting



#### Delete Map

data/layouts ➔ delete folder
data/maps ➔ delete folder

event_scripts.inc ➔ delete line references
layouts.json ➔ delete entry
map_groups.json ➔ delete entry line ( depends on map group )

run: make clean