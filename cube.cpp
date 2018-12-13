/*
    Virtual Rubik's Cube is a tool to explore Rubik's cubes of any sizes.
    Copyright (C) 2018 mooviies

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    https://github.com/mooviies/RubikCube
*/

#include "cube.h"
#include "vertex.h"

Cube::Cube() : _size(0)
{

}

Cube::Cube(QVector3D position, float size) : _position(position), _id(0)
{
    setSize(size);
}

void Cube::setPosition(const QVector3D& position)
{
    _position = position;
}

void Cube::setSize(float size)
{
    _size = size;
}

void Cube::setColor(Face face, Color color)
{
    switch(face)
    {
    case Face::Front:
        _colors[0] = color;
        break;
    case Face::Back:
        _colors[1] = color;
        break;
    case Face::Left:
        _colors[2] = color;
        break;
    case Face::Right:
        _colors[3] = color;
        break;
    case Face::Up:
        _colors[4] = color;
        break;
    case Face::Down:
        _colors[5] = color;
        break;
    }
}

void Cube::setID(int id)
{
    _id = id;
}

void Cube::generate()
{
    float hsize = _size / 2.0;

    // Front
    _vertices[0] = Vertex(QVector3D(-hsize,  -hsize, hsize) + _position, QVector2D(0, 0), _colors[0]);
    _vertices[1] = Vertex(QVector3D(hsize,  -hsize, hsize) + _position, QVector2D(1, 0), _colors[0]);
    _vertices[2] = Vertex(QVector3D(hsize,  hsize, hsize) + _position, QVector2D(1, 1), _colors[0]);
    _vertices[3] = Vertex(QVector3D(-hsize,  hsize, hsize) + _position, QVector2D(0, 1), _colors[0]);

    // Back
    _vertices[4] = Vertex(QVector3D(hsize,  -hsize, -hsize) + _position, QVector2D(0, 0), _colors[1]);
    _vertices[5] = Vertex(QVector3D(-hsize,  -hsize, -hsize) + _position, QVector2D(1, 0), _colors[1]);
    _vertices[6] = Vertex(QVector3D(-hsize,  hsize, -hsize) + _position, QVector2D(1, 1), _colors[1]);
    _vertices[7] = Vertex(QVector3D(hsize,  hsize, -hsize) + _position, QVector2D(0, 1), _colors[1]);

    // Left
    _vertices[8] = Vertex(QVector3D(-hsize,  -hsize, -hsize) + _position, QVector2D(0, 0), _colors[2]);
    _vertices[9] = Vertex(QVector3D(-hsize,  -hsize, hsize) + _position, QVector2D(1, 0), _colors[2]);
    _vertices[10] = Vertex(QVector3D(-hsize,  hsize, hsize) + _position, QVector2D(1, 1), _colors[2]);
    _vertices[11] = Vertex(QVector3D(-hsize,  hsize, -hsize) + _position, QVector2D(0, 1), _colors[2]);

    // Right
    _vertices[12] = Vertex(QVector3D(hsize,  -hsize, hsize) + _position, QVector2D(0, 0), _colors[3]);
    _vertices[13] = Vertex(QVector3D(hsize,  -hsize, -hsize) + _position, QVector2D(1, 0), _colors[3]);
    _vertices[14] = Vertex(QVector3D(hsize,  hsize, -hsize) + _position, QVector2D(1, 1), _colors[3]);
    _vertices[15] = Vertex(QVector3D(hsize,  hsize, hsize) + _position, QVector2D(0, 1), _colors[3]);

    // Up
    _vertices[16] = Vertex(QVector3D(-hsize,  hsize, hsize) + _position, QVector2D(0, 0), _colors[4]);
    _vertices[17] = Vertex(QVector3D(hsize,  hsize, hsize) + _position, QVector2D(1, 0), _colors[4]);
    _vertices[18] = Vertex(QVector3D(hsize,  hsize, -hsize) + _position, QVector2D(1, 1), _colors[4]);
    _vertices[19] = Vertex(QVector3D(-hsize,  hsize, -hsize) + _position, QVector2D(0, 1), _colors[4]);

    // Down
    _vertices[20] = Vertex(QVector3D(-hsize,  -hsize, -hsize) + _position, QVector2D(0, 0), _colors[5]);
    _vertices[21] = Vertex(QVector3D(hsize,  -hsize, -hsize) + _position, QVector2D(1, 0), _colors[5]);
    _vertices[22] = Vertex(QVector3D(hsize,  -hsize, hsize) + _position, QVector2D(1, 1), _colors[5]);
    _vertices[23] = Vertex(QVector3D(-hsize,  -hsize, hsize) + _position, QVector2D(0, 1), _colors[5]);

    for(int i = 0; i < CUBE_VERTICES_COUNT; i++)
    {
        _verticesByFace[i / 4][i % 4] = _vertices[i];
    }
}

const Vertex* Cube::vertices(Face face) const
{
    switch(face)
    {
    case Face::Front:
        return _verticesByFace[0];
        break;
    case Face::Back:
        return _verticesByFace[1];
        break;
    case Face::Left:
        return _verticesByFace[2];
        break;
    case Face::Right:
        return _verticesByFace[3];
        break;
    case Face::Up:
        return _verticesByFace[4];
        break;
    case Face::Down:
        return _verticesByFace[5];
        break;
    }
    return _vertices;
}
