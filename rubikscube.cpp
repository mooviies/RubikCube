#include "rubikscube.h"

#include <stdexcept>

#include "cube.h"

RubiksCube::RubiksCube(int size)
{
    if(size < 1)
        throw std::invalid_argument("A rubik's cube can't be smaller than 1");

    _size = size;
    _sizeOfVertices = 0;
}

RubiksCube::~RubiksCube()
{
    clean();
}

void RubiksCube::reset()
{
    clean();
    create();
}

void RubiksCube::create()
{
    createModel();
    create3DModel();
}

void RubiksCube::createModel()
{
    int nbVertices = NUMBER_SIDE * _size * _size * SQUARE_VERTICES_COUNT;
    _vertices = new Vertex[nbVertices];
    _sizeOfVertices = sizeof(Vertex) * nbVertices;

    Cube cube;

    _cube = new Face**[NUMBER_SIDE];
    for(int i = 0; i < NUMBER_SIDE; i++)
    {
        _cube[i] = new Face*[_size];
        for(int j = 0; j < _size; j++)
        {
            _cube[i][j] = new Face[_size];
            for(int k = 0; k < _size; k++)
            {
                _cube[i][j][k] = (Face)(1 << i);
            }
        }
    }
}

void RubiksCube::create3DModel()
{
}

void RubiksCube::clean()
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
    delete[] _vertices;
    _sizeOfVertices = 0;
}
