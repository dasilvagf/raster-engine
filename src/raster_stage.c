/*
===========================================================================
                          _________
                         /\        \	created by
                        /  \        \			(GABRIEL ->
                       /    \________\                <- EPILEF)
                       \    /        /
                        \  /        /
                         \/________/
     - RASTER ENGINE					raster_stage.c
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

#include "../include/raster_stage.h"

void RasterTriangles(SurfaceBuffer* sb, Triangle* tb, uint32_t tb_size)
{
	// Clear Back Buffer
	for (uint32_t i = 0u; i < sb->height; ++i) 
		for (uint32_t j = 0u; j < sb->width; j += 4)
				_mm_storeu_si128((__m128i*)&sb->surface_buffer[i*sb->width + j], 
				_mm_setzero_si128());

	// For each of our triangles
	for (uint32_t t = 0u; t < tb_size; ++t){

		// triangle Area
		float tri_area = OrientedTriangleArea(tb[t].p0, tb[t].p1, tb[t].p2);
		
		// Back-Face culling (CCW is front)
		if (tri_area > 0.0f)
		{
			// Traverse inside Bounding Box
			uint32_t x_min = (uint32_t)roundf(tb[t].triBB.p0.x);
			uint32_t y_min = (uint32_t)roundf(tb[t].triBB.p0.y);
			uint32_t x_max = (uint32_t)roundf(tb[t].triBB.p1.x);
			uint32_t y_max = (uint32_t)roundf(tb[t].triBB.p1.y);

			//
			// Edge Functions (Constants)
			//

			// (P2 - P1)
			float a0 = tb[t].p1.y - tb[t].p2.y;
			float b0 = tb[t].p2.x - tb[t].p1.x;
			float c0 = tb[t].p1.x * (tb[t].p2.y - tb[t].p1.y) + tb[t].p1.y * -b0;

			// (P0 - P2)
			float a1 = tb[t].p2.y - tb[t].p0.y;
			float b1 = tb[t].p0.x - tb[t].p2.x;
			float c1 = tb[t].p2.x * (tb[t].p0.y - tb[t].p2.y) + tb[t].p2.y * -b1;

			// (P1 - P0)
			float a2 = tb[t].p0.y - tb[t].p1.y;
			float b2 = tb[t].p1.x - tb[t].p0.x;
			float c2 = tb[t].p0.x * (tb[t].p1.y - tb[t].p0.y) + tb[t].p0.y * -b2;

			// (The Scan-Line goes DOWN in Raster Coordinates)
			// Loop (4 each time) SSE SIMD lanes
			for (uint32_t i = y_max; i > y_min; --i) {
				for (uint32_t j = x_min; j < x_max; ++j) {

					// pixel center
					float px = j + 0.5f;
					float py = i - 0.5f;

					//
					// Edge Functions
					//

					// (P2 - P1)
					float e0 = a0 * px + b0 * py + c0;

					// (P0 - P2)
					float e1 = a1 * px + b1 * py + c1;

					// (P1 - P0)
					float e2 = a2 * px + b2 * py + c2;

					// Rasterize
					if (IsPixelInsideTriangle(e0, e1, e2, a0, a1, a2, b0, b1, b2))
					{
						// barycentric coordinates
						float l0 = e0/tri_area;
						float l1 = e1/tri_area;

						// interpolate the color
						float c_r = l0*(tb[t].c0.x - tb[t].c1.x) + l1*(tb[t].c1.x - tb[t].c0.x) + tb[t].c2.x;
						float c_g = l0*(tb[t].c0.y - tb[t].c1.y) + l1*(tb[t].c1.y - tb[t].c0.y) + tb[t].c2.y;
						float c_b = l0*(tb[t].c0.z - tb[t].c1.z) + l1*(tb[t].c1.z - tb[t].c0.z) + tb[t].c2.z;
						Vec3 c = {c_r, c_g, c_b};

						sb->surface_buffer[(sb->height - i) * sb->width + j] = rgb_float_to_uint32(c);
					}
				}
			}
		}
	}
}
