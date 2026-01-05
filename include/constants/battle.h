#ifndef GUARD_CONSTANTS_BATTLE_H
#define GUARD_CONSTANTS_BATTLE_H

/*
 * A battler may be in one of four positions on the field. The first bit determines
 * what side the battler is on, either the player's side or the opponent's side.
 * The second bit determines what flank the battler is on, either the left or right.
 * Note that the opponent's flanks are drawn corresponding to their perspective, so
 * their right mon appears on the left, and their left mon appears on the right.
 * The battler ID is usually the same as the position, except in the case of link battles.
 *
 *   + ------------------------- +
 *   |           Opponent's side |
 *   |            Right    Left  |
 *   |              3       1    |
 *   |                           |
 *   | Player's side             |
 *   |  Left   Right             |
 *   |   0       2               |
 *   ----------------------------+
 *   |                           |
 *   |                           |
 *   +---------------------------+
 */

#define MAX_BATTLERS_COUNT  4

#define B_POSITION_PLAYER_LEFT        0
#define B_POSITION_OPPONENT_LEFT      1
#define B_POSITION_PLAYER_RIGHT       2
#define B_POSITION_OPPONENT_RIGHT     3

// These macros can be used with either battler ID or positions to get the partner or the opposite mon
#define BATTLE_OPPOSITE(id) ((id) ^ 1)
#define BATTLE_PARTNER(id) ((id) ^ 2)

#define B_SIDE_PLAYER     0
#define B_SIDE_OPPONENT   1

#define B_FLANK_LEFT 0
#define B_FLANK_RIGHT 1

#define BIT_SIDE        1
#define BIT_FLANK       2

// Battle Type Flags
#define BATTLE_TYPE_DOUBLE             (1 << 0)
#define BATTLE_TYPE_LINK               (1 << 1)
#define BATTLE_TYPE_IS_MASTER          (1 << 2) // In not-link battles, it's always set.
#define BATTLE_TYPE_TRAINER            (1 << 3)
#define BATTLE_TYPE_FIRST_BATTLE       (1 << 4)
#define BATTLE_TYPE_LINK_IN_BATTLE     (1 << 5) // Set on battle entry, cleared on exit. Checked rarely
#define BATTLE_TYPE_MULTI              (1 << 6)
#define BATTLE_TYPE_SAFARI             (1 << 7)
#define BATTLE_TYPE_BATTLE_TOWER       (1 << 8)
#define BATTLE_TYPE_OLD_MAN_TUTORIAL   (1 << 9) // Used in pokeemerald as BATTLE_TYPE_WALLY_TUTORIAL.
#define BATTLE_TYPE_ROAMER             (1 << 10)
#define BATTLE_TYPE_EREADER_TRAINER    (1 << 11)
#define BATTLE_TYPE_KYOGRE_GROUDON     (1 << 12)
#define BATTLE_TYPE_LEGENDARY          (1 << 13)
#define BATTLE_TYPE_GHOST_UNVEILED     (1 << 13) // Re-use of BATTLE_TYPE_LEGENDARY, when combined with BATTLE_TYPE_GHOST
#define BATTLE_TYPE_REGI               (1 << 14)
#define BATTLE_TYPE_GHOST              (1 << 15) // Used in pokeemerald as BATTLE_TYPE_TWO_OPPONENTS.
#define BATTLE_TYPE_POKEDUDE           (1 << 16) // Used in pokeemerald as BATTLE_TYPE_DOME.
#define BATTLE_TYPE_WILD_SCRIPTED      (1 << 17) // Used in pokeemerald as BATTLE_TYPE_PALACE.
#define BATTLE_TYPE_LEGENDARY_FRLG     (1 << 18) // Used in pokeemerald as BATTLE_TYPE_ARENA.
#define BATTLE_TYPE_TRAINER_TOWER      (1 << 19) // Used in pokeemerald as BATTLE_TYPE_FACTORY.

