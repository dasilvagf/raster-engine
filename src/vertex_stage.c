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

#include "../include/vertex_stage.h"

void LoadVerticesDataFromDisk(const char* filename, vertex_data** out_vertex_data)
{
    //
    // Parse file
    //
    uint32_t vertices_count = 4u;
    uint32_t indices_count = 6u;

    //
    // Init mem
    //
    Vertex* vertex_buffer = (Vertex*)malloc(sizeof(Vertex) * vertices_count);
    uint32_t* index_buffer = (uint32_t*)malloc(sizeof(uint32_t) * indices_count);

    // Set the vertex buffer to 1. It will be used by the homogenous processing later
    memset(vertex_buffer, 1, sizeof(Vertex) * vertices_count);

    //
    // Populate with data
    //

	// ADD A TEST QUAD
		
	// position
	Vec3 p0 = { 100.0f , 100.0f , 0.0f};
	Vec3 p1 = { 500.0f , 100.0f  , 0.0f};
	Vec3 p2 = { 500.0f , 500.0f  , 0.0f};
	Vec3 p3 = { 100.0f , 500.0f  , 0.0f};

	// color
	Vec3 r = { 1.0f, 0.0f, 0.0f };
	Vec3 g = { 0.0f, 1.0f, 0.0f };
	Vec3 b = { 0.0f, 0.0f, 1.0f };
	Vec3 w = { 1.0f, 1.0f, 1.0f };
	
	vertex_buffer[0].position = p0;
	vertex_buffer[0].color = r;

	vertex_buffer[1].position = p1;
	vertex_buffer[1].color = g;


	vertex_buffer[2].position = p2;
	vertex_buffer[2].color = b;

	
	vertex_buffer[3].position = p3;
	vertex_buffer[3].color = w;

    index_buffer[0] = 0u;
	index_buffer[1] = 1u;
	index_buffer[2] = 2u;
    
    index_buffer[3] = 0u;
	index_buffer[4] = 2u;
	index_buffer[5] = 3u;

	*out_vertex_data = (vertex_data*)malloc(sizeof(vertex_data));
	(*out_vertex_data)->ib_size = indices_count;
	(*out_vertex_data)->vb_size = vertices_count;
	(*out_vertex_data)->ib = index_buffer;
	(*out_vertex_data)->vb = vertex_buffer;
}

void ProcessVertices(vertex_pipeline_desc* pipeline_desc, vertex_data** in_out_data)
{
	//
    // Linear Coordinates Space
    //



    //
    // Homogenous/Projective Coordinates Space
    //



    //
    // Viewport mapping
    //

    // construct viewport matrix
    Mat4x4 viewport_mat;

}

uint32_t AssemblyTriangles(vertex_data* input_data, Triangle** output_data)
{
    if (!input_data)
    {
        *output_data = NULL;
        return -1;
    }

	uint32_t vb_size = input_data->vb_size;
	uint32_t ib_size = input_data->ib_size;
	Vertex* vb = input_data->vb;
	uint32_t* ib = input_data->ib;
    
    if (vb_size > 0u && ib_size > 0u)
    {
        // our triangle count
        assert((ib_size % 3) == 0);
        (*output_data) = (Triangle*)malloc(sizeof(Triangle) * (ib_size / 3u));

        // generate triangles folliwing CCW winding order
        for (uint32_t i = 0u; i < ib_size; i += 3u) {
            Triangle tri;

            //
            // position and depth
            //
            tri.p0.x = vb[ib[i]].position.x;
            tri.p0.y = vb[ib[i]].position.y;
            tri.d0 = vb[ib[i]].position.z;

            tri.p1.x = vb[ib[i + 1u]].position.x;
            tri.p1.y = vb[ib[i + 1u]].position.y;
            tri.d1 = vb[ib[i + 1u]].position.z;

            tri.p2.x = vb[ib[i + 2u]].position.x;
            tri.p2.y = vb[ib[i + 2u]].position.y;
            tri.d2 = vb[ib[i + 2u]].position.z;

            //
            // uv texture mapping
            //
            tri.uv0.x = vb[ib[i]].texcoord.x;
            tri.uv0.y = vb[ib[i]].texcoord.y;

            tri.uv1.x = vb[ib[i + 1u]].texcoord.x;
            tri.uv1.y = vb[ib[i + 1u]].texcoord.y;

            tri.uv2.x = vb[ib[i + 2u]].texcoord.x;
            tri.uv2.y = vb[ib[i + 2u]].texcoord.y;

            //
            // vertex color
            //
            tri.c0.x = vb[ib[i]].color.x;
            tri.c0.y = vb[ib[i]].color.y;
            tri.c0.z = vb[ib[i]].color.z;

            tri.c1.x = vb[ib[i + 1u]].color.x;
            tri.c1.y = vb[ib[i + 1u]].color.y;
            tri.c1.z = vb[ib[i + 1u]].color.z;

            tri.c2.x = vb[ib[i + 2u]].color.x;
            tri.c2.y = vb[ib[i + 2u]].color.y;
            tri.c2.z = vb[ib[i + 2u]].color.z;

            GenerateBoundingBoxForTriangle(&tri);

            //
            // add to triangle list
            //
            (*output_data)[(i / 3u)] = tri;
        }

        return (ib_size / 3u);
    }
    else
        return -1;
}


