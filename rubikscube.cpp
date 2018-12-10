#include "rubikscube.h"

#include <stdexcept>

#include "cube.h"

RubiksCube::RubiksCube(int size)
{
    if(size < 1)
        throw std::invalid_argument("A rubik's cube can't be smaller than 1");

    _size = size;
    _sizeOfVertices = 0;
    create();
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
}

void RubiksCube::createModel()
{
    int nbVertices = NUMBER_SIDE * _size * _size * SQUARE_VERTICES_COUNT;
    _vertices = new Vertex[nbVertices];
    _sizeOfVertices = sizeof(Vertex) * nbVertices;
    _width = getWidth(_size);
    _cellWidth = _width / float(_size);
    float halfCellWidth = _cellWidth / 2.0;
    float halfWidth = _width / 2.0;
    float widthLess = halfWidth - halfCellWidth;

    Cube cube;
    cube.setSize(_cellWidth);

    _cube = new Face**[NUMBER_SIDE];
    for(int i = 0; i < NUMBER_SIDE; i++)
    {
        Face face = (Face)(1 << i);
        float x, y, z;
        float xi = 0, yi = 0, zi = 0;
        switch(face)
        {
        case Face::Front:
            x = -widthLess;
            y = -widthLess;
            z = widthLess;
            xi = _cellWidth;
            yi = _cellWidth;
            cube.setColor(face, Color::Green);
            break;
        case Face::Back:
            x = widthLess;
            y = -widthLess;
            z = -widthLess;
            xi = -_cellWidth;
            yi = _cellWidth;
            cube.setColor(face, Color::Blue);
            break;
        case Face::Left:
            x = -widthLess;
            y = -widthLess;
            z = -widthLess;
            yi = _cellWidth;
            zi = _cellWidth;
            cube.setColor(face, Color::Orange);
            break;
        case Face::Right:
            x = widthLess;
            y = -widthLess;
            z = widthLess;
            yi = _cellWidth;
            zi = -_cellWidth;
            cube.setColor(face, Color::Red);
            break;
        case Face::Up:
            x = -widthLess;
            y = widthLess;
            z = widthLess;
            xi = _cellWidth;
            zi = -_cellWidth;
            cube.setColor(face, Color::White);
            break;
        case Face::Down:
            x = -widthLess;
            y = -widthLess;
            z = -widthLess;
            xi = _cellWidth;
            zi = _cellWidth;
            cube.setColor(face, Color::Yellow);
            break;
        }

        _cube[i] = new Face*[_size];
        for(int j = 0; j < _size; j++)
        {
            _cube[i][j] = new Face[_size];
            for(int k = 0; k < _size; k++)
            {
                if(xi == 0)
                    cube.setPosition(QVector3D(x, y + yi * k, z + zi * j));
                else if(yi == 0)
                    cube.setPosition(QVector3D(x + xi * j, y, z + zi * k));
                else if(zi == 0)
                    cube.setPosition(QVector3D(x + xi * j, y + yi * k, z));

                int id = getID(i, j, k);
                cube.setID(id);
                cube.generate();

                const Vertex* faceVertices = cube.vertices(face);
                for(int m = 0; m < SQUARE_VERTICES_COUNT; m++)
                {
                    _vertices[m + id * SQUARE_VERTICES_COUNT] = faceVertices[m];
                }

                _cube[i][j][k] = face;
            }
        }
    }
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