#define IS_BATTLE_TYPE_GHOST_WITHOUT_SCOPE(flags) ((flags) & BATTLE_TYPE_GHOST && !((flags) & BATTLE_TYPE_GHOST_UNVEILED))
#define IS_BATTLE_TYPE_GHOST_WITH_SCOPE(flags) ((flags) & BATTLE_TYPE_GHOST && (flags) & BATTLE_TYPE_GHOST_UNVEILED)

#define RIVAL_BATTLE_HEAL_AFTER  1
#define RIVAL_BATTLE_TUTORIAL    3

// Battle Outcome defines
#define B_OUTCOME_WON                  1
#define B_OUTCOME_LOST                 2
#define B_OUTCOME_DREW                 3
#define B_OUTCOME_RAN                  4
#define B_OUTCOME_PLAYER_TELEPORTED    5
#define B_OUTCOME_MON_FLED             6
#define B_OUTCOME_CAUGHT               7
#define B_OUTCOME_NO_SAFARI_BALLS      8
#define B_OUTCOME_FORFEITED            9
#define B_OUTCOME_MON_TELEPORTED       10
#define B_OUTCOME_LINK_BATTLE_RAN      (1 << 7) // 128

// NEW status
#define STATUS_NONE             0
#define STATUS_ANY_STATUS1      0
#define STATUS_SLEEP            1
#define STATUS_POISON           2
#define STATUS_BURN             3
#define STATUS_FREEZE           4
#define STATUS_PARALYSIS        5
#define STATUS_TOXIC            6
#define STATUS_MAX_STATUS1      6 // 1-6 are status1
#define STATUS_CONFUSED         7
#define STATUS_FLINCHED         8
#define STATUS_UPROAR           9
#define STATUS_BIDE             10
#define STATUS_MULTI_TURN       42
#define STATUS_THRASH           43
#define STATUS_WRAPPED          11
#define STATUS_INFATUATION      12
#define STATUS_FOCUS            13
#define STATUS_TRANSFORMED      14
#define STATUS_RECHARGE         15
#define STATUS_RAGE             16
#define STATUS_SUBSTITUTE       17
#define STATUS_DESTINY_BOND     18
#define STATUS_NO_ESCAPE        19
#define STATUS_NIGHTMARE        20
#define STATUS_CURSED           21
#define STATUS_FORESIGHT        22
#define STATUS_DEFENSE_CURL     23
#define STATUS_TORMENT          24
//status3
#define STATUS_LEECHSEED        25
#define STATUS_ALWAYS_HITS      26
#define STATUS_PERISH_SONG      27
#define STATUS_IN_AIR           28
#define STATUS_UNDERGROUND      29
#define STATUS_MINIMIZED        30
#define STATUS_CHARGED_UP       31
#define STATUS_ROOTED           32
#define STATUS_YAWN             33
#define STATUS_IMPRISONED       34
#define STATUS_GRUDGE           35
#define STATUS_NO_CRIT          36
#define STATUS_MUDSPORT         37
#define STATUS_WATERSPORT       38
#define STATUS_UNDERWATER       39
#define STATUS_INTIMIDATE       40
#define STATUS_TRACE            41
#define STATUS_END              43

// last 8bits = 256 types
// first 8 bits data -> 16 targetId & 16 levels
#define ENCODE_STATUS(type, data) (((u16)(data) << 8) | ((u16)(type) & 0xFF))
#define GET_STATUS_DATA(status) (((u16)(status) >> 8) & 0xFF)
#define GET_STATUS_TYPE(status) ((u16)(status) & 0xFF)

