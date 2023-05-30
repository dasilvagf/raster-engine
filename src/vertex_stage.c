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

void TransformVerticesToHomogenousSpace()
{

}

void TransformVerticesToNDCSpace()
{
    /*
	* ===================================================
	*	Cuboid Frustum (LHS) - Direct3D Convention
	* ===================================================
	*  
	*  x e [-1.0f, 1.0f]
	*  y e [-1.0f, 1.0f]
	*  z e [ 0.0f, 1.0f]
	*  
	*				   _________ (1.0f, 1.0f, 1.0f)
	*				  /|	   /| 
	*	   Z+		 / |	  / |
	*	  /	 		*--------* (1.0f, 1.0f, 0.0f)
	*	 /		    |  |_____|__| (1.0f, -1.0f, 1.0f)
	*	/			| / 	 | /
	*				|/		 |/
	*				*--------* (1.0f, -1.0f, 0.0f) 
	*/
}

void ClipVetices()
{

}

Triangle* AssemblyTriangles(Vertex* vb, uint32_t* ib, uint32_t vb_size, uint32_t ib_size)
{
    if (vb_size > 0u && ib_size > 0u)
    {
        // our triangle count
        assert((ib_size % 3) != 0);
        Triangle* t = (Triangle*)malloc(sizeof(Triangle) * (ib_size / 3u));

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

            //
            // add to triangle list
            //
            t[(i / 3u)] = tri;
        }

        return t;
    }
    else
        return NULL;
}


