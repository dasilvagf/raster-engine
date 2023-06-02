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
		MSG msg;
		while(PeekMessage(&msg, sb->hwnd, 0u, 0u, PM_REMOVE))
		{
			switch(msg.message)
			{
			 case WM_QUIT: running = 0u; break;
			 case WM_KEYDOWN:
				 {
					if (msg.wParam == VK_LEFT)
						dx -= 10.777f;
					else if (msg.wParam == VK_RIGHT)
						dx += 10.777f;
					else if (msg.wParam == VK_UP)
						dy += 10.777f;
					else if (msg.wParam == VK_DOWN)
						dy -= 10.777f;
				 }
			 default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			 
			}
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
		ProcessVertices(&pipeline_desc, &input_vertices);

		// Generate Triangles in CCW order 
		Triangle* triangles_buffer = NULL;
		uint32_t tri_buffer_size = AssemblyTriangles(&input_vertices, &triangles_buffer);
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
	}

	CloseWin32(sb);
	return 0x0;
}
