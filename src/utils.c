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

#include "../include/utils.h"

StarPolygon* CreateZeldaTriForcePolygon(float side_length, Vec2 center)
{
		float h = 0.8661f*side_length;
		float l = side_length;

		StarPolygon* star = (StarPolygon*) malloc(sizeof(StarPolygon));
		star->n_triangles = 3;
		star->triangles = (Triangle*) malloc(3*sizeof(Triangle));

		Triangle t_tmp;

		// triangle 0
		t_tmp.p0.x = center.x - l/2.0f;
		t_tmp.p0.y = center.y + h;
		t_tmp.p1.x = center.x + l/2.0f;
		t_tmp.p1.y = center.y + h;
		t_tmp.p2.x = center.x;
		t_tmp.p2.y = center.y + 2.0f*h;
		GenerateBoundingBoxForTriangle(&t_tmp);
		star->triangles[0] = t_tmp;

		// triangle 1
		t_tmp.p0.x = center.x - l;
		t_tmp.p0.y = center.y;
		t_tmp.p1.x = center.x;
		t_tmp.p1.y = center.y;
		t_tmp.p2.x = center.x - l/2.0f;
		t_tmp.p2.y = center.y + h;
		GenerateBoundingBoxForTriangle(&t_tmp);
		star->triangles[1] = t_tmp;

		// triangle 2
		t_tmp.p0.x = center.x;
		t_tmp.p0.y = center.y;
		t_tmp.p1.x = center.x + l;
		t_tmp.p1.y = center.y;
		t_tmp.p2.x = center.x + l/2.0f;
		t_tmp.p2.y = center.y + h;
		GenerateBoundingBoxForTriangle(&t_tmp);
		star->triangles[2] = t_tmp;

		return star;
}

StarPolygon* CreateStarPolygon(float side_length, Vec2 center)
{
	float h = 0.8661f*side_length;
	float l = side_length;

	StarPolygon* star = (StarPolygon*) malloc(sizeof(StarPolygon));
	star->n_triangles = 12;
	star->triangles = (Triangle*) malloc(12*sizeof(Triangle));

	//
	// Create Up-Side Triangles
	//
	Triangle t_tmp;

	// triangle 0
	t_tmp.p0.x = center.x - l/2.0f;
	t_tmp.p0.y = center.y + h;
	t_tmp.p1.x = center.x + l/2.0f;
	t_tmp.p1.y = center.y + h;
	t_tmp.p2.x = center.x;
	t_tmp.p2.y = center.y + 2.0f*h;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[0] = t_tmp;

	// triangle 1
	t_tmp.p0.x = center.x - l;
	t_tmp.p0.y = center.y;
	t_tmp.p1.x = center.x;
	t_tmp.p1.y = center.y;
	t_tmp.p2.x = center.x - l/2.0f;
	t_tmp.p2.y = center.y + h;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[1] = t_tmp;

	// triangle 2
	t_tmp.p0.x = center.x;
	t_tmp.p0.y = center.y;
	t_tmp.p1.x = center.x + l;
	t_tmp.p1.y = center.y;
	t_tmp.p2.x = center.x + l/2.0f;
	t_tmp.p2.y = center.y + h;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[2] = t_tmp;

	// triangle 3
	t_tmp.p0.x = center.x - 1.5f*l;
	t_tmp.p0.y = center.y - h;
	t_tmp.p1.x = center.x - l/2.0f;
	t_tmp.p1.y = center.y - h;
	t_tmp.p2.x = center.x - l;
	t_tmp.p2.y = center.y;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[3] = t_tmp;

	// triangle 4
	t_tmp.p0.x = center.x - l/2.0f;
	t_tmp.p0.y = center.y - h;
	t_tmp.p1.x = center.x + l/2.0f;
	t_tmp.p1.y = center.y - h;
	t_tmp.p2.x = center.x;
	t_tmp.p2.y = center.y;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[4] = t_tmp;

	// triangle 5
	t_tmp.p0.x = center.x + l/2.0f;
	t_tmp.p0.y = center.y - h;
	t_tmp.p1.x = center.x + 1.5f*l;
	t_tmp.p1.y = center.y - h;
	t_tmp.p2.x = center.x + l;
	t_tmp.p2.y = center.y;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[5] = t_tmp;

	//
	// Create Down-Side Triangles
	//

	// triangle 6
	t_tmp.p0.x = center.x;
	t_tmp.p0.y = center.y - 2.0f*h;
	t_tmp.p1.x = center.x + l/2.0f;
	t_tmp.p1.y = center.y - h;
	t_tmp.p2.x = center.x - l/2.0f;
	t_tmp.p2.y = center.y - h;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[6] = t_tmp;

	// triangle 7
	t_tmp.p0.x = center.x - l/2.0f;
	t_tmp.p0.y = center.y - h;
	t_tmp.p1.x = center.x;
	t_tmp.p1.y = center.y;
	t_tmp.p2.x = center.x - l;
	t_tmp.p2.y = center.y;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[7] = t_tmp;

	// triangle 8
	t_tmp.p0.x = center.x + l/2.0f;
	t_tmp.p0.y = center.y - h;
	t_tmp.p1.x = center.x + l;
	t_tmp.p1.y = center.y;
	t_tmp.p2.x = center.x;
	t_tmp.p2.y = center.y;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[8] = t_tmp;

	// triangle 9
	t_tmp.p0.x = center.x - l;
	t_tmp.p0.y = center.y;
	t_tmp.p1.x = center.x - l/2.0f;
	t_tmp.p1.y = center.y + h;
	t_tmp.p2.x = center.x - 1.5f*l;
	t_tmp.p2.y = center.y + h;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[9] = t_tmp;

	// triangle 10
	t_tmp.p0.x = center.x;
	t_tmp.p0.y = center.y;
	t_tmp.p1.x = center.x + l/2.0f;
	t_tmp.p1.y = center.y + h;
	t_tmp.p2.x = center.x - l/2.0f;
	t_tmp.p2.y = center.y + h;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[10] = t_tmp;

	// triangle 11
	t_tmp.p0.x = center.x + l;
	t_tmp.p0.y = center.y;
	t_tmp.p1.x = center.x + 1.5f*l;
	t_tmp.p1.y = center.y + h;
	t_tmp.p2.x = center.x + l/2.0f;
	t_tmp.p2.y = center.y + h;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[11] = t_tmp;

	return star;
}
