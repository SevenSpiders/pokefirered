

#include "DNS/DNS.h"
#include "global.h"


static u16 FadeColourForDNS(struct PlttData* blend, u8 coeff, s8 r, s8 g, s8 b)
{
	return ((r + (((blend->r - r) * coeff) >> 4)) << 0)
		 | ((g + (((blend->g - g) * coeff) >> 4)) << 5)
		 | ((b + (((blend->b - b) * coeff) >> 4)) << 10);

}

static u16 DarkenColor(struct PlttData* srcColor) {
    
    // Constants
    const u32 darkFactor = 128;   // ~0.5 in fixed point (0-255)
    const u32 blueFactor = 77;    // ~0.3 in fixed point (0-255)
    
    
    // Convert to 0-255 range for math
    u32 r = srcColor->r << 3;
    u32 g = srcColor->g << 3;
    u32 b = srcColor->b << 3;
    
    // Compute luminance (scaled integer math)
    // Y = 0.2126R + 0.7152G + 0.0722B
    u32 luminance = (54 * r + 183 * g + 19 * b) >> 8;  // weights sum to 256
    
    // Apply darkening
    r = (r * darkFactor) >> 8;
    g = (g * darkFactor) >> 8;
    b = (b * darkFactor) >> 8;
    
    // Add blue shift proportional to luminance
    b += (luminance * blueFactor) >> 8;
    if (b > 255) b = 255;

    return RGB2(r >> 3, g >> 3, b >> 3);
}


static void BlendFadedUnfadedPalette(u16 palOffset, u16 numEntries, u8 coeff, u32 blendColor, bool8 palFadeActive)
{
	u16 i;
	u16 ignoreOffset = palOffset / 16;

	for (i = 0; i < numEntries; ++i)
	{
		u16 index = i + palOffset;
        
        
		struct PlttData* color1 = (struct PlttData*) &gPlttBufferUnfaded[index];
		struct PlttData* color2 = (struct PlttData*) &blendColor;
		s8 r = color1->r;
		s8 g = color1->g;
		s8 b = color1->b;
        
		// u16 newColor = FadeColourForDNS(color2, coeff, r, g, b);
        u16 newColor = DarkenColor(color1);
		// if (gIgnoredDNSPalIndices[ignoreOffset][i]) continue; //Don't fade this index.
		if (gPlttBufferUnfaded[index] == RGB_BLACK) continue; //Don't fade black
		gPlttBufferUnfaded[index] = newColor;

		if (!palFadeActive)
			gPlttBufferFaded[index] = newColor;
	}
}





void TimeMixPalettes(u32 selectedPalettes, u16 *src, u16 *dst, struct BlendSettings *blend0, 
                    struct BlendSettings *blend1, u16 weight0) 
{

    u32 i, paletteOffset;

    // if (!selectedPalettes) 
    //     return;

    for (paletteOffset = 0; paletteOffset < 13 * 16; paletteOffset += 16) //Only background colours
	{
		if (selectedPalettes & 1)
			BlendFadedUnfadedPalette(paletteOffset, 16, 1, RGB_BLUE, FALSE);

		selectedPalettes >>= 1;
	}
}