// Non-volatile status conditions
// These persist remain outside of battle and after switching out
#define STATUS1_NONE             0
#define STATUS1_SLEEP            (1 << 0 | 1 << 1 | 1 << 2) // First 3 bits (Number of turns to sleep)
#define STATUS1_SLEEP_TURN(num)  ((num) << 0) // Just for readability (or if rearranging statuses)
#define STATUS1_POISON           (1 << 3)
#define STATUS1_BURN             (1 << 4)
#define STATUS1_FREEZE           (1 << 5)
#define STATUS1_PARALYSIS        (1 << 6)
#define STATUS1_TOXIC_POISON     (1 << 7)
#define STATUS1_TOXIC_COUNTER    (1 << 8 | 1 << 9 | 1 << 10 | 1 << 11)
#define STATUS1_TOXIC_TURN(num)  ((num) << 8)
#define STATUS1_PSN_ANY          (STATUS1_POISON | STATUS1_TOXIC_POISON)
#define STATUS1_ANY              (STATUS1_SLEEP | STATUS1_POISON | STATUS1_BURN | STATUS1_FREEZE | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON)


// Not really sure what a "hitmarker" is.
#define HITMARKER_WAKE_UP_CLEAR         (1 << 4) // Cleared when waking up. Never set or checked.
#define HITMARKER_SKIP_DMG_TRACK        (1 << 5)
#define HITMARKER_DESTINYBOND           (1 << 6)
#define HITMARKER_NO_ANIMATIONS         (1 << 7)
#define HITMARKER_IGNORE_SUBSTITUTE     (1 << 8)
#define HITMARKER_NO_ATTACKSTRING       (1 << 9)
#define HITMARKER_ATTACKSTRING_PRINTED  (1 << 10)
#define HITMARKER_NO_PPDEDUCT           (1 << 11)
#define HITMARKER_SWAP_ATTACKER_TARGET  (1 << 12)
#define HITMARKER_STATUS_ABILITY_EFFECT (1 << 13)
#define HITMARKER_SYNCHRONISE_EFFECT    (1 << 14)
#define HITMARKER_RUN                   (1 << 15)
#define HITMARKER_IGNORE_ON_AIR         (1 << 16)
#define HITMARKER_IGNORE_UNDERGROUND    (1 << 17)
#define HITMARKER_IGNORE_UNDERWATER     (1 << 18)
#define HITMARKER_UNABLE_TO_USE_MOVE    (1 << 19)
#define HITMARKER_PASSIVE_DAMAGE        (1 << 20)
#define HITMARKER_DISOBEDIENT_MOVE      (1 << 21)
#define HITMARKER_PLAYER_FAINTED        (1 << 22)
#define HITMARKER_ALLOW_NO_PP           (1 << 23)
#define HITMARKER_GRUDGE                (1 << 24)
#define HITMARKER_OBEYS                 (1 << 25)
#define HITMARKER_NEVER_SET             (1 << 26) // Cleared as part of a large group. Never set or checked
#define HITMARKER_CHARGING              (1 << 27)
#define HITMARKER_FAINTED(battler)      (gBitTable[battler] << 28)
#define HITMARKER_FAINTED2(battler)     ((1 << 28) << battler)

// Per-side statuses that affect an entire party
#define SIDE_STATUS_REFLECT          (1 << 0)
#define SIDE_STATUS_LIGHTSCREEN      (1 << 1)
#define SIDE_STATUS_X4               (1 << 2)
#define SIDE_STATUS_SPIKES           (1 << 4)
#define SIDE_STATUS_SAFEGUARD        (1 << 5)
#define SIDE_STATUS_FUTUREATTACK     (1 << 6)
#define SIDE_STATUS_MIST             (1 << 8)
#define SIDE_STATUS_SPIKES_DAMAGED   (1 << 9)

// Flags describing move's result
#define MOVE_RESULT_MISSED             (1 << 0)
#define MOVE_RESULT_SUPER_EFFECTIVE    (1 << 1)
#define MOVE_RESULT_NOT_VERY_EFFECTIVE (1 << 2)
#define MOVE_RESULT_DOESNT_AFFECT_FOE  (1 << 3)
#define MOVE_RESULT_ONE_HIT_KO         (1 << 4)
#define MOVE_RESULT_FAILED             (1 << 5)
#define MOVE_RESULT_FOE_ENDURED        (1 << 6)
#define MOVE_RESULT_FOE_HUNG_ON        (1 << 7)
#define MOVE_RESULT_NO_EFFECT          (MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE | MOVE_RESULT_FAILED)

