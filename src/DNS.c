#include "gflib.h"
#include "DNS.h"

// void TintPaletteForDayNight(u16 offset, u16 size)
// {
//     u32 i;
//     DebugPrintf("tint palette");

//     if (IsMapTypeOutdoors(gMapHeader.mapType)) // should tint?
//     {

//         for(i=offset; i< offset + size; i++)
//         {
//             struct PlttData* color1 = (struct PlttData*) &gPlttBufferPreDNS[i];
//             DarkenColor(color1);
//         }
//     }
    
//     CpuCopy16(&gPlttBufferPreDNS[offset], &gPlttBufferUnfaded[offset], size);
//     // LoadPaletteOverrides();
// }

// static u16 DarkenColor(struct PlttData* srcColor) {
    
//     // Constants
//     const u32 darkFactor = 128;   // ~0.5 in fixed point (0-255)
//     const u32 blueFactor = 77;    // ~0.3 in fixed point (0-255)
    
    
//     // Convert to 0-255 range for math
//     u32 r = srcColor->r << 3;
//     u32 g = srcColor->g << 3;
//     u32 b = srcColor->b << 3;
    
//     // Compute luminance (scaled integer math)
//     // Y = 0.2126R + 0.7152G + 0.0722B
//     u32 luminance = (54 * r + 183 * g + 19 * b) >> 8;  // weights sum to 256
    
//     // Apply darkening
//     r = (r * darkFactor) >> 8;
//     g = (g * darkFactor) >> 8;
//     b = (b * darkFactor) >> 8;
    
//     // Add blue shift proportional to luminance
//     b += (luminance * blueFactor) >> 8;
//     if (b > 255) b = 255;

//     return RGB2(r >> 3, g >> 3, b >> 3);
// }


// static void BlendFadedUnfadedPalette(u16 palOffset, u16 numEntries, u8 coeff, u32 blendColor, bool8 palFadeActive)
// {
// 	u16 i;
// 	u16 ignoreOffset = palOffset / 16;

// 	for (i = 0; i < numEntries; ++i)
// 	{
// 		u16 index = i + palOffset;
        
        
// 		struct PlttData* color1 = (struct PlttData*) &gPlttBufferUnfaded[index];
// 		struct PlttData* color2 = (struct PlttData*) &blendColor;
// 		// s8 r = color1->r;
// 		// s8 g = color1->g;
// 		// s8 b = color1->b;
        
// 		// u16 newColor = FadeColourForDNS(color2, coeff, r, g, b);
//         u16 newColor = DarkenColor(color1);
// 		// if (gIgnoredDNSPalIndices[ignoreOffset][i]) continue; //Don't fade this index.
// 		if (gPlttBufferUnfaded[index] == RGB_BLACK) continue; //Don't fade black
// 		gPlttBufferUnfaded[index] = newColor;

// 		if (!palFadeActive)
// 			gPlttBufferFaded[index] = newColor;
// 	}
// }

// void TintPalette_CustomToneWithCopy(const u16 *src, u16 *dest, u16 count, u16 rTone, u16 gTone, u16 bTone, bool8 excludeZeroes)
// {
//     s32 r, g, b, i;
//     u32 gray;

//     for (i = 0; i < count; i++, src++, dest++)
//     {
//         if (excludeZeroes && *src == RGB_BLACK)
//             continue;

//         r = (*src >>  0) & 0x1F;
//         g = (*src >>  5) & 0x1F;
//         b = (*src >> 10) & 0x1F;

//         r = (u16)((rTone * r)) >> 8;
//         g = (u16)((gTone * g)) >> 8;
//         b = (u16)((bTone * b)) >> 8;

//         if (r > 31)
//             r = 31;
//         if (g > 31)
//             g = 31;
//         if (b > 31)
//             b = 31;

//         *dest = (b << 10) | (g << 5) | (r << 0);
//     }
// }