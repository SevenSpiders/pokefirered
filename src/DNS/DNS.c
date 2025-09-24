
#include "DNS/DNS.h"
#include "rtc.h"
#include "gflib.h"
#include "constants/map_types.h"

#define DNS_UPDATE_COUNTER 10000

u8 gTimeOfDay;
BlendState currentTimeBlend;
s32 gTimeUpdateCounter;

const BlendSettings gTimeOfDayBlend[] =
{
    [TIME_OF_DAY_NIGHT] = {.coeff = 10, .blendColor = TINT_NIGHT, .isTint = TRUE},
    [TIME_OF_DAY_DAY] = {.coeff = 0, .blendColor = 0},
};


static u8 UpdateTimeOfDay(void);
void UpdatePalettesWithTime(u32 palettes);




void DNS_Update(void)
{
    // DebugPrintf("DNS_Update %d %d %d", gTimeUpdateCounter, !gPaletteFade.active);
    if (!gPaletteFade.active && --gTimeUpdateCounter <= 0) {
        BlendState cachedBlend = currentTimeBlend;
        u32 *bld0 = (u32*)&cachedBlend;
        u32 *bld1 = (u32*)&currentTimeBlend;

        DebugPrintf("UPDATE ");
        gTimeUpdateCounter = DNS_UPDATE_COUNTER;
        UpdateTimeOfDay();
        if (bld0[0] != bld1[0]
            || bld0[1] != bld1[1]
            || bld0[2] != bld1[2]
        ) {
           UpdatePalettesWithTime(PALETTES_ALL);
        }
    }
    else
    {
        DebugPrintf("DNS_Update %d", gTimeUpdateCounter);
    }
}


void UpdatePalettesWithTime(u32 palettes) {
    if (MapHasNaturalLight(gMapHeader.mapType)) {
        u32 i;
        u32 mask = 1 << 16;

        DebugPrintf("Update Palettes with time %d", palettes);

        for (i = 0; i < 16; i++, mask <<= 1)
        {
            if (IS_BLEND_IMMUNE_TAG(GetSpritePaletteTagByPaletteNum(i)))
                palettes &= ~(mask);
        }
        palettes &= PALETTES_MAP | PALETTES_OBJECTS; // Don't blend UI pals
        if (!palettes)
            return;
        TimeMixPalettes(
            palettes,
            gPlttBufferUnfaded,
            gPlttBufferFaded,
            &currentTimeBlend.bld0,
            &currentTimeBlend.bld1,
            currentTimeBlend.weight
        );
    }
}

// u8 UpdateSpritePaletteWithTime(u8 paletteNum) {
//     if (MapHasNaturalLight(gMapHeader.mapType)) {
//         if (IS_BLEND_IMMUNE_TAG(GetSpritePaletteTagByPaletteNum(paletteNum)))
//             return paletteNum;
//         TimeMixPalettes(
//             1,
//             &gPlttBufferUnfaded[OBJ_PLTT_ID(paletteNum)],
//             &gPlttBufferFaded[OBJ_PLTT_ID(paletteNum)],
//             &currentTimeBlend.bld0,
//             &currentTimeBlend.bld1,
//             currentTimeBlend.weight
//         );
//     }
//     return paletteNum;
// }


