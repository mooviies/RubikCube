#ifndef CONSTANTS_H
#define CONSTANTS_H

enum Color
{
    Green  = 0x49b849,
    Red    = 0xdb261b,
    Yellow = 0xddd101,
    White  = 0xd4cfd5,
    Blue   = 0x0281c8,
    Orange = 0xf58220,
    Black  = 0x000000
};

enum class Face
{
    Left  = 0,
    Front = 1,
    Right = 2,
    Back  = 3,
    Up    = 4,
    Down  = 5
};

enum Layer
{
    Left       = 0x001,
    Front      = 0x002,
    Right      = 0x004,
    Back       = 0x008,
    Up         = 0x010,
    Down       = 0x020,
    Middle     = 0x040,
    Equator    = 0x080,
    Horizontal = 0x100,
    Standing   = 0x200,
    CubeR      = 0x005,
    CubeU      = 0x030,
    CubeF      = 0x00A
};

enum class LayerLetterOut
{
    Left       = 'L',
    Front      = 'F',
    Right      = 'R',
    Back       = 'B',
    Up         = 'U',
    Down       = 'D',
    Middle     = 'M',
    Equator    = 'E',
    Horizontal = 'H',
    Standing   = 'S',
    CubeR      = 'x',
    CubeU      = 'y',
    CubeF      = 'z'
};

enum class LayerLetterIn
{
    Left  = 'l',
    Front = 'f',
    Right = 'r',
    Back  = 'b',
    Up    = 'u',
    Down  = 'd'
};

enum class Rotation
{
    Clockwise,
    CounterClockwise,
    Turn180
};

const char SYMBOL_COUNTER_CLOCKWISE = '\'';
const char SYMBOL_180 = '2';
const char SYMBOL_WIDE = 'w';

const int NUMBER_SIDE = 6;

const float CUBE_SIZE = 0.5f;
const float CUBE_HSIZE = CUBE_SIZE / 2.0f;

const int MIN_SIZE = 1;
const int MAX_SIZE = 100;

#endif // CONSTANTS_H
