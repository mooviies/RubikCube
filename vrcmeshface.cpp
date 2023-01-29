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

#include "vrcmeshface.h"
#include "vertex.h"
#include "constants.h"

typedef VRCFace::Side Side;

VRCMeshCube::VRCMeshCube() : _size(0)
{

}

VRCMeshCube::VRCMeshCube(QVector3D position, float size) : _position(position), _id(0)
{
    setSize(size);
}

void VRCMeshCube::setPosition(const QVector3D& position)
{
    _position = position;
}

void VRCMeshCube::setSize(float size)
{
    _size = size;
}

void VRCMeshCube::setColor(Side side, Color color)
{
    switch(side)
    {
    case Side::Front:
        _colors[0] = color;
        break;
    case Side::Back:
        _colors[1] = color;
        break;
    case Side::Left:
        _colors[2] = color;
        break;
    case Side::Right:
        _colors[3] = color;
        break;
    case Side::Up:
        _colors[4] = color;
        break;
    case Side::Down:
        _colors[5] = color;
        break;
    }
}

void VRCMeshCube::setID(int id)
{
    _id = id;
}

void VRCMeshCube::generate()
{
    float hsize = _size / 2.0;

    // Front
    _vertices[0] = Vertex(QVector3D(-hsize,  -hsize, hsize) + _position, QVector2D(0, 0), (uint)_colors[0]);
    _vertices[1] = Vertex(QVector3D(hsize,  -hsize, hsize) + _position, QVector2D(1, 0), (uint)_colors[0]);
    _vertices[2] = Vertex(QVector3D(hsize,  hsize, hsize) + _position, QVector2D(1, 1), (uint)_colors[0]);
    _vertices[3] = Vertex(QVector3D(-hsize,  hsize, hsize) + _position, QVector2D(0, 1), (uint)_colors[0]);

    // Back
    _vertices[4] = Vertex(QVector3D(hsize,  -hsize, -hsize) + _position, QVector2D(0, 0), (uint)_colors[1]);
    _vertices[5] = Vertex(QVector3D(-hsize,  -hsize, -hsize) + _position, QVector2D(1, 0), (uint)_colors[1]);
    _vertices[6] = Vertex(QVector3D(-hsize,  hsize, -hsize) + _position, QVector2D(1, 1), (uint)_colors[1]);
    _vertices[7] = Vertex(QVector3D(hsize,  hsize, -hsize) + _position, QVector2D(0, 1), (uint)_colors[1]);

    // Left
    _vertices[8] = Vertex(QVector3D(-hsize,  -hsize, -hsize) + _position, QVector2D(0, 0), (uint)_colors[2]);
    _vertices[9] = Vertex(QVector3D(-hsize,  -hsize, hsize) + _position, QVector2D(1, 0), (uint)_colors[2]);
    _vertices[10] = Vertex(QVector3D(-hsize,  hsize, hsize) + _position, QVector2D(1, 1), (uint)_colors[2]);
    _vertices[11] = Vertex(QVector3D(-hsize,  hsize, -hsize) + _position, QVector2D(0, 1), (uint)_colors[2]);

    // Right
    _vertices[12] = Vertex(QVector3D(hsize,  -hsize, hsize) + _position, QVector2D(0, 0), (uint)_colors[3]);
    _vertices[13] = Vertex(QVector3D(hsize,  -hsize, -hsize) + _position, QVector2D(1, 0), (uint)_colors[3]);
    _vertices[14] = Vertex(QVector3D(hsize,  hsize, -hsize) + _position, QVector2D(1, 1), (uint)_colors[3]);
    _vertices[15] = Vertex(QVector3D(hsize,  hsize, hsize) + _position, QVector2D(0, 1), (uint)_colors[3]);

    // Up
    _vertices[16] = Vertex(QVector3D(-hsize,  hsize, hsize) + _position, QVector2D(0, 0), (uint)_colors[4]);
    _vertices[17] = Vertex(QVector3D(hsize,  hsize, hsize) + _position, QVector2D(1, 0), (uint)_colors[4]);
    _vertices[18] = Vertex(QVector3D(hsize,  hsize, -hsize) + _position, QVector2D(1, 1), (uint)_colors[4]);
    _vertices[19] = Vertex(QVector3D(-hsize,  hsize, -hsize) + _position, QVector2D(0, 1), (uint)_colors[4]);

    // Down
    _vertices[20] = Vertex(QVector3D(-hsize,  -hsize, -hsize) + _position, QVector2D(0, 0), (uint)_colors[5]);
    _vertices[21] = Vertex(QVector3D(hsize,  -hsize, -hsize) + _position, QVector2D(1, 0), (uint)_colors[5]);
    _vertices[22] = Vertex(QVector3D(hsize,  -hsize, hsize) + _position, QVector2D(1, 1), (uint)_colors[5]);
    _vertices[23] = Vertex(QVector3D(-hsize,  -hsize, hsize) + _position, QVector2D(0, 1), (uint)_colors[5]);

    for(int i = 0; i < CUBE_VERTICES_COUNT; i++)
    {
        _verticesByFace[i / 4][i % 4] = _vertices[i];
    }
}

const Vertex* VRCMeshCube::vertices(VRCFace::Side side) const
{
    switch(side)
    {
    case Side::Front:
        return _verticesByFace[0];
    case Side::Back:
        return _verticesByFace[1];
    case Side::Left:
        return _verticesByFace[2];
    case Side::Right:
        return _verticesByFace[3];
    case Side::Up:
        return _verticesByFace[4];
    case Side::Down:
        return _verticesByFace[5];
    }
    return _vertices;
}
