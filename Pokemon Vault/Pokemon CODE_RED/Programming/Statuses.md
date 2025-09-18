
```
struct BattlePokemon
{

	u16 statuses[10]; // up to 10 statuses 
}

enum StatusTypes { 

	STATUS_NONE = 0, 
	// Persistent statuses: 1-255 (survive battles) 
	STATUS_SLEEP = 1, 
	STATUS_POISON = 2, 
	STATUS_BURN = 3, 
	STATUS_FREEZE = 4, 
	STATUS_PARALYSIS = 5, 
	STATUS_BAD_POISON = 6, 
	// ... up to 255 
	
	// Volatile statuses: 256-511 (cleared after battle) 
	VOLATILE_STATUS_START = 256, 
	STATUS_CONFUSION = 256, 
	STATUS_FLINCH = 257, 
	STATUS_TRAPPED = 258, 
	STATUS_IDENTIFIED = 259, 
	STATUS_PERISH_SONG = 260, 
	STATUS_ENCORE = 261, 
	STATUS_DISABLED = 262, 
	STATUS_TAUNT = 263, 
	STATUS_SUBSTITUTE = 264, // ... up to 511 
};

```

lots of data already

gLastMoves[gActiveBattler] = MOVE_NONE;

gLastLandedMoves[gActiveBattler] = MOVE_NONE;

gLastHitByType[gActiveBattler] = 0;

gLastResultingMoves[gActiveBattler] = MOVE_NONE;

gLastPrintedMoves[gActiveBattler] = MOVE_NONE;

gLastHitBy[gActiveBattler] = 0xFF;