/*
===========================================================================
                          _________
                         /\        \	created by
                        /  \        \			(GABRIEL ->
                       /    \________\                <- EPILEF)
                       \    /        /
                        \  /        /
                         \/________/
     - RASTER ENGINE					win32_buffer.c
===========================================================================
The MIT License

Copyright (c) 2019-2020 Gabriel Felipe. https://github.com/dasilvagf

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

//
//			Window Initialisation & SetUp
//
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	    {
			case WM_DESTROY: PostQuitMessage(0); break;
			case WM_KEYDOWN:
			{
				if (wParam == VK_ESCAPE)
					PostQuitMessage(0);
			}break;
			/*
			case WM_PAINT:
			{

				StretchDIBits(pbuffer->hdc, pbuffer->width, pbuffer->height, 0, 0,
							pbuffer->width, pbuffer->height, 0, 0, (void*)pbuffer->pixels,
							&pbuffer->bminfo, DIB_RGB_COLORS, SRCCOPY);

			}break;
			*/
	        default:
	            return DefWindowProc(hwnd, uMsg, wParam, lParam);
	    }
	return 0;
}

SurfaceBuffer* InitWin32(uint32_t width, uint32_t height, HINSTANCE hinstance)
{
	// Basic Window Element Class Registration
	WNDCLASSEXA wndc = {};
	wndc.cbSize = sizeof(WNDCLASSEXA);
	wndc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndc.lpfnWndProc = WindowProc;
	wndc.hInstance = hinstance;
	wndc.lpszClassName = "raster-engine";
	assert(RegisterClassExA(&wndc));

	// Double Buffering
	// NOTE: width x height is a little bit bigger than the actual width x height
	uint32_t* d_buffering[2];
	d_buffering[0] = (uint32_t*) malloc(width*height*0x4);
	d_buffering[1] = (uint32_t*) malloc(width*height*0x4);

	HWND hwnd = CreateWindowExA(0, wndc.lpszClassName, "Raster Engine", WS_OVERLAPPED | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, (int32_t)width, (int32_t)height, NULL,
			NULL, hinstance, NULL);
	assert(hwnd);

	// True width x height
	RECT rect;
	GetClientRect(hwnd, &rect);

	// The SurfaceBuffer structure representing the window surface
	SurfaceBuffer* sb = (SurfaceBuffer*) malloc(sizeof(SurfaceBuffer));
	sb->width = (uint32_t) rect.right;
	sb->height = (uint32_t) rect.bottom;
	sb->pixel_buffers[0] = d_buffering[0];
	sb->pixel_buffers[1] = d_buffering[1];

	// GDI Specific Info
	BITMAPINFOHEADER bmh = {};
	bmh.biSize = sizeof(BITMAPINFOHEADER);
	bmh.biWidth = sb->width;
	bmh.biHeight = sb->height;
	bmh.biPlanes = 1;
	bmh.biBitCount = 32;
	bmh.biCompression = BI_RGB;

	sb->bminfo.bmiHeader = bmh;
	sb->hwnd = hwnd;

	return sb;
}

void CloseWin32(SurfaceBuffer* sb)
{
	if (sb && sb->pixel_buffers[0] && sb->pixel_buffers[1])
	{
		free(sb->pixel_buffers[0]);
		free(sb->pixel_buffers[1]);

		free(sb);
	}
}

