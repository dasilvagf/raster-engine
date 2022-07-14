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
	// clear back buffer
	for (uint32_t i = 0u; i < sb->height; ++i) 
		for (uint32_t j = 0u; j < sb->width; j += 4)
				_mm_storeu_si128((__m128i*)&sb->surface_buffer[i*sb->width + j], 
				_mm_setzero_si128());

	// for each of our triangles
	for (uint32_t t = 0u; t < tb_size; ++t){

		// triangle area (multipled by 2)
		float tri_area2 = OrientedArea(tb[t].p0, tb[t].p1, tb[t].p2);
		float inv_tri_simd[] = {1.0f/ tri_area2, 1.0f/ tri_area2, 0.0f, 0.0f};
		
		// back-face culling (CCW is front)
		if (tri_area2 > 0.0f)
		{
			// traverse inside bounding box
			uint32_t x_min = (uint32_t)roundf(tb[t].triBB.p0.x);
			uint32_t y_min = (uint32_t)roundf(tb[t].triBB.p0.y);
			uint32_t x_max = (uint32_t)roundf(tb[t].triBB.p1.x);
			uint32_t y_max = (uint32_t)roundf(tb[t].triBB.p1.y);

			//
			// edge functions (constants)
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

			//
			// edge functions (initial value)
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

			//
			// color interpolation (constants)
			//	
			float c_r[4], c_g[4], c_b[4];
			
			// red
			c_r[0] = tb[t].c0.x - tb[t].c2.x;
			c_r[1] = tb[t].c1.x - tb[t].c2.x;
			c_r[2] = tb[t].c2.x;
			c_r[3] = 0.0f;

			// green
			c_g[0] = tb[t].c0.y - tb[t].c2.y;
			c_g[1] = tb[t].c1.y - tb[t].c2.y;
			c_g[2] = tb[t].c2.y;
			c_g[3] = 0.0f;

			// blue
			c_b[0] = tb[t].c0.z - tb[t].c2.z;
			c_b[1] = tb[t].c1.z - tb[t].c2.z;
			c_b[2] = tb[t].c2.z;
			c_b[3] = 0.0f;

			//
			// load to SIMD registers
			//
			
			// rasterization
			__m128 const_a = _mm_load_ps(a);
			__m128 const_b = _mm_load_ps(b);
			__m128 column_e = _mm_load_ps(e);
			__m128 curr_e = _mm_load_ps(curr_edge);

			// interpolation
			__m128 inv_tri = _mm_load_ps(inv_tri_simd);
			__m128 red = _mm_load_ps(c_r);
			__m128 green = _mm_load_ps(c_g);
			__m128 blue = _mm_load_ps(c_b);
			
			// rasterizer inside the bounding-box
			for (uint32_t i = y_max; i > y_min; i--) {
				for (uint32_t j = x_min; j < x_max; j++) {
					//
					// check if the pixels are inside the triangle
					//

					// curr_e > float4(0.0f) for all components
					__m128 mask_inside = _mm_cmpgt_ps(curr_e, _mm_setzero_ps());
					
					//
					// check if the pixels are in the edge 
					//

					// const_a > float4(0.0f)
					__m128 mask_edge_a = _mm_cmpgt_ps(const_a, _mm_setzero_ps());

					// const_b < float4(0.0f)
					__m128 mask_edge_b = _mm_cmplt_ps(const_b, _mm_setzero_ps());

					// curr_e == float4(0.0f)
					__m128 mask_edge_e = _mm_cmpeq_ps(curr_e, _mm_setzero_ps());

					// curr_e == float4(0.0f) && ( curr_a > float4(0.0f) || curr_b < float4(0.0f) 
					__m128 mask_edge = _mm_and_ps(mask_edge_e, _mm_or_ps(mask_edge_a, mask_edge_b));

					//
					// calculate barycentric coordinates
					//


					/// HERE IS WHERE I'M SEE HOW I'M GONNA MULTIPLY THIS WITHOUT SPENDING MUCH
					/// TIME AND KEEP Z being 1
					// divide edges by 2 times the area of the triangle 
					// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=set&ig_expand=6234,4085&techs=SSE4_1
					__m128 l = _mm_insert_ps(_mm_mul_ps(curr_e, inv_tri), SIMD_128_Z_ONE, );
					

					//float l0 = curr_edge[0] / (tri_area2);
					//float l1 = curr_edge[1] / (tri_area2);
					
					//
					// interpolate the color (Dot Product)
					//

					//float c_r = l0 * (tb[t].c0.x - tb[t].c2.x) + l1 * (tb[t].c1.x - tb[t].c2.x) + tb[t].c2.x;
					//float c_g = l0 * (tb[t].c0.y - tb[t].c2.y) + l1 * (tb[t].c1.y - tb[t].c2.y) + tb[t].c2.y;
					//float c_b = l0 * (tb[t].c0.z - tb[t].c2.z) + l1 * (tb[t].c1.z - tb[t].c2.z) + tb[t].c2.z;

					float c_r = l.m128_f32[0] * (tb[t].c0.x - tb[t].c2.x) + l.m128_f32[1] * (tb[t].c1.x - tb[t].c2.x) + tb[t].c2.x;
					float c_g = l.m128_f32[0] * (tb[t].c0.y - tb[t].c2.y) + l.m128_f32[1] * (tb[t].c1.y - tb[t].c2.y) + tb[t].c2.y;
					float c_b = l.m128_f32[0] * (tb[t].c0.z - tb[t].c2.z) + l.m128_f32[1] * (tb[t].c1.z - tb[t].c2.z) + tb[t].c2.z;
					
					// multiplication part (ignore w component)
					__m128 c_red = _mm_mul_ps(l, red);
					__m128 c_green = _mm_mul_ps(l, green);
					__m128 c_blue = _mm_mul_ps(l, blue);

					float b_1 = l.m128_f32[0] * (tb[t].c0.z - tb[t].c2.z);
					float b_2 = l.m128_f32[1] * (tb[t].c1.z - tb[t].c2.z);
					float b_3 = tb[t].c2.z;

					// addition part
					c_red = _mm_hadd_ps(c_red, c_green);
					c_blue = _mm_hadd_ps(c_blue, _mm_setzero_ps());

					__m128 rgba_color = _mm_hadd_ps(c_red, c_blue);

					// step edge functions in +x
					//curr_edge[0] += a[0];
					//curr_edge[1] += a[1];
					//curr_edge[2] += a[2];

					curr_e = _mm_add_ps(curr_e, const_a);
					//curr_e = _mm_load_ps(curr_edge);

					Vec3 c = { rgba_color.m128_f32[0], rgba_color.m128_f32[1],  rgba_color.m128_f32[2]};
					//Vec3 c = { c_r, c_g, c_b };

					//
					// check if the pixels passed in one of the two tests (edge or insde)
					//
					int32_t edge_mask = _mm_movemask_ps(mask_inside);
					int32_t rasterize = (edge_mask & 0x1 && edge_mask & 0x2 && edge_mask & 0x4) || _mm_movemask_ps(mask_edge);

					//
					// rasterize pixel, IF and ONLY IF, it passed in the test
					//
					sb->surface_buffer[rasterize * (sb->height - i) * sb->width + j] = rasterize * rgb_float_to_uint32(c);
				}

				// step edge functions in -y
				
				
				//e[0] -= b[0];
				//e[1] -= b[1];
				//e[2] -= b[2];
				//curr_e = _mm_load_ps(e);
				

				column_e = _mm_sub_ps(column_e, const_b);
				curr_e = column_e;

				_mm_store_ps((__m128*)curr_edge, curr_e);
			}
			
			/*
			// NON SIMD Version
			for (uint32_t i = y_max; i > y_min; --i) {
				
				// start at the left of the bounding-box
				float curr_e0 = e[0];
				float curr_e1 = e[1];
				float curr_e2 = e[2];

				for (uint32_t j = x_min; j < x_max; ++j) {

					// rasterize
					if (IsPixelInsideTriangle(curr_e0, curr_e1, curr_e2, a[0], a[1], a[2], b[0], b[1], b[2]))
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
					curr_e0 += a[0];
					curr_e1 += a[1];
					curr_e2 += a[2];
				}

				// step edge functions in -y
				e[0] -= b[0];
				e[1] -= b[1];
				e[2] -= b[2];
			}
			*/
		
			
		}
	}
}
