/*
===========================================================================
                          _________
                         /\        \	created by
                        /  \        \			(GABRIEL ->
                       /    \________\                <- EPILEF)
                       \    /        /
                        \  /        /
                         \/________/
     - RASTER ENGINE					math3D.h
===========================================================================
The MIT License

Copyright (c) 2020 Gabriel Felipe. https://github.com/dasilvagf

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

#ifndef INCLUDE_MATH3D_H_
#define INCLUDE_MATH3D_H_

#include <math.h>
#include <immintrin.h>

//
//	Definitions
//

typedef struct Vec2_t
{
		float x, y;
}Vec2;

typedef struct BB_t
{
	Vec2 p0, p1;
}BB;

typedef struct Triangle_t
{
	Vec2 p0, p1, p2;
	BB triBB;
}Triangle;

//
//	Utils
//

static inline float min_f(float a, float b)
{
	return (a <= b) ? a : b;
}

static inline float max_f(float a, float b)
{
	return (a >= b) ? a : b;
}

static inline void GenerateBoundingBoxForTriangle(Triangle* t)
{
	t->triBB.p0.x = min_f(min_f(t->p0.x, t->p1.x), t->p2.x);
	t->triBB.p0.y = min_f(min_f(t->p0.y, t->p1.y), t->p2.y);

	t->triBB.p1.x = max_f(max_f(t->p0.x, t->p1.x), t->p2.x);
	t->triBB.p1.y = max_f(max_f(t->p0.y, t->p1.y), t->p2.y);
}

typedef struct StarPolygon_t
{
	Triangle* triangles;
	uint32_t n_triangles;
}StarPolygon;

static inline StarPolygon* ZeldaTriForcePolygon(float side_length, Vec2 center)
{
		float h = (sqrt(3.0f)/2.0f)*side_length;
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

static inline StarPolygon* CreateStarPolygon(float side_length, Vec2 center)
{
	float h = (sqrt(3.0f)/2.0f)*side_length;
	float l = side_length;

	StarPolygon* star = (StarPolygon*) malloc(sizeof(StarPolygon));
	star->n_triangles = 6;
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
	t_tmp.p0.x = center.x - (l/2.0f + l);
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
	t_tmp.p1.x = center.x + l + l/2.0f;
	t_tmp.p1.y = center.y - h;
	t_tmp.p2.x = center.x + l;
	t_tmp.p2.y = center.y;
	GenerateBoundingBoxForTriangle(&t_tmp);
	star->triangles[5] = t_tmp;


	// Create Down-Side Triangles

	return star;
}


#endif /* INCLUDE_MATH3D_H_ */
