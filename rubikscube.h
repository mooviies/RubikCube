#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include "constants.h"

class RubiksCube
{
public:
    RubiksCube(int size);
    ~RubiksCube();

private:
    int _size;
    Side ***_cube;
};

#endif // RUBIKSCUBE_H