static u8 UpdateTimeOfDay(void) {
    s32 hours, minutes;
    struct Time *localTime = RTC_GetLocalTime();
    RtcCalcLocalTime();
    hours = localTime->hours;
    minutes = localTime->minutes;
    switch (hours)
    {
        case 0 ... 8: // night
            gTimeOfDay = TIME_OF_DAY_NIGHT;
            currentTimeBlend.bld0 = currentTimeBlend.bld1 = gTimeOfDayBlend[gTimeOfDay];
            currentTimeBlend.weight = 256;
            currentTimeBlend.altWeight = 0;
            break;

        case 9 ... 20: // night -> day
            currentTimeBlend.bld0 = gTimeOfDayBlend[TIME_OF_DAY_DAY];
            currentTimeBlend.weight = 256;
            currentTimeBlend.altWeight = 0;
            gTimeOfDay = TIME_OF_DAY_DAY;
            break;

        default: // day -> night
            currentTimeBlend.bld0 = gTimeOfDayBlend[TIME_OF_DAY_NIGHT];
            currentTimeBlend.weight = 256;
            currentTimeBlend.altWeight = 0;
            gTimeOfDay = TIME_OF_DAY_NIGHT;
            break;
    // case 0 ... 3: // night
    //     gTimeOfDay = TIME_OF_DAY_NIGHT;
    //     currentTimeBlend.bld0 = currentTimeBlend.bld1 = gTimeOfDayBlend[gTimeOfDay];
    //     currentTimeBlend.weight = 256;
    //     currentTimeBlend.altWeight = 0;
    //     break;
    // case 4 ... 6: // night -> twilight
    //     currentTimeBlend.bld0 = gTimeOfDayBlend[TIME_OF_DAY_NIGHT];
    //     currentTimeBlend.bld1 = gTimeOfDayBlend[TIME_OF_DAY_TWILIGHT];
    //     currentTimeBlend.weight = 256 - 256 * ((hours - 4) * 60 + minutes) / ((7-4)*60);
    //     currentTimeBlend.altWeight = (256 - currentTimeBlend.weight) / 2;
    //     gTimeOfDay = TIME_OF_DAY_DAY;
    //     break;
    // case 7 ... 9: // twilight -> day
    //     currentTimeBlend.bld0 = gTimeOfDayBlend[TIME_OF_DAY_TWILIGHT];
    //     currentTimeBlend.bld1 = gTimeOfDayBlend[TIME_OF_DAY_DAY];
    //     currentTimeBlend.weight = 256 - 256 * ((hours - 7) * 60 + minutes) / ((10-7)*60);
    //     currentTimeBlend.altWeight = (256 - currentTimeBlend.weight) / 2 + 128;
    //     gTimeOfDay = TIME_OF_DAY_DAY;
    //     break;
    // case 10 ... 17: // day
    //     gTimeOfDay = TIME_OF_DAY_DAY;
    //     currentTimeBlend.bld0 = currentTimeBlend.bld1 = gTimeOfDayBlend[gTimeOfDay];
    //     currentTimeBlend.weight = currentTimeBlend.altWeight = 256;
    //     break;
    // case 18 ... 19: // day -> twilight
    //     currentTimeBlend.bld0 = gTimeOfDayBlend[TIME_OF_DAY_DAY];
    //     currentTimeBlend.bld1 = gTimeOfDayBlend[TIME_OF_DAY_TWILIGHT];
    //     currentTimeBlend.weight = 256 - 256 * ((hours - 18) * 60 + minutes) / ((20-18)*60);
    //     currentTimeBlend.altWeight = currentTimeBlend.weight / 2 + 128;
    //     gTimeOfDay = TIME_OF_DAY_TWILIGHT;
    //     break;
    // case 20 ... 21: // twilight -> night
    //     currentTimeBlend.bld0 = gTimeOfDayBlend[TIME_OF_DAY_TWILIGHT];
    //     currentTimeBlend.bld1 = gTimeOfDayBlend[TIME_OF_DAY_NIGHT];
    //     currentTimeBlend.weight = 256 - 256 * ((hours - 20) * 60 + minutes) / ((22-20)*60);
    //     currentTimeBlend.altWeight = currentTimeBlend.weight / 2;
    //     gTimeOfDay = TIME_OF_DAY_NIGHT;
    //     break;
    // case 22 ... 24:
    //     gTimeOfDay = TIME_OF_DAY_NIGHT;
    //     currentTimeBlend.bld0 = currentTimeBlend.bld1 = gTimeOfDayBlend[gTimeOfDay];
    //     currentTimeBlend.weight = 256;
    //     currentTimeBlend.altWeight = 0;
    //     break;

    }

    return gTimeOfDay;
}