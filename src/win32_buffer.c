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

#ifdef _DEBUG
#define WINDOW_NAME "Raster Engine - DEBUG"
#else
#define WINDOW_NAME "Raster Engine - RELEASE"
#endif

#include "../include/win32_buffer.h"

//
//			Window Initialisation & SetUp
//
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	    {
			case WM_CREATE:
			{
				// Get our initial data passed via CreateWindow and made it our
				// additional window data
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
			}break;
			case WM_DESTROY: PostQuitMessage(0); break;
			case WM_KEYDOWN:
			{
				if (wParam == VK_ESCAPE)
					PostQuitMessage(0);
			}break;
			case WM_PAINT:
			{
				// Get our surface data
				SurfaceBuffer* sb = (SurfaceBuffer*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
				assert(sb);

				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);

				assert(
				StretchDIBits(hdc, 0, 0, sb->width, sb->height, 0, 0, sb->width, sb->height,
						(void*)sb->surface_buffer, &sb->bminfo, DIB_RGB_COLORS, SRCCOPY));

				EndPaint(hwnd, &ps);

			}break;
	        default:
	            return DefWindowProc(hwnd, uMsg, wParam, lParam);
	    }
	return 0;
}

SurfaceBuffer* InitWin32(uint32_t width, uint32_t height, HINSTANCE hinstance)
{
	// Basic Window Element Class Registration
	WNDCLASSEXA wndc; 
	ZeroMemory(&wndc, sizeof(WNDCLASSEXA));

	wndc.cbSize = sizeof(WNDCLASSEXA);
	wndc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndc.lpfnWndProc = WindowProc;
	wndc.hInstance = hinstance;
	wndc.lpszClassName = "raster-engine";
	assert(RegisterClassExA(&wndc));

	// The SurfaceBuffer structure representing the window surface
	SurfaceBuffer* sb = (SurfaceBuffer*) malloc(sizeof(SurfaceBuffer));

	// Create the window and pass our surface buffer, so the window callback can updated the screen-area
	// with out back-buffer
	HWND hwnd = CreateWindowExA(0, wndc.lpszClassName, WINDOW_NAME, WS_OVERLAPPED | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, (int32_t)width, (int32_t)height, NULL,
			NULL, hinstance, (void*)sb);
	assert(hwnd);

	// True width x height
	RECT rect;
	GetClientRect(hwnd, &rect);
	sb->width = (uint32_t) rect.right;
	sb->height = (uint32_t) rect.bottom;
	sb->surface_buffer = (uint32_t*) malloc(sb->width*sb->height*0x4);
	//sb->surface_buffer[1] = (uint32_t*) malloc(sb->width*sb->height*0x4);

	// GDI Specific Info
	BITMAPINFOHEADER bmh;
	bmh.biSize = sizeof(BITMAPINFOHEADER);
	bmh.biWidth = (int32_t)sb->width;
	bmh.biHeight = -((int32_t)sb->height);
	bmh.biPlanes = 1;
	bmh.biBitCount = 32;
	bmh.biCompression = BI_RGB;

	sb->bminfo.bmiHeader = bmh;
	sb->hwnd = hwnd;

	return sb;
}

void CloseWin32(SurfaceBuffer* sb)
{
	if (sb && sb->surface_buffer /*&& sb->surface_buffer[1]*/)
	{
		free(sb->surface_buffer);
		//free(sb->surface_buffer[1]);

		free(sb);
	}
}
