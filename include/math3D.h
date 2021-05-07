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

typedef struct Vec3_t
{
		float x, y, z;
}Vec3;

typedef struct BB_t
{
	Vec2 p0, p1;
}BB;

typedef struct Vertex
{
	Vec3 position;
	Vec3 color;
	Vec2 texcoord;
}Vertex;

typedef struct Triangle_t
{
	Vec2 p0, p1, p2; // 2d pixel position
	Vec3 c0, c1, c2; // color
	Vec2 uv0, uv1, uv2; // (u,v) texture coordinates

	BB triBB;
}Triangle;

//
//	Math Utils
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

static inline float OrientedTriangleArea(Vec2 a, Vec2 b, Vec2 c)
{
	return ((b.x - a.x)*(c.y - a.y) - (c.x - a.x)*(b.y - a.y))/2.0f;
}

#endif /* INCLUDE_MATH3D_H_ */
