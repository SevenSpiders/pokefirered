

// #include "DNS/DNS.h"
// #include "global.h"

// struct RGB {
//     s32 r, g, b;
// };


// // Extract RGB from 15-bit GBA color
// struct RGB ExtractGBARGB(u16 color) {
//     struct RGB rgb;
//     rgb.r = (color << 27) >> 27;
//     rgb.g = (color << 22) >> 27;
//     rgb.b = (color << 17) >> 27;
//     return rgb;
// }

// // struct RGB ExtractGBARGB(u16 color) {
// //     struct RGB out;

// //     // Extract 5-bit values
// //     u32 r5 =  color        & 0x1F;
// //     u32 g5 = (color >> 5)  & 0x1F;
// //     u32 b5 = (color >> 10) & 0x1F;

// //     // Expand to 0–255 range for easier math later
// //     out.r = (r5 << 3) | (r5 >> 2);  // replicate bits to fill 8 bits
// //     out.g = (g5 << 3) | (g5 >> 2);
// //     out.b = (b5 << 3) | (b5 >> 2);

// //     return out;
// // }




// static void Darken(u16 *src, u16 *dst) {
    
//     // Constants
//     const u32 darkFactor = 128;   // ~0.5 in fixed point (0-255)
//     const u32 blueFactor = 77;    // ~0.3 in fixed point (0-255)
//     u32 i;

//     // Copy color 0 (transparent) unchanged
//     *dst++ = *src++;

//     // Process colors 1-15
//     for (i = 1; i < 16; i++) {
//         u16 srcColor = *src++;
//         struct RGB srcRGB = ExtractGBARGB(srcColor);

//         // Convert to 0-255 range for math
//         u32 r = srcRGB.r << 3;
//         u32 g = srcRGB.g << 3;
//         u32 b = srcRGB.b << 3;

//         // Compute luminance (scaled integer math)
//         // Y = 0.2126R + 0.7152G + 0.0722B
//         u32 luminance = (54 * r + 183 * g + 19 * b) >> 8;  // weights sum to 256

//         // Apply darkening
//         r = (r * darkFactor) >> 8;
//         g = (g * darkFactor) >> 8;
//         b = (b * darkFactor) >> 8;

//         // Add blue shift proportional to luminance
//         b += (luminance * blueFactor) >> 8;
//         if (b > 255) b = 255;

//         // Convert back to 5-bit per channel GBA color
//         *dst++ = RGB2(r >> 3, g >> 3, b >> 3);
//     }
// }

// static void ProcessSinglePalette(u16 *src, u16 *dst, struct BlendSettings *blend0, 
//                          struct BlendSettings *blend1, u16 weight0) 
// {
//     u32 i;

//     if (blend0->isTint)
//         Darken(src, dst);
//     else
//         for (i = 0; i < 16; i++) {
//             *dst++ = *src++;
//         }
// }




// void TimeMixPalettes(u32 palettes, u16 *src, u16 *dst, struct BlendSettings *blend0, 
//                     struct BlendSettings *blend1, u16 weight0) {

//     u32 i;

//     if (!palettes) 
//         return;
    
//     // Process each palette bit
//     do {
//         if (palettes & 1) {
//             ProcessSinglePalette(src, dst, blend0, blend1, weight0);
//         } else {
//             // Skip this palette - copy 16 colors unchanged
//             for (i = 0; i < 16; i++) {
//                 *dst++ = *src++;
//             }
//         }
//         palettes >>= 1;
//     } while (palettes);
// }