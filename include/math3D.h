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
	union
	{
		struct
		{
			float x, y, z, w;
		};
		__m128 xyzw;
	};
}Vec4;

typedef struct Mat4x4
{
	union
	{
		struct
		{
			float	m00, m01, m02, m03,
					m10, m11, m12, m13,
					m20, m21, m22, m23,
					m30, m31, m32, m33;
		};

		struct
		{
			__m128 row0;
			__m128 row1;
			__m128 row2;
			__m128 row3;
		};
	};
}Mat4x4;

typedef struct BB_t
{
	Vec2 p0, p1;
}BB;

typedef struct Vertex
{
	union
	{
		Vec3 position;
		Vec4 h_position;
	};
	Vec3 color;
	Vec2 texcoord;
}Vertex;

typedef struct Triangle_t
{
	Vec2 p0, p1, p2; // 2d pixel position
	Vec3 c0, c1, c2; // color
	Vec2 uv0, uv1, uv2; // (u,v) texture coordinates
	float d0, d1, d2; // depth 

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

// Dot product 8 vec4 at the same time
static inline __m128 DotProduct_SIMD(__m128 vectors[8])
{
	// Multiplications
	__m128 AdotB = _mm_mul_ps(vectors[0], vectors[1]);
	__m128 CdotD = _mm_mul_ps(vectors[2], vectors[3]);
	__m128 EdotF = _mm_mul_ps(vectors[4], vectors[5]);
	__m128 GdotH = _mm_mul_ps(vectors[6], vectors[7]);

	// Additions
	AdotB = _mm_hadd_ps(AdotB, CdotD);
	CdotD = _mm_hadd_ps(EdotF, GdotH);
	return  _mm_hadd_ps(AdotB, CdotD);
}



#endif /* INCLUDE_MATH3D_H_ */
