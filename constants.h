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

enum RotationComponent
{
    Left             = 0x0000100,
    Front            = 0x0000200,
    Right            = 0x0000400,
    Back             = 0x0000800,
    Up               = 0x0001000,
    Down             = 0x0002000,
    Middle           = 0x0004000,
    Equator          = 0x0008000,
    Horizontal       = 0x0010000,
    Standing         = 0x0020000,
    CenterLayers     = 0x003C000,
    CubeX            = 0x0040000,
    CubeY            = 0x0080000,
    CubeZ            = 0x0100000,
    WholeCube        = 0x01C0000,
    Wide             = 0x0200000,
    Clockwise        = 0x0400000,
    CounterClockwise = 0x0800000,
    Turn180          = 0x1000000,
    NbLayerMask      = 0x00000FF,
    ComponentsMask   = 0xFFFFF00
};

enum LayerOut
{
    L_Left       = 'L',
    L_Front      = 'F',
    L_Right      = 'R',
    L_Back       = 'B',
    L_Up         = 'U',
    L_Down       = 'D',
    L_Middle     = 'M',
    L_Equator    = 'E',
    L_Horizontal = 'H',
    L_Standing   = 'S',
    L_CubeX      = 'x',
    L_CubeY      = 'y',
    L_CubeZ      = 'z'
};

enum LayerIn
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

const float CUBE_SIZE = 0.5f;
const float CUBE_HSIZE = CUBE_SIZE / 2.0f;

const int MIN_SIZE = 1;
const int MAX_SIZE = 100;

const float ROTATION_SPEED = 6;

#endif // CONSTANTS_H
