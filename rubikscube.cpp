#include "rubikscube.h"

#include <stdexcept>

RubiksCube::RubiksCube(int size)
{
    if(size < 1)
        throw std::invalid_argument("A rubik's cube can't be smaller than 1");

    _size = size;

    _cube = new Side**[NUMBER_SIDE];
    for(int i = 0; i < NUMBER_SIDE; i++)
    {
        _cube[i] = new Side*[_size];
        for(int j = 0; j < _size; j++)
        {
            _cube[i][j] = new Side[_size];
            for(int k = 0; k < _size; k++)
            {
                _cube[i][j][k] = (Side)(1 << i);
            }
        }
    }
}

RubiksCube::~RubiksCube()
{
    for(int i = 0; i < NUMBER_SIDE; i++)
    {

        for(int j = 0; j < _size; j++)
        {
            delete _cube[i][j];
        }
        delete _cube[i];
    }
    delete _cube;
}
