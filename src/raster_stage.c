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

#include "../include/raster_stage.h"
static float* g_depth_buffer = NULL;

void RasterTriangles(SurfaceBuffer* sb, Triangle* tb, uint32_t tb_size)
{
	if (!g_depth_buffer)
		g_depth_buffer = (float*)malloc(sizeof(float) * sb->width * sb->height);

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

					// divide edges by 2 times the area of the triangle and set z to 1
					__m128 l = _mm_insert_ps(_mm_mul_ps(curr_e, inv_tri), SIMD_128_Z_ONE, 0xA0);

					//
					// interpolate the color (Dot Product)
					//

					// multiplication part (ignore w component)
					__m128 c_red = _mm_mul_ps(l, red);
					__m128 c_green = _mm_mul_ps(l, green);
					__m128 c_blue = _mm_mul_ps(l, blue);

					// addition part
					c_red = _mm_hadd_ps(c_red, c_green);
					c_blue = _mm_hadd_ps(c_blue, _mm_setzero_ps());

					// final interpolated color
					__m128 rgba_color = _mm_hadd_ps(c_red, c_blue);

					//
					// step edge functions in +x
					//
					curr_e = _mm_add_ps(curr_e, const_a);

					//
					// check if the pixels passed in one of the two tests (edge or insde)
					//
					int32_t edge_mask = _mm_movemask_ps(mask_inside);
					int32_t rasterize = (edge_mask & 0x1 && edge_mask & 0x2 && edge_mask & 0x4) || _mm_movemask_ps(mask_edge);

					//
					// rasterize pixel, IF and ONLY IF, it passed in the test
					//
					sb->surface_buffer[rasterize * (sb->height - i) * sb->width + j] = rasterize * rgba_SIMD_float_to_uint32(rgba_color);
				}

				// step edge functions in -y
				column_e = _mm_sub_ps(column_e, const_b);
				curr_e = column_e;
			}
		}
	}

	// free triangles buffer
	free(tb);
}
