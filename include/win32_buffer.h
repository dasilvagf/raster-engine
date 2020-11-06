/*
===========================================================================
                          _________
                         /\        \	created by
                        /  \        \			(GABRIEL ->
                       /    \________\                <- EPILEF)
                       \    /        /
                        \  /        /
                         \/________/
     - RASTER ENGINE					win32_buffer.h
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

#ifndef INCLUDE_WIN32_BUFFER_H_
#define INCLUDE_WIN32_BUFFER_H_

#include <Windows.h>
#include "utils.h"

//
//			Windows Init/SetUp
//

// Act as the SwapChain structure of Direct3D
typedef struct SurfaceBuffer_t
{
	uint32_t width, height;
	uint32_t* surface_buffer;

	// Win32/GDI specific
	HWND hwnd;
	BITMAPINFO bminfo;
}SurfaceBuffer;

SurfaceBuffer* InitWin32(uint32_t width, uint32_t height, HINSTANCE hinstance);
void CloseWin32(SurfaceBuffer* sb);

#endif /* INCLUDE_WIN32_BUFFER_H_ */
