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

#ifndef _INCLUDE_RASTER_STAGE_H_
#define _INCLUDE_RASTER_STAGE_H_

#include "win32_buffer.h"
#include "math3D.h"

//
// Hybrid SoA/AoS based on Data Oriented Design and Intel recommendations
// towards performance on modern SIMD hardware
//
// 

static inline uint32_t IsPixelInsideTriangle(float e0, float e1, float e2, 
    float a0, float a1, float a2, 
    float b0, float b1, float b2)
{
    // Check if sample point is inside the triangle
    if (e0 > 0.0f && e1 > 0.0f && e2 > 0.0f)
        return 1u;
    // Otherwise, check whether or not it lies in one of the edges
    else if ((e0 == 0.0f && (a0 > 0.0f || b0 < 0.0f)) ||
             (e1 == 0.0f && (a1 > 0.0f || b1 < 0.0f)) ||
             (e2 == 0.0f && (a2 > 0.0f || b2 < 0.0f)))
        return 1u;

    return 0u;
}

void RasterTriangles(SurfaceBuffer* sb, Triangle* tb, uint32_t tb_size);

#endif
