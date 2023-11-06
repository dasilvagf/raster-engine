/*
	===========================================================================
                          _________
                         /\        \	created by
                        /  \        \			(GABRIEL ->
                       /    \________\          y  z  <- EPILEF)
                       \    /        /          | /
                        \  /        /           |/___x
                         \/________/
     - RASTER ENGINE					
	===========================================================================
    
    Copyright (C) 2023  Gabriel F. S. da Silva

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

    // Extract the channels from the SIMD register and compress it 
    // into a 32-bit unsigned RGBA integer
    return (uint32_t)(0xFF << 24 | 
        _mm_cvtsi128_si32(_mm_srli_si128(int_rgba, 0)) << 16 | // Red Channel
        _mm_cvtsi128_si32(_mm_srli_si128(int_rgba, 4)) << 8  | // Green Channel
        _mm_cvtsi128_si32(_mm_srli_si128(int_rgba, 8)));       // Blue Chanell
}

static inline void draw_horizontal_line(uint32_t y, uint32_t x0, uint32_t x1, uint32_t argb_color, 
    uint32_t width, uint32_t height, uint32_t* color_buffer)
{
    assert(y < height && x0 < width && x1 < width && color_buffer);
    for (uint32_t i = 0u; i < (x1 - x0); ++i)
        color_buffer[y * width + (x0 + i)] = argb_color;
}

static inline void draw_vertical_line(uint32_t x, uint32_t y0, uint32_t y1, uint32_t argb_color, 
    uint32_t width, uint32_t height, uint32_t* color_buffer)
{
    assert(x < width && y0 < height && y1 < height && color_buffer);
    // DEBUG ONLY: Slow! we gonna miss some cache goodness on this one! aggahhh! :( 
    for (uint32_t i = 0u; i < (y1 - y0); ++i)
        color_buffer[((y0 + i) * width) + x] = argb_color;
}

#endif /* INCLUDE_UTILS_H_ */