// Battle Weather flags
#define B_WEATHER_RAIN_TEMPORARY      (1 << 0)
#define B_WEATHER_RAIN_DOWNPOUR       (1 << 1)  // unused
#define B_WEATHER_RAIN_PERMANENT      (1 << 2)
#define B_WEATHER_RAIN                (B_WEATHER_RAIN_TEMPORARY | B_WEATHER_RAIN_DOWNPOUR | B_WEATHER_RAIN_PERMANENT)
#define B_WEATHER_SANDSTORM_TEMPORARY (1 << 3)
#define B_WEATHER_SANDSTORM_PERMANENT (1 << 4)
#define B_WEATHER_SANDSTORM           (B_WEATHER_SANDSTORM_TEMPORARY | B_WEATHER_SANDSTORM_PERMANENT)
#define B_WEATHER_SUN_TEMPORARY       (1 << 5)
#define B_WEATHER_SUN_PERMANENT       (1 << 6)
#define B_WEATHER_SUN                 (B_WEATHER_SUN_TEMPORARY | B_WEATHER_SUN_PERMANENT)
#define B_WEATHER_HAIL_TEMPORARY      (1 << 7)
#define B_WEATHER_HAIL                (B_WEATHER_HAIL_TEMPORARY)
#define B_WEATHER_ANY                 (B_WEATHER_RAIN | B_WEATHER_SANDSTORM | B_WEATHER_SUN | B_WEATHER_HAIL)

// Move Effects
#define MOVE_EFFECT_SLEEP               1
#define MOVE_EFFECT_POISON              2
#define MOVE_EFFECT_BURN                3
#define MOVE_EFFECT_FREEZE              4
#define MOVE_EFFECT_PARALYSIS           5
#define MOVE_EFFECT_TOXIC               6
#define PRIMARY_STATUS_MOVE_EFFECT      MOVE_EFFECT_TOXIC // All above move effects apply primary status
#define MOVE_EFFECT_CONFUSION           7
#define MOVE_EFFECT_FLINCH              8
#define MOVE_EFFECT_TRI_ATTACK          9
#define MOVE_EFFECT_UPROAR              10
#define MOVE_EFFECT_PAYDAY              11
#define MOVE_EFFECT_CHARGING            12
#define MOVE_EFFECT_WRAP                13
#define MOVE_EFFECT_RECOIL_25           14
#define MOVE_EFFECT_ATK_PLUS_1          15
#define MOVE_EFFECT_DEF_PLUS_1          16
#define MOVE_EFFECT_SPD_PLUS_1          17
#define MOVE_EFFECT_SP_ATK_PLUS_1       18
#define MOVE_EFFECT_SP_DEF_PLUS_1       19
#define MOVE_EFFECT_ACC_PLUS_1          20
#define MOVE_EFFECT_EVS_PLUS_1          21
#define MOVE_EFFECT_ATK_MINUS_1         22
#define MOVE_EFFECT_DEF_MINUS_1         23
#define MOVE_EFFECT_SPD_MINUS_1         24
#define MOVE_EFFECT_SP_ATK_MINUS_1      25
#define MOVE_EFFECT_SP_DEF_MINUS_1      26
#define MOVE_EFFECT_ACC_MINUS_1         27
#define MOVE_EFFECT_EVS_MINUS_1         28
#define MOVE_EFFECT_RECHARGE            29
#define MOVE_EFFECT_RAGE                30
#define MOVE_EFFECT_STEAL_ITEM          31
#define MOVE_EFFECT_PREVENT_ESCAPE      32
#define MOVE_EFFECT_NIGHTMARE           33
#define MOVE_EFFECT_ALL_STATS_UP        34
#define MOVE_EFFECT_RAPIDSPIN           35
#define MOVE_EFFECT_REMOVE_PARALYSIS    36
#define MOVE_EFFECT_ATK_DEF_DOWN        37
#define MOVE_EFFECT_RECOIL_33           38
#define MOVE_EFFECT_ATK_PLUS_2          39
#define MOVE_EFFECT_DEF_PLUS_2          40
#define MOVE_EFFECT_SPD_PLUS_2          41
#define MOVE_EFFECT_SP_ATK_PLUS_2       42
#define MOVE_EFFECT_SP_DEF_PLUS_2       43
#define MOVE_EFFECT_ACC_PLUS_2          44
#define MOVE_EFFECT_EVS_PLUS_2          45
#define MOVE_EFFECT_ATK_MINUS_2         46
#define MOVE_EFFECT_DEF_MINUS_2         47
#define MOVE_EFFECT_SPD_MINUS_2         48
#define MOVE_EFFECT_SP_ATK_MINUS_2      49
#define MOVE_EFFECT_SP_DEF_MINUS_2      50
#define MOVE_EFFECT_ACC_MINUS_2         51
#define MOVE_EFFECT_EVS_MINUS_2         52
#define MOVE_EFFECT_THRASH              53
#define MOVE_EFFECT_KNOCK_OFF           54
#define MOVE_EFFECT_NOTHING_37          55
#define MOVE_EFFECT_NOTHING_38          56
#define MOVE_EFFECT_NOTHING_39          57
#define MOVE_EFFECT_NOTHING_3A          58
#define MOVE_EFFECT_SP_ATK_TWO_DOWN     59
#define NUM_MOVE_EFFECTS                60

