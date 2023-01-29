/*
    Virtual Rubik's Cube is a tool to explore Rubik's cubes of any sizes.
    Copyright (C) 2018 mooviies

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    https://github.com/mooviies/RubikCube
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

enum LayerMain
{
    L_Left       = 'L',
    L_Front      = 'F',
    L_Right      = 'R',
    L_Back       = 'B',
    L_Up         = 'U',
    L_Down       = 'D',
    L_Middle     = 'M',
    L_Equator    = 'E',
    L_Standing   = 'S',
    L_CubeX      = 'x',
    L_CubeY      = 'y',
    L_CubeZ      = 'z'
};

enum LayerSub
{
    Li_Left  = 'l',
    Li_Front = 'f',
    Li_Right = 'r',
    Li_Back  = 'b',
    Li_Up    = 'u',
    Li_Down  = 'd'
};

const char SYMBOL_COUNTER_CLOCKWISE = '\'';
const char SYMBOL_180 = '2';
const char SYMBOL_WIDE = 'w';

const int NUMBER_SIDE = 6;
const int SQUARE_VERTICES_COUNT = 4;
const int CUBE_VERTICES_COUNT = SQUARE_VERTICES_COUNT * 6;

const float CUBE_SIZE = 0.5f;
const float CUBE_HSIZE = CUBE_SIZE / 2.0f;

const int MIN_SIZE = 1;
const int MAX_SIZE = 100;

const float ROTATION_SPEED = 6;

const char* const SETTINGS_KEY_SIZE = "general/size";
const char* const SETTINGS_KEY_SAVE = "general/save";
const char* const SETTINGS_KEY_FAST_MODE = "options/fastmode";
const char* const SETTINGS_KEY_WINDOW_STATE = "window/state";
const char* const SETTINGS_KEY_WINDOW_RECT = "window/rect";

const float DEPTH_NEAR = 0.1f;
const float DEPTH_FAR = 30.0f;

#endif // CONSTANTS_H
