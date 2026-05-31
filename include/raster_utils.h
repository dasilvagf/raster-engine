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
    
    Copyright (C) 2023-2026  Gabriel F. S. da Silva

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

#ifndef INCLUDE_RASTER_UTILS_H_
#define INCLUDE_RASTER_UTILS_H_

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "math3D.h"
#include "utils.h"

typedef struct StarPolygon_t
{
	Triangle* triangles;
	uint32_t n_triangles;
}StarPolygon;

static inline uint32_t rgb_float_to_uint32(Vec3 color_rgb)
{
    // ARGB
    uint8_t r = (uint8_t)(color_rgb.x * 0xFF);
    uint8_t g = (uint8_t)(color_rgb.y * 0xFF);
    uint8_t b = (uint8_t)(color_rgb.z * 0xFF);

    return (uint32_t)(0xFF << 24 | r << 16 | g << 8 | b);
}

// WARNING: This function will BREAK the flow of SIMD instructions
static inline uint32_t rgba_SIMD_float_to_uint32(__m128 color_rgba)
{
    // Convert from float [0,1] to uint8 [0, 255] (Compression)
    __m128 rgba_white = _mm_set_ps(0xFF, 0xFF, 0xFF, 0xFF);
    __m128i int_rgba  = _mm_cvtps_epi32(_mm_mul_ps(color_rgba, rgba_white));

    // Extract the channels from the SIMD register and compress it 
    // into a 32-bit unsigned RGBA integer
    return (uint32_t)(0xFF << 24 | 
        _mm_cvtsi128_si32(_mm_srli_si128(int_rgba, 0)) << 16 | // Red Channel
        _mm_cvtsi128_si32(_mm_srli_si128(int_rgba, 4)) << 8  | // Green Channel
        _mm_cvtsi128_si32(_mm_srli_si128(int_rgba, 8)));       // Blue Channel
}

static inline void draw_horizontal_line(uint32_t y, uint32_t x0, uint32_t x1, uint32_t argb_color, 
    uint32_t buffer_width, uint32_t buffer_height, uint32_t* color_buffer)
{ 
    assert(y < buffer_height && x0 < buffer_width && x1 < buffer_width && color_buffer);
    
    // the line can go in both directions: left and right
    x0 = min(x0, x1);
    x1 = max(x0, x1);

    for (uint32_t i = 0u; i < (x1 - x0); ++i)
        color_buffer[(buffer_height - y) * buffer_width + (x0 + i)] = argb_color;
}

static inline void draw_vertical_line(uint32_t x, uint32_t y0, uint32_t y1, uint32_t argb_color, 
    uint32_t buffer_width, uint32_t buffer_height, uint32_t* color_buffer)
{
    assert(x < buffer_width && y0 < buffer_height && y1 < buffer_height && color_buffer);

    // the line can go in both directions: up and down
    y0 = min(y0, y1);
    y1 = max(y0, y1);

    // DEBUG ONLY: Slow! we gonna miss some cache goodness on this one! aggahhh! :( 
    for (uint32_t i = 0u; i < (y1 - y0); ++i)
        color_buffer[((buffer_height - (y0 + i)) * buffer_width) + x] = argb_color;
}

static inline void draw_debug_line_dda(Vec2 p0, Vec2 p1, uint32_t argb_color, 
    uint32_t buffer_width, uint32_t buffer_height, uint32_t* color_buffer)
{
    const uint32_t x0 = (uint32_t)p0.x;
    const uint32_t y0 = (uint32_t)p0.y;
    const uint32_t x1 = (uint32_t)p1.x;
    const uint32_t y1 = (uint32_t)p1.y;

    assert(color_buffer);
    assert(x0 < buffer_width  && x1 < buffer_width);
    assert(y0 < buffer_height && y1 < buffer_height);

    const float dy = p1.y - p0.y;
    const float dx = p1.x - p0.x;
    const float slope = dy / dx;

    const uint32_t x_delta = (uint32_t)fabs(dx);
    const int32_t  x_incr  = (dx) > 0.0f ? 1 : -1;

    // check special cases
    const uint32_t is_edge_vertical = !isfinite(slope);
    const uint32_t is_edge_horizontal = (fabs(dy) < EPSILON_FLOAT);

    float x = p0.x;
    float y = p0.y;

    if (is_edge_vertical)
        draw_vertical_line((uint32_t)x, y0, y1, argb_color, buffer_width, buffer_height, color_buffer);
    else if (is_edge_horizontal)
        draw_horizontal_line((uint32_t)y, x0, x1, argb_color, buffer_width, buffer_height, color_buffer);
    else
    {
        /*
        for (uint32_t i = 0u; i < x_delta; ++i) {
            uint32_t xi = (uint32_t)x;
            uint32_t yi = (uint32_t)y;

            color_buffer[(buffer_height - yi) * buffer_width + xi] = argb_color;

            x += x_incr;
            y += slope;
        }
        */
    }
}

static inline void print_triangle_debug_statistics(const Triangle* tri, uint32_t tri_index, uint8_t is_clipped)
{
    static char debug_string[256];

    sprintf(debug_string, "=====================================================================\n");
    OutputDebugStringA(debug_string);

    if (!is_clipped)
        sprintf(debug_string, "DEBUG TRIANGLE %d\n", tri_index);
    else
        sprintf(debug_string, "DEBUG CLIPPED SUB-TRIANGLE %d\n", tri_index);

    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\tVertex 0:\n");
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\t\tpos(%f,%f)\n", tri->p0.x, tri->p0.y);
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\t\ttex(%f,%f)\n", tri->uv0.x, tri->uv0.y);
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\t\tcol(%f,%f,%f)\n", tri->c0.x, tri->c0.y, tri->c0.z);
    OutputDebugStringA(debug_string);


    sprintf(debug_string, "\tVertex 1:\n");
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\t\tpos(%f,%f)\n", tri->p1.x, tri->p1.y);
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\t\ttex(%f,%f)\n", tri->uv1.x, tri->uv1.y);
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\t\tcol(%f,%f,%f)\n", tri->c1.x, tri->c1.y, tri->c1.z);
    OutputDebugStringA(debug_string);


    sprintf(debug_string, "\tVertex 2:\n");
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\t\tpos(%f,%f)\n", tri->p2.x, tri->p2.y);
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\t\ttex(%f,%f)\n", tri->uv2.x, tri->uv2.y);
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\t\tcol(%f,%f,%f)\n", tri->c2.x, tri->c2.y, tri->c2.z);
    OutputDebugStringA(debug_string);

    //sprintf(debug_string, "\tArea(p0, p1, p2): %f\n", tri_area2 / 2.0f);
    //OutputDebugStringA(debug_string);

    sprintf(debug_string, "\tEdge 0: p0(%f,%f) -> p1(%f, %f)\n", tri->p0.x, tri->p0.y, tri->p1.x, tri->p1.y);
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\tEdge 1: p1(%f,%f) -> p2(%f, %f)\n", tri->p1.x, tri->p1.y, tri->p2.x, tri->p2.y);
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "\tEdge 2: p2(%f,%f) -> p0(%f, %f)\n", tri->p2.x, tri->p2.y, tri->p0.x, tri->p0.y);
    OutputDebugStringA(debug_string);

    sprintf(debug_string, "=====================================================================\n");
    OutputDebugStringA(debug_string);
}

#endif /* INCLUDE_RASTER_UTILS_H_ */
