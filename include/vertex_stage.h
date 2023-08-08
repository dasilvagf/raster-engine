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

typedef struct vertex_pipeline_desc_t
{
    Mat4x4 transformation_matrices; // contactenation of the object/world/projection matrices (multiply only ONCE)
    viewport viewp;
}vertex_pipeline_desc;

void LoadVerticesDataFromDisk(const char* filename, vertex_data** out_vertex_data);

// simulate (try-to!) the D3D9 Fixed-Pipeline Vertex Processing pipeline
void ProcessVertices(vertex_pipeline_desc* pipeline_desc, vertex_data* in_data, vertex_data** out_data);

// generate Triangles from Vertex/Index data
uint32_t AssemblyTriangles(vertex_data* input_data, Triangle** output_data);


#endif
