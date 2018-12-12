#ifndef CUBE_H
#define CUBE_H

#include "vertex.h"
#include "constants.h"

static const int SQUARE_VERTICES_COUNT = 4;
static const int CUBE_VERTICES_COUNT = SQUARE_VERTICES_COUNT * 6;

class Cube
{
public:
    Cube();
    Cube(QVector3D position, float size);

    int size() const { return _size; }

    void setPosition(const QVector3D& position);
    void setSize(float size);
    void setColor(Face face, Color color);
    void setID(int id);

    void generate();

    const Vertex* vertices() const { return _vertices; }
    const Vertex* vertices(Face face) const;
    int verticesSize() { return sizeof(_vertices); }

private:
    QVector3D _position;
    float _size;
    Vertex _vertices[24];
    Vertex _verticesByFace[6][4];
    Color _colors[6];
    int _id;
};

#endif // CUBE_H
