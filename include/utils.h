/*
===========================================================================
                          _________
                         /\        \	created by
                        /  \        \			(GABRIEL ->
                       /    \________\                <- EPILEF)
                       \    /        /
                        \  /        /
                         \/________/
     - RASTER ENGINE					utils.h
===========================================================================
The MIT License

Copyright (c) 2019-2020 Gabriel Felipe. https://github.com/dasilvagf

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "math3D.h"

typedef struct StarPolygon_t
{
	Triangle* triangles;
	uint32_t n_triangles;
}StarPolygon;

static inline uint32_t rgb_float_to_uint32(Vec3 color_rgb)
{
    // ARGB
    uint8_t r = (uint8_t)(color_rgb.x*0xFF);
    uint8_t g = (uint8_t)(color_rgb.y*0xFF);
    uint8_t b = (uint8_t)(color_rgb.z*0xFF);

    return (uint32_t)(0xFF << 24 | r << 16 | g << 8 | b);
}

// WARNING: This function will BREAK the flow of SIMD instructions
static inline uint32_t rgba_SIMD_float_to_uint32(__m128 color_rgba)
{
    // Convert from float [0,1] to uint8 [0, 255] (Compression)
    __m128 rgba_white = _mm_set_ps(0xFF, 0xFF, 0xFF, 0xFF);
    __m128i int_rgba = _mm_cvtps_epi32(_mm_mul_ps(color_rgba, rgba_white));

    // Pack each color to a byte in the first SIMD register (lest signficative)
    __m128i R = _mm_slli_si128(int_rgba, 12 + 2);
    __m128i G = _mm_slli_si128(int_rgba, 12 + 0);
    __m128i B = _mm_slli_si128(int_rgba, 12);

    // Pack ARGB together in the first SIMD register (lest signficative)
    __m128i final_color = _mm_setr_epi32(0x0, 0x0, 0x0, 0xFF000000);
    final_color = _mm_or_si128(final_color, R);
    final_color = _mm_or_si128(final_color, G);
    final_color = _mm_or_si128(final_color, B);

    // Return first SIMD register (most signficative)
    uint32_t final_color_scalar = (uint32_t) _mm_cvtsi128_si32(_mm_srli_si128(final_color, 12));
    //return final_color_scalar;
    uint32_t final_color_scalar2 = (uint32_t)(0xFF << 24 | int_rgba.m128i_u32[0] << 16 | int_rgba.m128i_u32[1] << 8 | int_rgba.m128i_u32[2]);
    return final_color_scalar2;

    //return (uint32_t)(0xFF << 24 | int_rgba.m128i_u32[0] << 16 | int_rgba.m128i_u32[1] << 8 | int_rgba.m128i_u32[2]);
}

StarPolygon* CreateZeldaTriForcePolygon(float side_length, Vec2 center);
StarPolygon* CreateStarPolygon(float side_length, Vec2 center);

#endif /* INCLUDE_UTILS_H_ */
