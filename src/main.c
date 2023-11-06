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

#include <stdio.h>

#include "../include/win32_buffer.h"
#include "../include/vertex_stage.h"
#include "../include/raster_stage.h"
#include "../include/math3D.h"
#include "../include/vram.h"

// GPL message
static const char* gpl_license[] =
{
	"raster_engine - Copyright (C) 2023  Gabriel F. S. da Silva\n",
	"This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.\n",
	"This is free software, and you are welcome to redistribute it\n"
	"under certain conditions; type `show c' for details.\n\n"
};

#define WINDOW_WIDTH 1024u
#define WINDOW_HEIGHT 640u

//
// Act as the GPU vide0 memory
//
uint8_t* VERTEX_GPU_VRAM;
uint32_t mem_offset_ptr = 0u;

void LoadVerticesDataFromDisk(const char* filename, vertex_data** out_vertex_data);

int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		PWSTR pCmdLine, int32_t nCmdShow)
{
	//
	// System Initialisation
	//
	SurfaceBuffer* sb = InitWin32(WINDOW_WIDTH, WINDOW_HEIGHT, hInstance);
	uint32_t running = 1u;

	LARGE_INTEGER cpu_frequency; QueryPerformanceFrequency(&cpu_frequency);
	float second_tick = 1.0f/cpu_frequency.QuadPart;

	LARGE_INTEGER t0, t1;
	QueryPerformanceFrequency(&t1);

	float dx = 0.0f;
	float dy = 0.0f;
	float delta = 0.01f;

	// Alloc VRAM space
	VERTEX_GPU_VRAM = (uint8_t*)malloc(VRAM_MAX_SIZE);
	assert(VERTEX_GPU_VRAM);

	// GPL message
	//printf("%s %s %s %s", gpl_license[0], gpl_license[1], gpl_license[2], gpl_license[3]);

	//
	// Data Pre-processing
	//
	vertex_data* input_vertices = NULL;
	LoadVerticesDataFromDisk("dummy.obj", &input_vertices);
	assert(input_vertices);

	while(running)
	{
		t0 = t1;

		//
		// Process OS Stuff
		//
		dx = 0.0f; dy = 0.0f; 
		MSG msg;
		while(PeekMessage(&msg, sb->hwnd, 0u, 0u, PM_REMOVE))
		{
			switch(msg.message)
			{
			 case WM_QUIT: running = 0u; break;
			 case WM_KEYDOWN:
				 {
					 if (msg.wParam == VK_LEFT)
						 dx = -delta;
					 else if (msg.wParam == VK_RIGHT)
						 dx = delta;
					 else if (msg.wParam == VK_UP)
						 dy = delta;
					 else if (msg.wParam == VK_DOWN)
						 dy = -delta;
				 }
			 default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			 
			}
		}

		// update vertices position
		for (int i = 0u; i < input_vertices->vb_size; i++) {
			input_vertices->vb[i].position.x += dx;
			input_vertices->vb[i].position.y += dy;
		}

		// ----------------------------------------------
		// Rendering Pipeline (Direct3D9 Level)
		// ----------------------------------------------

		//
		// 0 - Vertex Processing
		//
		vertex_pipeline_desc pipeline_desc;

		// viewport mapping
		viewport viewp = { WINDOW_WIDTH, WINDOW_HEIGHT, 0u, 0u };
		pipeline_desc.viewp = viewp;

		// Fixed-Pipeline vertex stage
		vertex_data* output_vertices = NULL;
		ProcessVertices(&pipeline_desc, input_vertices, &output_vertices);

		// Generate Triangles in CCW order 
		Triangle* triangles_buffer = NULL;
		uint32_t tri_buffer_size = AssemblyTriangles(output_vertices, &triangles_buffer);
		assert(triangles_buffer);

		//
		// 1 Rasterization & Depth Buffering
		//

		// Rasterize Triangles using the Pinneda Algorithm
		RasterTriangles(sb, triangles_buffer, tri_buffer_size);

		//
		// 2 - Pixel Processing
		//

		//
		// 3 - Output/Merger (Presentation)
		//

		// Compute Time to Compute Frame (ms)
		QueryPerformanceCounter(&t1);
		float elapsed_ms = 1000.0f * second_tick * (t1.QuadPart - t0.QuadPart);
		
		char* debug_string[256];
		sprintf(debug_string, "RASTER ENGINE | frame time %f (ms)", elapsed_ms);
		SetWindowText(sb->hwnd, debug_string);

		// Swap Buffers
		InvalidateRect(sb->hwnd, NULL, 1u);
		UpdateWindow(sb->hwnd);

		// Clear VRAM
		mem_offset_ptr = CLEAR_VRAM_OFFSET;
	}

	//
	// Free Resourcers
	//
	CloseWin32(sb); free(VERTEX_GPU_VRAM);
	return 0x0;
}

// This will be changed later to something decent
void LoadVerticesDataFromDisk(const char* filename, vertex_data** out_vertex_data)
{
    //
    // Parse file
    //
    uint32_t vertices_count = 4u;
    uint32_t indices_count = 6u;

    //
    // Init mem
    //
    Vertex* vertex_buffer = (Vertex*)malloc(sizeof(Vertex) * vertices_count);
    uint32_t* index_buffer = (uint32_t*)malloc(sizeof(uint32_t) * indices_count);

    //
    // Populate with data
    //

	// ADD A TEST QUAD
		
	// position
	Vec4 p0 = { -0.5f, -0.5f, 0.0f, 1.0f};
	Vec4 p1 = {  0.5f, -0.5f, 0.0f, 1.0f};
	Vec4 p2 = {  0.5f,  0.5f, 0.0f, 1.0f};
	Vec4 p3 = { -0.5f,  0.5f, 0.0f, 1.0f};

	// color
	Vec3 r = { 1.0f, 0.0f, 0.0f };
	Vec3 g = { 0.0f, 1.0f, 0.0f };
	Vec3 b = { 0.0f, 0.0f, 1.0f };
	Vec3 w = { 1.0f, 1.0f, 1.0f };
	
	vertex_buffer[0].h_position = p0;
	vertex_buffer[0].color = r;

	vertex_buffer[1].h_position = p1;
	vertex_buffer[1].color = g;

	vertex_buffer[2].h_position = p2;
	vertex_buffer[2].color = b;
	
	vertex_buffer[3].h_position = p3;
	vertex_buffer[3].color = w;

    index_buffer[0] = 0u;
	index_buffer[1] = 1u;
	index_buffer[2] = 2u;
    
    index_buffer[3] = 0u;
	index_buffer[4] = 2u;
	index_buffer[5] = 3u;

	*out_vertex_data = (vertex_data*)malloc(sizeof(vertex_data));
	(*out_vertex_data)->ib_size = indices_count;
	(*out_vertex_data)->vb_size = vertices_count;
	(*out_vertex_data)->ib = index_buffer;
	(*out_vertex_data)->vb = vertex_buffer;
}
