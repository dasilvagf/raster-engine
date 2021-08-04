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
#include "SIMD.h"

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

typedef union Vec4_t
{
	struct
	{
		float x, y, z, w;
	};
	__m128 xyzw;
}Vec4;

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

static inline float OrientedArea(Vec2 a, Vec2 b, Vec2 c)
{
	return ((b.x - a.x)*(c.y - a.y) - (c.x - a.x)*(b.y - a.y));
}

static inline float OrientedTriangleArea(Vec2 a, Vec2 b, Vec2 c)
{
	return OrientedArea(a, b, c) / 2.0f;
}

//
//	FAST Math Utils
//
static inline Vec4 DotProduct_SIMD(Vec4 a[4], Vec4 b[4])
{
	// spread the values by coordinates
	__m128 dx_a = _mm_set_ps(a[0].x, a[1].x, a[2].x, a[3].x);
	__m128 dx_b = _mm_set_ps(b[0].x, b[1].x, b[2].x, b[3].x);

	__m128 dy_a = _mm_set_ps(a[0].y, a[1].y, a[2].y, a[3].y);
	__m128 dy_b = _mm_set_ps(b[0].y, b[1].y, b[2].y, b[3].y);

	__m128 dz_a = _mm_set_ps(a[0].z, a[1].z, a[2].z, a[3].z);
	__m128 dz_b = _mm_set_ps(b[0].z, b[1].z, b[2].z, b[3].z);

	__m128 dw_a = _mm_set_ps(a[0].w, a[1].w, a[2].w, a[3].w);
	__m128 dw_b = _mm_set_ps(b[0].w, b[1].w, b[2].w, b[3].w);

	// multiply each set of coordinates
	__m128 dx = _mm_mul_ps(dx_a, dx_b);  
	__m128 dy = _mm_mul_ps(dy_a, dy_b);
	__m128 dz = _mm_mul_ps(dz_a, dz_b);
	__m128 dw = _mm_mul_ps(dw_a, dw_b);

	// add
	dx = _mm_add_ps(dx, dy);
	dy = _mm_add_ps(dz, dw);

	// dot
	Vec4 dot;
	dot.xyzw = _mm_add_ps(dx, dy);

	return dot;
}



#endif /* INCLUDE_MATH3D_H_ */