#define MOVE_EFFECT_AFFECTS_USER        (1 << 6) // 64
#define MOVE_EFFECT_CERTAIN             (1 << 7) // 128

// Battle terrain defines for gBattleTerrain.
#define BATTLE_TERRAIN_GRASS        0
#define BATTLE_TERRAIN_LONG_GRASS   1
#define BATTLE_TERRAIN_SAND         2
#define BATTLE_TERRAIN_UNDERWATER   3
#define BATTLE_TERRAIN_WATER        4
#define BATTLE_TERRAIN_POND         5
#define BATTLE_TERRAIN_MOUNTAIN     6
#define BATTLE_TERRAIN_CAVE         7
#define BATTLE_TERRAIN_BUILDING     8
#define BATTLE_TERRAIN_PLAIN        9
#define BATTLE_TERRAIN_LINK        10
#define BATTLE_TERRAIN_GYM         11
#define BATTLE_TERRAIN_LEADER      12
#define BATTLE_TERRAIN_INDOOR_2    13
#define BATTLE_TERRAIN_INDOOR_1    14
#define BATTLE_TERRAIN_LORELEI     15
#define BATTLE_TERRAIN_BRUNO       16
#define BATTLE_TERRAIN_AGATHA      17
#define BATTLE_TERRAIN_LANCE       18
#define BATTLE_TERRAIN_CHAMPION    19

#define B_WAIT_TIME_LONG  64
#define B_WAIT_TIME_MED   48
#define B_WAIT_TIME_SHORT 32

#define CASTFORM_NORMAL     0
#define CASTFORM_FIRE       1
#define CASTFORM_WATER      2
#define CASTFORM_ICE        3
#define NUM_CASTFORM_FORMS  4
#define CASTFORM_SUBSTITUTE (1 << 7)

#define FLEE_ITEM    1
#define FLEE_ABILITY 2

// Return value for IsRunningFromBattleImpossible.
#define BATTLE_RUN_SUCCESS        0
#define BATTLE_RUN_FORBIDDEN      1
#define BATTLE_RUN_FAILURE        2

