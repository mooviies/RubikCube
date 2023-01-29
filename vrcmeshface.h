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

#ifndef VRCMESHFACE_H
#define VRCMESHFACE_H

#include "vertex.h"
#include "vrcface.h"

class VRCMeshCube
{
public:
    enum class Color
    {
        Green  = 0x53D151,
        Red    = 0xDB2617,
        Yellow = 0xF2F70C,
        White  = 0xFFFFFF,
        Blue   = 0x026CDB,
        Orange = 0xFF8E07,
        Black  = 0x000000,
        Error  = 0xFF0DFF
    };

    VRCMeshCube();
    VRCMeshCube(QVector3D position, float size);

    int size() const { return _size; }

    void setPosition(const QVector3D& position);
    void setSize(float size);
    void setColor(VRCFace::Side side, Color color);
    void setID(int id);

    void generate();

    const Vertex* vertices() const { return _vertices; }
    const Vertex* vertices(VRCFace::Side face) const;
    int verticesSize() { return sizeof(_vertices); }

private:
    QVector3D _position;
    float _size;
    Vertex _vertices[24];
    Vertex _verticesByFace[6][4];
    Color _colors[6];
    int _id;
};

#endif // VRCMESHFACE_H
