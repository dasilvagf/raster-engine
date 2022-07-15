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
    // ARGB
    __m128 rgba_white = {0xFF, 0xFF, 0xFF, 0xff};
    __m128i int_rgba = _mm_cvtps_epi32(_mm_mul_ps(color_rgba, rgba_white));

    return (uint32_t)(0xFF << 24 | int_rgba.m128i_u32[0] << 16 | int_rgba.m128i_u32[1] << 8 | int_rgba.m128i_u32[2]);
}

StarPolygon* CreateZeldaTriForcePolygon(float side_length, Vec2 center);
StarPolygon* CreateStarPolygon(float side_length, Vec2 center);

#endif /* INCLUDE_UTILS_H_ */
