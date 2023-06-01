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

#ifndef _INCLUDE_VERTEX_STAGE_H_
#define _INCLUDE_VERTEX_STAGE_H_

#include "win32_buffer.h"
#include "math3D.h"

typedef struct vertex_data_t
{
    Vertex* vb;
    uint32_t vb_size;
    uint32_t* ib;
    uint32_t ib_size;
}vertex_data;

typedef struct viewport_t
{
    uint32_t width;
    uint32_t height;
    uint32_t top;
    uint32_t left;
}viewport;

void TransformVerticesToHomogenousSpace();
void TransformVerticesToNDCSpace();
void ClipVetices();
void MapToViewport(viewport vw, vertex_data vd);

uint32_t AssemblyTriangles(Vertex* vb, uint32_t* ib, uint32_t vb_size, uint32_t ib_size, Triangle** t);


#endif
