#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include "constants.h"
#include "vertex.h"

class RubiksCube
{
public:
    RubiksCube(int size);
    ~RubiksCube();

    int getSizeOfVertices() const { return _sizeOfVertices; }
    const Vertex* vertices() const { return _vertices; }

    void reset();

private:
    void create();
    void createDataModel();
    void create3DModel();

    void clean();

private:
    int _size;
    Face ***_cube;
    Vertex* _vertices;
    int _sizeOfVertices;
};

#endif // RUBIKSCUBE_H