// Window Ids for sTextOnWindowsInfo_Normal
#define B_WIN_MSG                 0
#define B_WIN_ACTION_PROMPT       1 // "What will {x} do?"
#define B_WIN_ACTION_MENU         2 // "Fight/Pokémon/Bag/Run" menu
#define B_WIN_MOVE_NAME_1         3 // Top left
#define B_WIN_MOVE_NAME_2         4 // Top right
#define B_WIN_MOVE_NAME_3         5 // Bottom left
#define B_WIN_MOVE_NAME_4         6 // Bottom right
#define B_WIN_PP                  7
#define B_WIN_MOVE_TYPE           8
#define B_WIN_PP_REMAINING        9
#define B_WIN_DUMMY              10
#define B_WIN_SWITCH_PROMPT      11 // "Switch which?"
#define B_WIN_LEVEL_UP_BOX       12
#define B_WIN_LEVEL_UP_BANNER    13
#define B_WIN_YESNO              14
#define B_WIN_VS_PLAYER          15
#define B_WIN_VS_OPPONENT        16
#define B_WIN_VS_MULTI_PLAYER_1  17
#define B_WIN_VS_MULTI_PLAYER_2  18
#define B_WIN_VS_MULTI_PLAYER_3  19
#define B_WIN_VS_MULTI_PLAYER_4  20
#define B_WIN_VS_OUTCOME_DRAW    21
#define B_WIN_VS_OUTCOME_LEFT    22
#define B_WIN_VS_OUTCOME_RIGHT   23
#define B_WIN_OAK_OLD_MAN        24
#define B_WIN_STATUS1            25
#define B_WIN_STATUS2            26

#define B_WIN_STATUS_W           13 //in tiles
#define B_WIN_STATUS_H           1  //in tiles

// Palettes
#define B_PLTT_UI               0
#define B_PLTT_FRAME            1
#define B_PLTT_TERRAIN          2
#define B_PLTT_TERRAIN2         3
#define B_PLTT_TERRAIN3         4
#define B_PLTT_TEXT             5
#define B_PLTT_6                6 // DrawBattleEntryBackground, level up, attack?
#define B_PLTT_7                7 // FIRST BATTLE
#define B_PLTT_TYPE             13
#define B_PLTT_ICONS            12
#define B_PLTT_POKEMON          8 // player1
#define B_PLTT_POKEMON2         9 // opponent1
#define B_PLTT_POKEMON3         10
#define B_PLTT_POKEMON4         11
#define B_PLTT_12               12 // free
#define B_PLTT_13               13 // union room?
#define B_PLTT_14               14 // egg hatch?
#define B_PLTT_15               15 // sliding pokeball intro, mugshot intro
// object palettes
#define B_PLTT_16               16 // pokemon1
#define B_PLTT_17               17 // pokemon2
#define B_PLTT_18               18 // pokemon3
#define B_PLTT_19               19 // pokemon4

// Tiles
#define B_TILE_FRAME1           18 // 27
#define B_TILE_FRAME2           34 // 43

#define B_TILE_MSG              144 // 255
#define B_TILE_ICONS            256 // 319 status icons
#define B_TILE_LEVEL_UP         256 // 365 level up stats
#define B_TILE_MINI_BOX         366 // 401 tiny pokemon representation
#define B_TILE_ACTIONS          400 // 455
#define B_TILE_PROMPT           448 // 503 what will x do?

#define B_TILE_YESNO            512 // 527 was 256 - 272
#define B_TILE_TEMP             512 // 576 temporary copy of enemy pokemon, grass swoosh intro

#define B_TILE_PWR              656 // 661
#define B_TILE_TYPE             662 // 677
#define B_TILE_POWER            678 // 687
#define B_TILE_TEMP2            768 // 832 temporary copy of player pokemon
#define B_TILE_MOVE1            768 // 783
#define B_TILE_MOVE2            784 // 799
#define B_TILE_MOVE3            800 // 815
#define B_TILE_MOVE4            816 // 831
#define B_TILE_TERRAIN          1025 // 1112

#define B_TEXT_VALUE_MASK               0x3F
#define B_TEXT_FLAG_MASK                0xC0
#define B_TEXT_FLAG_NPC_CONTEXT_FONT    (1 << 6)
#define B_TEXT_FLAG_WINDOW_CLEAR        (1 << 7)

// Indicator for the party summary bar to display an empty slot.
#define HP_EMPTY_SLOT 0xFFFF

#endif // GUARD_CONSTANTS_BATTLE_H
