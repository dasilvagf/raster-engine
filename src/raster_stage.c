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

//
// act as the GPU vide0 memory
//
extern uint8_t* VERTEX_GPU_VRAM;
extern uint32_t mem_offset_ptr;

// enable clipping debugging functionality
#if (1 && _DEBUG)
#define DEBUG_CLIPPING_BOUNDS 
#endif

#ifdef DEBUG_CLIPPING_BOUNDS
#define CLIP_OFFSET 200u
#else
#define CLIP_OFFSET 0u
#endif

// maximum number of vertices allowed by a clipped triangle
#define MAX_VERTICES 6u

struct clip_rectangle_t
{
	uint32_t	
		min_x,
		min_y,
		max_x,
		max_y;
}clip_rectangle;

// where the actual magic happens
void __forceinline RasterTriangle(SurfaceBuffer* sb, Triangle* tb, float inv_tri_simd[4]);

// clip those triangles which witnessed too much - muhahaha (search for The Silence of the Clamps if you didn't got the joke!) 
Triangle* ClipTriangle(struct clip_rectangle_t* clip_rect, Triangle* in_tb, uint32_t shuterland_outcodes[3], uint32_t* n_out_triangles);

void RasterTriangles(SurfaceBuffer* sb, Triangle* tb, uint32_t tb_size)
{
	uint32_t width = sb->width;
	uint32_t height = sb->height;
	
	// clipping
	struct clip_rectangle_t clip_rect = { CLIP_OFFSET, CLIP_OFFSET, width - CLIP_OFFSET, height - CLIP_OFFSET };

	//uint32_t x_min = CLIP_OFFSET;
	//uint32_t x_max = width - CLIP_OFFSET;
	//uint32_t y_min = CLIP_OFFSET;
	//uint32_t y_max = height - CLIP_OFFSET;

	if (!g_depth_buffer)
		g_depth_buffer = (float*)malloc(sizeof(float) * width * height);

	// clear back buffer
	for (uint32_t i = 0u; i < height; ++i) 
		for (uint32_t j = 0u; j < width; j += 4)
				_mm_storeu_si128((__m128i*)&sb->surface_buffer[i*width + j], 
				_mm_setzero_si128());

	// for each of our triangles
	for (uint32_t t = 0u; t < tb_size; ++t){
		Triangle* tri = &tb[t];

		// triangle area (multipled by 2)
		float tri_area2 = OrientedArea(tri->p0, tri->p1, tri->p2);

		// test back-face culling (CCW is front)
		if (tri_area2 > 0.0f)
		{
			// for barycentric interpolation
			float inv_tri_simd[] = {1.0f/ tri_area2, 1.0f/ tri_area2, 0.0f, 0.0f};

			// use Cohem-Sutherland to check if we gonna need to clip
			uint32_t outcode_p0 = (((tri->p0.y > (float)clip_rect.max_y) << 3) | ((tri->p0.y < (float)clip_rect.min_y) << 2) | ((tri->p0.x > (float)clip_rect.max_x) << 1) | ((tri->p0.x < (float)clip_rect.min_x)));
			uint32_t outcode_p1 = (((tri->p1.y > (float)clip_rect.max_y) << 3) | ((tri->p1.y < (float)clip_rect.min_y) << 2) | ((tri->p1.x > (float)clip_rect.max_x) << 1) | ((tri->p1.x < (float)clip_rect.min_x)));
			uint32_t outcode_p2 = (((tri->p2.y > (float)clip_rect.max_y) << 3) | ((tri->p2.y < (float)clip_rect.min_y) << 2) | ((tri->p2.x > (float)clip_rect.max_x) << 1) | ((tri->p2.x < (float)clip_rect.min_x)));

			// check for trivial rejection (best cases)
			uint32_t is_inside = !((outcode_p0 | outcode_p1) || (outcode_p1 | outcode_p2) || (outcode_p2 | outcode_p0));
			uint32_t is_outside = ((outcode_p0 & outcode_p1) && (outcode_p1 & outcode_p2) && (outcode_p2 & outcode_p0));

			// no need to clip or generate new triangles, thus we just render the original triangle
			if (is_inside)
				RasterTriangle(sb, tri, inv_tri_simd);
			// if the triangle isn't outside we gonna need to clip it! :( [SLOW]
			else if (!is_outside)
			{
				// Clip and generate new triangles
				uint32_t n_clip_tri = 0u;
				uint32_t outcodes[] = { outcode_p0, outcode_p1, outcode_p2 };
				Triangle* clip_tri = ClipTriangle(&clip_rect, tri, outcodes, &n_clip_tri);

				for (uint32_t tc = 0u; tc < n_clip_tri; ++tc) {
					// triangle area (multipled by 2)
					tri_area2 = OrientedArea(clip_tri[tc].p0, clip_tri[tc].p1, clip_tri[tc].p2);
					assert(tri_area2 < 0.0f);
					inv_tri_simd[0] = inv_tri_simd[1] = 1.0f / tri_area2;

					// Raster the new triangles
					RasterTriangle(sb, &clip_tri[tc], inv_tri_simd);
				}

				free(clip_tri);
			}
		}
	}

	// Draw viewport only if we are debugging clipping functionality
#ifdef DEBUG_CLIPPING_BOUNDS
	draw_horizontal_line(CLIP_OFFSET, CLIP_OFFSET, width - CLIP_OFFSET, 0xFFFF0000, width, height, sb->surface_buffer);
	draw_horizontal_line(height - CLIP_OFFSET, CLIP_OFFSET, width - CLIP_OFFSET, 0xFFFF0000, width, height, sb->surface_buffer);
	draw_vertical_line(CLIP_OFFSET, CLIP_OFFSET, height - CLIP_OFFSET, 0xFFFF0000, width, height, sb->surface_buffer);
	draw_vertical_line(width - CLIP_OFFSET, CLIP_OFFSET, height - CLIP_OFFSET, 0xFFFF0000, width, height, sb->surface_buffer);
#endif
}

