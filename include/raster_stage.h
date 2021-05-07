/*
===========================================================================
                          _________
                         /\        \	created by
                        /  \        \			(GABRIEL ->
                       /    \________\                <- EPILEF)
                       \    /        /
                        \  /        /
                         \/________/
     - RASTER ENGINE					raster_stage.h
===========================================================================
The MIT License

Copyright (c) 2020 Gabriel Felipe. https://github.com/dasilvagf

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

#ifndef _INCLUDE_RASTER_STAGE_H_
#define _INCLUDE_RASTER_STAGE_H_

#include "win32_buffer.h"
#include "math3D.h"

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
