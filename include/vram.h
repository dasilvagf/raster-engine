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

#ifndef INCLUDE_VRAM_H_
#define INCLUDE_VRAM_H_

#include "utils.h"

#define VRAM_MAX_SIZE 1024u * 1024u * 1024u // 1GB
#define CLEAR_VRAM_OFFSET 0u
#define ALLOC_VRAM(size, mem_offset_ptr)(VERTEX_GPU_VRAM + mem_offset_ptr); mem_offset_ptr += size; assert(mem_offset_ptr < VRAM_MAX_SIZE)

#endif
