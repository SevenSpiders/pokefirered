
gba resolution
240px x 160px
30 x 20 tiles

nds resolution
256x192

4 background layers ➔ tiles
1 object layer ➔ Sprites


[How Menus Work ‐ Part 1 · pret/pokeemerald Wiki](https://github.com/pret/pokeemerald/wiki/How-Menus-Work-%E2%80%90-Part-1)


### Background

- tileset ➔ [pixels]
- tilemap ➔ [(tilesetID, palette, data)]


### VRAM

- 96KB
- 4 tilesets a 512 entries 
- ➔ 2048 tileset entries
- ➔ 4 x 16KB = 64KB
- ➔ 32KB for sprites
- 256x256 sprite ➔ 32KB


DMA = Direct Memory Access

### I/O Registers

-  `0x04000010 = BG0HOFS` ( horizontal offset for bg0 )
- `BLDCNT = effect type`
- choose alpha or transparency blending
- `BLDALPHA = alpha_reg`
- `BLDY = brigthness`


### Screen Windows

- 4 regions ➔ 3 windows  +  1 not a window (NotAWindow)
- main: Window0  +  Window1
- window ➔ layer visiblity + effect types


### Example

```

// set Window0 to (0,0) pos
SetGpuReg(REG_OFFSET_WIN0H,    0); // horizontal edge = 0
SetGpuReg(REG_OFFSET_WIN0V,    0); // vertical edge = 0

// define what layers are active in windows
SetGpuReg(REG_OFFSET_WININ,    WININ_WIN0_BG0); // only bg0

// define layers in NotAWindow
SetGpuReg(REG_OFFSET_WINOUT,   WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_CLR); // ➔ bg0 + bg1 + colorEffect are active

// darken bg0 in window1
SetGpuReg(REG_OFFSET_BLDCNT,   BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_DARKEN);

// don't use alpha blend
SetGpuReg(REG_OFFSET_BLDALPHA, 0);

// set brightness 4
SetGpuReg(REG_OFFSET_BLDY,     4);

// dispatch window0 
SetGpuReg(REG_OFFSET_DISPCNT,  DISPCNT_WIN0_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);

```


Use in Code

```
static void HighlightOptionMenuItem(u8 index)
{
	// left right edges [ 16, 234]
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(16, DISPLAY_WIDTH - 16));
    
    // top bottom edges [40, 56] + index* [16, 16]
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(index * 16 + 40, index * 16 + 56));
}
```


## Question ??
- why do backgrounds have size?
- what determines where tilemap goes? ➔ GetTileMapIndexFromCoords
- how do tilePixels and tilemap get drawn?
- where are move anim tile maps stored ➔ bounds
- how do other roms store text ➔ zelda minishcap

Tilemap index:
```
&((u16 *)sGpuBgConfigs2[bg].tilemap)[(u16)GetTileMapIndexFromCoords(x16, y16, attribute, mode, mode2)]
```


max tiles ➔ 2048 
- bg0 [0, 511]
- bg1 [512, 1023] 0x200
- bg2 [1024, 1535] 0x400
- bg3 [1536, 2047] 0x600

observations
- tilemap starts at 1564 // NotAWindow

issue #1 
- combat box ( base 768) is printed to background 3 (0,  0) when drawing moves
- tail whip uses wrong graphics
- ➔ commit "New Icons" ➔  "Not Working"
- free? ➔ 372 - 399 ( 27)

before issue
- def down sprite tile starts at 512
- move category icon (2x2) starts block 688 ➔ tile 1646
- what is at T 720? issues if switch prompt higher?


issue #2: corrupted status icons
- B_WIN_HB_STATUS_0 ( base 934) is not being redrawn when exiting bag
```
// battlecontroller player
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


// callbacks ➔ reshow_battle_screen.c
  

void ReshowBattleScreenAfterMenu(void)
```



battle controller has 2 main functions
- PlayerHandleChooseAction ➔ battle, items, pokemon, flee
- PlayerHandleChooseMove ➔ move 1,2,3,4