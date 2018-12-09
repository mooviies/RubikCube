#ifndef CONSTANTS_H
#define CONSTANTS_H

enum class Side
{
    Left      = 0x01,
    Front    = 0x02,
    Right   = 0x04,
    Back    = 0x08,
    Up    = 0x10,
    Down   = 0x20
};

enum class Layer
{
    Left,
    Front,
    Right,
    Back,
    Up,
    Down,
    Middle,
    Equator,
    Standing,
    CubeR,
    CubeU,
    CubeF
};

enum class LayerLetterOut
{
    Left = 'L',
    Front = 'F',
    Right = 'R',
    Back = 'B',
    Up = 'U',
    Down = 'D',
    Middle = 'M',
    Equator = 'E',
    Standing = 'S',
    CubeR = 'x',
    CubeU = 'y',
    CubeF = 'z'
};

enum class LayerLetterIn
{
    Left = 'l',
    Front = 'f',
    Right = 'r',
    Back = 'b',
    Up = 'u',
    Down = 'd'
};

const char SYMBOL_COUNTER_CLOCKWISE = '\'';
const char SYMBOL_180 = '2';
const char SYMBOL_WIDE = 'w';

const int NUMBER_SIDE = 6;

#endif // CONSTANTS_H
