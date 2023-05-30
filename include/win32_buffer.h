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

#ifndef INCLUDE_WIN32_BUFFER_H_
#define INCLUDE_WIN32_BUFFER_H_

#include <Windows.h>
#include <xmmintrin.h>

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
