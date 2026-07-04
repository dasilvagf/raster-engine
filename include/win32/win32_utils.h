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

#ifndef INCLUDE_WIN32_UTILS_H_
#define INCLUDE_WIN32_UTILS_H_

#include <stdio.h>
#include <Windows.h>
#include <xmmintrin.h>

static void OpenLogConsole()
{
    if (AllocConsole())
    {
        FILE* fp = NULL;

        // Redirect both standard and error output to my log console
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);

        SetConsoleTitleA("Raster Engine - Debug Console");
        printf("Debug Console has been initialized...\n");
    }
}

static void CloseLogConsole()
{
    FreeConsole();
}

#endif