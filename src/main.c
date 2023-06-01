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
	// Pipeline Initialisation
	//

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

		// ADD A TEST QUAD
		
		// position
		Vec3 p0 = { 100.0f + dx, 100.0f + dy, 0.0f};
		Vec3 p1 = { 500.0f + dx, 100.0f + dy, 0.0f};
		Vec3 p2 = { 500.0f + dx, 500.0f + dy, 0.0f};
		Vec3 p3 = { 100.0f + dx, 500.0f + dy, 0.0f};

		// color
		Vec3 r = { 1.0f, 0.0f, 0.0f };
		Vec3 g = { 0.0f, 1.0f, 0.0f };
		Vec3 b = { 0.0f, 0.0f, 1.0f };
		Vec3 w = { 1.0f, 1.0f, 1.0f };
		
		Vertex vertex_buffer[4];
		vertex_buffer[0].position = p0;
		vertex_buffer[0].color = r;

		vertex_buffer[1].position = p1;
		vertex_buffer[1].color = g;


		vertex_buffer[2].position = p2;
		vertex_buffer[2].color = b;
	
		
		vertex_buffer[3].position = p3;
		vertex_buffer[3].color = w;

		uint32_t index_buffer[6] = {0u, 1u, 2u,			// triangle 1 
									0u, 2u, 3u };		// triangle 2

		//
		// 1 - Vertex Processing
		//

		// Transform Vertices (Local*World Transformations)

		// Transform to Homogenous Coordinates and Project to NDC

		// Clip Triangles using the Cohen–Sutherland Algorithm

		// Map to Viewport
		

		// Generate Triangles in CCW order 
		Triangle* triangles_buffer = NULL;
		uint32_t tri_buffer_size = AssemblyTriangles(vertex_buffer, index_buffer, 4, 6, &triangles_buffer);

		//
		// 2 Rasterization & Depth Buffering
		//

		// Rasterize Triangles using the Pinneda Algorithm
		RasterTriangles(sb, triangles_buffer, tri_buffer_size);

		//
		// 3 - Pixel Processing
		//

		//
		// 4 - Output/Merger (Presentation)
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
