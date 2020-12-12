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

	//
	// Pipeline Initialisation
	//

	while(running)
	{
		//
		// Process OS Stuff
		//
		MSG msg;
		while(PeekMessage(&msg, sb->hwnd, 0u, 0u, PM_REMOVE))
		{
			switch(msg.message)
			{
			case WM_QUIT: running = 0u; break;
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

		// Triangles Buffer
		uint32_t n_triangles = 2u;
		Triangle* tb = (Triangle*) malloc(sizeof(Triangle)*n_triangles);

		// test triangle
		tb[0].p0.x = 10.0f;  tb[0].p0.y = 10.0f;
		tb[0].p1.x = 100.0f;  tb[0].p1.y = 10.0f;
		tb[0].p2.x = 70.0f;  tb[0].p2.y = 100.0f;

		GenerateBoundingBoxForTriangle(&tb[0]);

		tb[1].p0.x = 100.0f; tb[1].p0.y = 10.0f;
		tb[1].p1.x = 200.0f;  tb[1].p1.y = 60.0f;
		tb[1].p2.x = 70.0f;  tb[1].p2.y = 100.0f;

		GenerateBoundingBoxForTriangle(&tb[1]);

		//
		// 2 - Rasterization
		//
		RasterTriangles(sb, tb, n_triangles);


		//
		// 3 - Pixel Processing
		//

		//
		// 4 - Output/Merger (Presentation)
		//
		InvalidateRect(sb->hwnd, NULL, 1u);
		UpdateWindow(sb->hwnd);
	}

	CloseWin32(sb);
	return 0x0;
}
