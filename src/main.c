/*
===========================================================================
                          _________
                         /\        \	created by
                        /  \        \			(GABRIEL ->
                       /    \________\                <- EPILEF)
                       \    /        /
                        \  /        /
                         \/________/
     - RASTER ENGINE					main.c
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

#include <stdio.h>

#include "../include/win32_buffer.h"
#include "../include/raster_stage.h"
#include "../include/math3D.h"

#define WINDOW_WIDTH 1024u
#define WINDOW_HEIGHT 640u


int32_t WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
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
		// 0 - Input Assembler (IA)
		//

		//
		// 1 - Vertex Processing
		//

		// test quad
		Triangle t[2];

		// position
		Vec2 p0 = { 100.0f + dx, 100.0f + dy};
		Vec2 p1 = { 200.0f + dx, 100.0f + dy};
		Vec2 p2 = { 200.0f + dx, 200.0f + dy};
		Vec2 p3 = { 100.0f + dx, 200.0f + dy};

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
		// 2 - Rasterization
		//
		RasterTriangles(sb, t, 2u);


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