// no need to worry about code bloat for this project 
// maybe in the future if I port it to a low spec hardware this may change!
void __forceinline RasterTriangle(SurfaceBuffer* sb, Triangle* tb, float inv_tri_simd[4])
{
	// traverse inside bounding box
	uint32_t x_min = (uint32_t)roundf(tb->triBB.p0.x);
	uint32_t y_min = (uint32_t)roundf(tb->triBB.p0.y);
	uint32_t x_max = (uint32_t)roundf(tb->triBB.p1.x);
	uint32_t y_max = (uint32_t)roundf(tb->triBB.p1.y);

	//
	// edge functions (constants)
	//
	float a[4], b[4], c[4];

	// (P2 - P1)
	a[0] = tb->p1.y - tb->p2.y;
	b[0] = tb->p2.x - tb->p1.x;
	c[0] = tb->p1.x * (tb->p2.y - tb->p1.y) + tb->p1.y * -b[0];

	// (P0 - P2)
	a[1] = tb->p2.y - tb->p0.y;
	b[1] = tb->p0.x - tb->p2.x;
	c[1] = tb->p2.x * (tb->p0.y - tb->p2.y) + tb->p2.y * -b[1];

	// (P1 - P0)
	a[2] = tb->p0.y - tb->p1.y;
	b[2] = tb->p1.x - tb->p0.x;
	c[2] = tb->p0.x * (tb->p1.y - tb->p0.y) + tb->p0.y * -b[2];

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
	c_r[0] = tb->c0.x - tb->c2.x;
	c_r[1] = tb->c1.x - tb->c2.x;
	c_r[2] = tb->c2.x;
	c_r[3] = 0.0f;

	// green
	c_g[0] = tb->c0.y - tb->c2.y;
	c_g[1] = tb->c1.y - tb->c2.y;
	c_g[2] = tb->c2.y;
	c_g[3] = 0.0f;

	// blue
	c_b[0] = tb->c0.z - tb->c2.z;
	c_b[1] = tb->c1.z - tb->c2.z;
	c_b[2] = tb->c2.z;
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

Triangle* ClipTriangle(struct clip_rectangle_t* clip_rect, Triangle* in_tb, uint32_t shuterland_outcodes[3], uint32_t* n_out_triangles)
{
	*n_out_triangles = 0u;
	Triangle* tri_out = NULL;

	// clip rect aabb
	const float rect_x_min = (float)clip_rect->min_x;
	const float rect_x_max = (float)clip_rect->max_x;
	const float rect_y_min = (float)clip_rect->min_y;
	const float rect_y_max = (float)clip_rect->max_y;

	//
	// vertices bucket
	//
	Vec2 vertices_in[3];
	vertices_in[0] = in_tb->p0;
	vertices_in[1] = in_tb->p1;
	vertices_in[2] = in_tb->p2;

	Vec3 colors_in[3];
	colors_in[0] = in_tb->c0;
	colors_in[1] = in_tb->c1;
	colors_in[2] = in_tb->c2;

	uint32_t vertices_outcode[3];
	vertices_outcode[0] = shuterland_outcodes[0];
	vertices_outcode[1] = shuterland_outcodes[1];
	vertices_outcode[2] = shuterland_outcodes[2];

	Vec2 vertices_out[MAX_VERTICES];
	Vec3 colors_out[MAX_VERTICES];
	//
	// sutherland-hodgman
	//
	uint32_t n_vertices_out = 0u;
	uint32_t curr_vertex_out = 0u;

	// clip triangle edges
	for (uint32_t v = 0u; v < 3; ++v) {
	
		// edge position
		Vec2 p0 = vertices_in[v];
		Vec2 p1 = (v == 2u) ? vertices_in[0] /* last edge */ : vertices_in[v + 1];

		// edge colors
		Vec3 c0 = colors_in[v];
		Vec3 c1 = (v == 2u) ? colors_in[0] /* last edge */ : colors_in[v + 1];

		// edge linear factors
		const float dy = p1.y - p0.y;
		const float dx = p1.x - p0.x;
		const float slope = dy / dx;
		const float const_factor = p0.y - slope * p0.x;

		// check special cases
		const uint32_t is_edge_vertical = !isfinite(slope);
		const uint32_t is_edge_horizontal = (fabs(dy) < EPSILON_FLOAT);

		// clip aginst rectangle
		if (!is_edge_horizontal & !is_edge_vertical)
		{
			if (0x1 & vertices_outcode[v] /*left*/)
			{
				p0.x = rect_x_min;
				p0.y = slope * rect_x_min + const_factor;
			}
			if (0x2 & vertices_outcode[v] /*right*/)
			{
				p1.x = rect_x_max;
				p1.y = slope * rect_x_max + const_factor;
			}
			if (0x4 & vertices_outcode[v] /*bottom*/)
			{
				p0.x = (rect_y_min - const_factor) / slope;
				p0.y = rect_y_min;
			}
			if (0x8 & vertices_outcode[v] /*top*/)
			{
				p1.x = (rect_y_max - const_factor) / slope;
				p1.y = rect_y_max;
			}
		}

		// position
		vertices_out[n_vertices_out] = p0;
		vertices_out[n_vertices_out + 1u] = p1;

		// color
		colors_out[n_vertices_out] = c0;
		colors_out[n_vertices_out + 1u] = c1;

		// uv

		// depth

		n_vertices_out += 2;
	}

	//
	// triangulation
	//

	// any triangulation of a polygon with n vertices results in n - 2 triangles
	// https://sites.cs.ucsb.edu/~suri/cs235/Triangulation.pdf
	*n_out_triangles = n_vertices_out - 2;

	// fan triangulation algorithm (maybe improve this later?)
	Triangle* triangles_out = (Triangle*)calloc(*n_out_triangles, sizeof(Triangle));
	for (uint32_t t = 0u; t < *n_out_triangles; ++t) {
		
		// screen-space position
		triangles_out[t].p0 = vertices_out[0u];
		triangles_out[t].p1 = vertices_out[t + 1u];
		triangles_out[t].p2 = vertices_out[t + 2u];

		// color
		triangles_out[t].c0 = colors_out[0u];
		triangles_out[t].c1 = colors_out[t + 1u];
		triangles_out[t].c2 = colors_out[t + 2u];

		// uv coordinates

		// depth
	}


	//				Algorithm Overview
	// 0 - create an output bucket of Vertices
	// 1 - for each edge of the clipping rectangle
	//		1.1 - clip against each edge of current polygon (it may actually not be a triangle at this point)
	//		1.2 - add the output vertice in case it is in and continue with it as v0, in a reetrant form
	// 2 - run a triangulization algorithm on the vertices in the output buccket
	//		2.1 - for now, lets go with the fan algorithm
	// 3 - return the new triangles

	return triangles_out;
}
