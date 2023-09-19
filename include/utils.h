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



#endif /* INCLUDE_UTILS_H_ */
