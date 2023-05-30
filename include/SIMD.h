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

#ifndef INCLUDE_SIMD_H_
#define INCLUDE_SIMD_H_

#include <immintrin.h>

//
// SIMD registers 
//
#if defined(_M_X64)
#define SIMD_REGISTERS_COUNT 16u
#else
#define SIMD_REGISTERS_COUNT 8u
#endif

//
// Constants
//
static const __m128 SIMD_128_MASK_HI = { (float)(0xFFFFFFFF), (float)(0xFFFFFFFF), 0.0f, 0.0f };
static const __m128 SIMD_128_MASK_LOW = { 0.0f, 0.0f, (float)(0xFFFFFFFF), (float)(0xFFFFFFFF) };
static const __m128 SIMD_128_X_ONE = { 1.0f, 0.0f, 0.0f, 0.0f };
static const __m128 SIMD_128_Y_ONE = { 0.0f, 1.0f, 0.0f, 0.0f };
static const __m128 SIMD_128_Z_ONE = { 0.0f, 0.0f, 1.0f, 0.0f };
static const __m128 SIMD_128_W_ONE = { 0.0f, 0.0f, 0.0f, 1.0f };

#endif