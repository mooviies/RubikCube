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

#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QVector2D>

class Vertex
{
public:
    // Constructors
    constexpr Vertex() : _rotating(0) {}
    Vertex(const QVector3D &position, const QVector2D &uv, uint color) : _position(position), _uv(uv), _rotating(0) { setColor(color); }

    // Accessors / Mutators
    inline constexpr const QVector3D& position() const { return _position; }
    inline constexpr const QVector2D& uv() const { return _uv; }
    inline constexpr const QVector3D& color() const { return _color; }
    //inline constexpr float borderwidth() const { return _borderWidth; }
    inline constexpr float rotating() const { return _rotating; }
    inline void setPosition(const QVector3D& position) { _position = position; }
    inline void setUV(const QVector2D& uv) { _uv = uv; }
    inline void setColor(uint color) { _color = QVector3D(float(color >> 16) / 255.0f, float((color >> 8) & 0xFF) / 255.0f, float(color & 0xFF) / 255.0f); }
    //inline void setBorderWidth(float width) { _borderWidth = width; }
    inline void setRotating(float rotating) { _rotating = rotating; }

    // OpenGL Helpers
    static const int POSITION_TUPLE_SIZE = 3;
    static const int UV_TUPLE_SIZE = 2;
    static const int COLOR_TUPLE_SIZE = 3;
    //static const int BorderTupleSize = 1;
    static const int ROTATING_TUPLE_SIZE = 1;

    inline static constexpr int positionOffset() { return offsetof(Vertex, _position); }
    inline static constexpr int uvOffset() { return offsetof(Vertex, _uv); }
    inline static constexpr int colorOffset() { return offsetof(Vertex, _color); }
    //inline static constexpr int borderWidthOffset() { return offsetof(Vertex, _borderWidth); }
    inline static constexpr int rotatingOffset() { return offsetof(Vertex, _rotating); }
    inline static constexpr int stride() { return sizeof(Vertex); }

private:
    QVector3D _position;
    QVector2D _uv;
    QVector3D _color;
    //float _borderWidth;
    float _rotating;
};

// Note: Q_MOVABLE_TYPE means it can be memcpy'd.
Q_DECLARE_TYPEINFO(Vertex, Q_MOVABLE_TYPE);

#endif // VERTEX_H
