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

		// triangle area (multipled by 2)
		float tri_area2 = OrientedArea(tb[t].p0, tb[t].p1, tb[t].p2);
		
		// Back-Face culling (CCW is front)
		if (tri_area2 > 0.0f)
		{
			// Traverse inside Bounding Box
			uint32_t x_min = (uint32_t)roundf(tb[t].triBB.p0.x);
			uint32_t y_min = (uint32_t)roundf(tb[t].triBB.p0.y);
			uint32_t x_max = (uint32_t)roundf(tb[t].triBB.p1.x);
			uint32_t y_max = (uint32_t)roundf(tb[t].triBB.p1.y);

			//
			// Edge Functions (Constants)
			//
			float a[4], b[4], c[4];

			// (P2 - P1)
			a[0] = tb[t].p1.y - tb[t].p2.y;
			b[0] = tb[t].p2.x - tb[t].p1.x;
			c[0] = tb[t].p1.x * (tb[t].p2.y - tb[t].p1.y) + tb[t].p1.y * -b[0];

			// (P0 - P2)
			a[1] = tb[t].p2.y - tb[t].p0.y;
			b[1] = tb[t].p0.x - tb[t].p2.x;
			c[1] = tb[t].p2.x * (tb[t].p0.y - tb[t].p2.y) + tb[t].p2.y * -b[1];

			// (P1 - P0)
			a[2] = tb[t].p0.y - tb[t].p1.y;
			b[2] = tb[t].p1.x - tb[t].p0.x;
			c[2] = tb[t].p0.x * (tb[t].p1.y - tb[t].p0.y) + tb[t].p0.y * -b[2];

			a[3] = 0.0f;
			b[3] = 0.0f;
			c[3] = 0.0f;

			__m128 const_a = _mm_load_ps(a);
			__m128 const_b = _mm_load_ps(b);

			//
			// Edge Functions (Initial Value)
			//
			float e[4], curr_edge[4];

			// (P2 - P1)
			e[0] = a[0]*(x_min + 0.5f) + b[0]*(y_max - 0.5f) + c[0];
			curr_edge[0] = e[0];

			// (P0 - P2)
			e[1] = a[1]*(x_min + 0.5f) + b[1]*(y_max - 0.5f) + c[1];
			curr_edge[1] = e[1];

			// (P1 - P0)
			e[2] = a[2]*(x_min + 0.5f) + b[2]*(y_max - 0.5f) + c[2];
			curr_edge[2] = e[2];

			e[3] = 0.0f;
			curr_edge[3] = e[3];

			__m128 curr_e = _mm_load_ps(e);
			
			//
			// current performance without SIMD on DEBUG: 5ms (UPPER BOUND)
			//

			// rasterizer inside the bounding-box
			for (uint32_t i = y_max; i > y_min; i--) {
				for (uint32_t j = x_min; j < x_max; j++) {
					
					//
					// check if the pixels are inside/edge the triangle
					//

					// curr_e > float4(0.0f)
					__m128 mask_inside = _mm_cmpgt_ps(curr_e, _mm_setzero_ps());

					// const_a > float4(0.0f)
					__m128 mask_edge_a = _mm_cmpgt_ps(const_a, _mm_setzero_ps());

					// const_b < float4(0.0f)
					__m128 mask_edge_b = _mm_cmplt_ps(const_b, _mm_setzero_ps());

					// curr_e == float4(0.0f)
					__m128 mask_edge_e = _mm_cmpeq_ps(curr_e, _mm_setzero_ps());

					// curr_e > float4(0.0f) && ( curr_a > float4(0.0f) || curr_b < float4(0.0f) 
					__m128 mask_edge = _mm_and_ps(mask_edge_e, _mm_or_ps(mask_edge_a, mask_edge_b));

					// final decision value
					mask_inside = _mm_and_ps(mask_inside, mask_inside);
					__m128 rasterize = _mm_or_ps(mask_inside, mask_edge);
					
					//
					// rasterize pixel in case IF and ONLY IF it passed in the test
					//

					// GREATE DOC TO KEEP STUDYING (PAGE 14)
					//http://www.cs.uu.nl/docs/vakken/magr/2017-2018/files/SIMD%20Tutorial.pdf


					// barycentric coordinates
					float lt[4] = { curr_edge[0] / (tri_area2), curr_edge[1] / (tri_area2), 1.0f, 0.0f };
					__m128 l = _mm_load_ps(lt);
				
					float l0 = curr_edge[0] / (tri_area2);
					float l1 = curr_edge[1] / (tri_area2);

					// interpolate the color
				
					
					float c_r = l0 * (tb[t].c0.x - tb[t].c2.x) + l1 * (tb[t].c1.x - tb[t].c2.x) + tb[t].c2.x;
					float c_g = l0 * (tb[t].c0.y - tb[t].c2.y) + l1 * (tb[t].c1.y - tb[t].c2.y) + tb[t].c2.y;
					float c_b = l0 * (tb[t].c0.z - tb[t].c2.z) + l1 * (tb[t].c1.z - tb[t].c2.z) + tb[t].c2.z;
					Vec3 c = { c_r, c_g, c_b };

					uint32_t address_mask = (rasterize.m128_u32[0] || rasterize.m128_u32[1] || rasterize.m128_u32[2]);
					sb->surface_buffer[address_mask*(sb->height - i) * sb->width + j] = rgb_float_to_uint32(c);

					// step edge functions in +x
					curr_edge[0] += a[0];
					curr_edge[1] += a[1];
					curr_edge[2] += a[2];

					curr_e = _mm_load_ps(curr_edge);
				}

				// step edge functions in -y
				e[0] -= b[0];
				e[1] -= b[1];
				e[2] -= b[2];

				curr_e = _mm_load_ps(e);
				_mm_store_ps((__m128*)curr_edge, curr_e);
			}





			/*
			// loop (4 each time) SSE SIMD lanes
			for (uint32_t i = y_max; i > y_min; --i) {
				
				// start at the left of the bounding-box
				float curr_e0 = e0;
				float curr_e1 = e1;
				float curr_e2 = e2;

				for (uint32_t j = x_min; j < x_max; ++j) {

					// rasterize
					if (IsPixelInsideTriangle(curr_e0, curr_e1, curr_e2, a0, a1, a2, b0, b1, b2))
					{
						// barycentric coordinates
						float l0 = curr_e0 / (tri_area2);
						float l1 = curr_e1 / (tri_area2);

						// interpolate the color
						float c_r = l0 * (tb[t].c0.x - tb[t].c2.x) + l1 * (tb[t].c1.x - tb[t].c2.x) + tb[t].c2.x;
						float c_g = l0 * (tb[t].c0.y - tb[t].c2.y) + l1 * (tb[t].c1.y - tb[t].c2.y) + tb[t].c2.y;
						float c_b = l0 * (tb[t].c0.z - tb[t].c2.z) + l1 * (tb[t].c1.z - tb[t].c2.z) + tb[t].c2.z;
						Vec3 c = { c_r, c_g, c_b };

						sb->surface_buffer[(sb->height - i) * sb->width + j] = rgb_float_to_uint32(c);
					}

					// step edge functions in +x
					curr_e0 += a0;
					curr_e1 += a1;
					curr_e2 += a2;
				}

				// step edge functions in -y
				e0 -= b0;
				e1 -= b1;
				e2 -= b2;
			}
			*/
		}
	}
}
