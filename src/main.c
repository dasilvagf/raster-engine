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
	printf("%s %s %s %s", gpl_license[0], gpl_license[1], gpl_license[2], gpl_license[3]);

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

		// Test Quad
		Vertex* vertex_buffer = NULL;
		Vertex* index_buffer = NULL;

		// test quad
		Triangle t[2];

		// position
		Vec2 p0 = { 100.0f + dx, 100.0f + dy};
		Vec2 p1 = { 500.0f + dx, 100.0f + dy};
		Vec2 p2 = { 500.0f + dx, 500.0f + dy};
		Vec2 p3 = { 100.0f + dx, 500.0f + dy};

		//color
		Vec3 r = { 1.0f, 0.0f, 0.0f };
		Vec3 g = { 0.0f, 1.0f, 0.0f };
		Vec3 b = { 0.0f, 0.0f, 1.0f };
		Vec3 w = { 1.0f, 1.0f, 1.0f };
		
		// triangle 1
		t[0].p0 = p0; t[0].p1 = p1; t[0].p2 = p2;
		t[0].c0 = r;  t[0].c1 = g;  t[0].c2 = b;
		GenerateBoundingBoxForTriangle(&t[0]);

		// triangle 2
		t[1].p0 = p0; t[1].p1 = p2; t[1].p2 = p3;
		t[1].c0 = r;  t[1].c1 = b;  t[1].c2 = w;
		GenerateBoundingBoxForTriangle(&t[1]);

		//
		// 1 - Input Assembler (IA)
		//

		// Generate Triangles in CCW order 
		Triangle* triangles_buffer = AssemblyTriangles(vertex_buffer, index_buffer, 
			ARRAYSIZE(vertex_buffer), ARRAYSIZE(index_buffer));

		//
		// 2 - Clipping, Rasterization & Depth Buffering
		//

		// Clip Triangles using the Cohen–Sutherland Algorithm
		
		// Rasterize Triangles using the Pinneda Algorithm
		RasterTriangles(sb, t, ARRAYSIZE(t));

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
