#include "main.h"
#include "gflib.h" // maloc
#include "ui/summary_screen/summary_Page_MoveInfos.h"

#define FREE_AND_SET_NULL_IF_SET(ptr) \
{                                     \
    if (ptr != NULL)                  \
    {                                 \
        free(ptr);                    \
        (ptr) = NULL;                 \
    }                                 \
}

#define BLINK_SPEED 30

struct MoveSelectionCursor
{
    struct Sprite *sprite; /* 0x00 */
    u16 whichSprite; /* 0x04 */
    u16 tileTag; /* 0x06 */
    u16 palTag; /* 0x08 */
};

static const struct OamData sMoveSelectionCursorOamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0
};

static const union AnimCmd sMoveSelectionCursorOamAnim_Red[] = 
{
    ANIMCMD_FRAME(0, 20),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sMoveSelectionCursorOamAnim_Blue[] = 
{
    ANIMCMD_FRAME(0x20, 20),
    ANIMCMD_JUMP(0),
};


static const union AnimCmd * const sMoveSelectionCursorOamAnimTable[] =
{
    sMoveSelectionCursorOamAnim_Red,
    sMoveSelectionCursorOamAnim_Blue
};

// static EWRAM_DATA u8 sMoveSelectionCursorPos = 0;
// static EWRAM_DATA bool8 isSwappingMoves = FALSE;
static EWRAM_DATA struct MoveSelectionCursor * sMoveSelectionCursorObjs[4] = {};
static const u32 sMoveSelectionCursorTiles_Left[] = INCBIN_U32("graphics/summary_screen/move_selection_cursor_left.4bpp.lz");
static const u32 sMoveSelectionCursorTiles_Right[] = INCBIN_U32("graphics/summary_screen/move_selection_cursor_right.4bpp.lz");
static const u16 sMoveSelectionCursorPals[] = INCBIN_U16("graphics/summary_screen/move_selection_cursor.gbapal");

static void SpriteCB_MoveSelectionCursor(struct Sprite *sprite);



void SS_ShoworHideMoveSelectionCursor(bool8 invisible)
{
    u8 i;
    for (i = 0; i < 4; i++)
        sMoveSelectionCursorObjs[i]->sprite->invisible = invisible;
}



void SS_CreateMoveSelectionCursorObjs(u16 tileTag, u16 palTag)
{
    u8 i;
    u8 spriteId;
    void *gfxBufferPtrs[2];
    gfxBufferPtrs[0] = AllocZeroed(0x20 * 64);
    gfxBufferPtrs[1] = AllocZeroed(0x20 * 64);

    sMoveSelectionCursorObjs[0] = AllocZeroed(sizeof(struct MoveSelectionCursor));
    sMoveSelectionCursorObjs[1] = AllocZeroed(sizeof(struct MoveSelectionCursor));
    sMoveSelectionCursorObjs[2] = AllocZeroed(sizeof(struct MoveSelectionCursor));
    sMoveSelectionCursorObjs[3] = AllocZeroed(sizeof(struct MoveSelectionCursor));

    LZ77UnCompWram(sMoveSelectionCursorTiles_Left, gfxBufferPtrs[0]);
    LZ77UnCompWram(sMoveSelectionCursorTiles_Right, gfxBufferPtrs[1]);

    for (i = 0; i < 4; i++)
    {
        struct SpriteSheet sheet = {
            .data = gfxBufferPtrs[i % 2],
            .size = 0x20 * 64,
            .tag = tileTag + i
        };

        struct SpritePalette palette = {.data = sMoveSelectionCursorPals, .tag = palTag};
        struct SpriteTemplate template = {
            .tileTag = tileTag + i,
            .paletteTag = palTag,
            .oam = &sMoveSelectionCursorOamData,
            .anims = sMoveSelectionCursorOamAnimTable,
            .images = NULL,
            .affineAnims = gDummySpriteAffineAnimTable,
            .callback = SpriteCB_MoveSelectionCursor,
        };

        LoadSpriteSheet(&sheet);
        LoadSpritePalette(&palette);

        spriteId = CreateSprite(&template, 64 * (i % 2) + 152, Page_GetCursor() * 28 + 34, i % 2);
        sMoveSelectionCursorObjs[i]->sprite = &gSprites[spriteId];
        sMoveSelectionCursorObjs[i]->whichSprite = i;
        sMoveSelectionCursorObjs[i]->tileTag = tileTag + i;
        sMoveSelectionCursorObjs[i]->palTag = palTag;
        sMoveSelectionCursorObjs[i]->sprite->subpriority = i;

        if (i > 1)
            StartSpriteAnim(sMoveSelectionCursorObjs[i]->sprite, 1);
    }

    SS_ShoworHideMoveSelectionCursor(TRUE);

    FREE_AND_SET_NULL_IF_SET(gfxBufferPtrs[0]);
    FREE_AND_SET_NULL_IF_SET(gfxBufferPtrs[1]);
}



static void SpriteCB_MoveSelectionCursor(struct Sprite *sprite)
{
    u8 i;
    u8 cursorY = Page_GetCursor() * 28 + 34;
    u8 selectionCursorY;
    u8 selectionCursorIndex = Page_GetSelectionCursor();
    bool8 isSwapping = Page_IsSwapping();

    if (selectionCursorIndex < 5)
        selectionCursorY = selectionCursorIndex * 28 + 34;


    // Update Y position of cursor sprites
    for (i = 0; i < 4; i++)
    {
        if (isSwapping && i >= 2 && selectionCursorIndex < 5) // update selected cursor
        {
            sMoveSelectionCursorObjs[i]->sprite->y = selectionCursorY;
        }
        else 
            sMoveSelectionCursorObjs[i]->sprite->y = cursorY;
    }

    if (!isSwapping)
    {
        if (Page_IsScrolling())
        {
            sMoveSelectionCursorObjs[0]->sprite->invisible = FALSE;
            sMoveSelectionCursorObjs[1]->sprite->invisible = FALSE;
        }
        return;
    }

    // Handle blinking effect when swapping
    for (i = 0; i < 2; i++)
    {
        struct Sprite *cursorSprite = sMoveSelectionCursorObjs[i]->sprite;
        
        if (selectionCursorIndex > 4) // out of range
            cursorSprite->invisible = TRUE;

        else if (++cursorSprite->data[0] > BLINK_SPEED)
        {
            cursorSprite->invisible ^= TRUE;  // toggle invisibility
            cursorSprite->data[0] = 0;
        }
    }
}
