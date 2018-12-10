#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include <cmath>

#include "constants.h"
#include "vertex.h"

class RubiksCube
{
public:
    RubiksCube(int size);
    ~RubiksCube();

    int getSizeOfVertices() const { return _sizeOfVertices; }
    const Vertex* vertices() const { return _vertices; }

    int size() const { return _size; }
    float width() const { return _width; }

    void reset();

    inline float getID(int i, int j, int k) const { return j + k * _size + i * _size * _size; }

private:
    void create();
    void createModel();

    void clean();

    inline static float getWidth(float size) { return 1.6f * log(size) - 0.7f; }

private:
    Face ***_cube;
    Vertex* _vertices;

    float _width;
    float _cellWidth;

    int _size;
    int _sizeOfVertices;
};

#endif // RUBIKSCUBE_H
