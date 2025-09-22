#ifndef GUARD_DNS_H
#define GUARD_DNS_H

#include "gflib.h"


#define TIME_OF_DAY_NIGHT 0
#define TIME_OF_DAY_DAY 1
#define TIME_OF_DAY_TWILIGHT 2
#define TIME_OF_DAY_DUSK 3

#define TINT_NIGHT Q_8_8(0.5) | Q_8_8(0.5) << 8 | Q_8_8(0.8) << 16

typedef struct BlendSettings
{
    u16 coeff;
    u32 blendColor;
    bool8 isTint;
} BlendSettings;

typedef struct
{
    BlendSettings bld0;
    BlendSettings bld1;
    u16 weight;
    u16 altWeight;
} BlendState;

extern u8 gTimeOfDay;


void DNS_Update(void);
void TimeMixPalettes(u32 palettes, u16 *src, u16 *dest, BlendSettings *bld0, BlendSettings *bld1, u16 weight);


#endif // GUARD_DNS_H