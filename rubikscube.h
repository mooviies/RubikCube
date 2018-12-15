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

#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QMap>
#include <QVector3D>
#include <QMouseEvent>
#include <cmath>

#include "constants.h"
#include "vertex.h"
#include "opengl3dmodel.h"

class RubiksCubeView;

class RubiksCube
{
public:
    RubiksCube(RubiksCubeView* parent, int size);
    ~RubiksCube();

    int maxLayer() const { return _maxLayer; }
    int size() const { return _size; }
    float width() const { return _width; }
    inline float getID(int i, int j, int k) const { return j + k * _size + i * _size * _size; }

    inline bool isInitialized() const { return _cubeShaderProgram != nullptr; }

    void init(const QMatrix4x4 &camera, const QMatrix4x4 &projection);
    void reset(const QMatrix4x4 &camera, const QMatrix4x4 &projection);
    bool rotate(const QList<int>& flagsList, bool fast = false);
    bool rotate(int flags, bool fast = false);
    void draw();
    void mouseReleaseEvent(QMouseEvent* event, const QMatrix4x4& projection, const QMatrix4x4& camera);

protected:
    void rotateFace(Face faceID, int flags);
    void rotateLayer(Face faceID, int flags);
    void completeRotation();

private:
    void create(const QMatrix4x4 &camera, const QMatrix4x4 &projection);
    void clean();

    inline static float getWidth(float size) { return 1.6f * log(size) - 0.7f; }

    void setColor(int offset, Color color);
    void setColor(Face face, int i, int j, Color color);

    QVector3D getHitPoint(const QVector3D& n, const QVector3D& p0, const QVector3D& l0, const QVector3D& ray);

private:
    RubiksCubeView* _parent;
    Face ***_cube;
    float _borderWidth;

    QList<int> _commands;
    int _currentCommand;

    int _rotationFlags;
    bool _isAnimating;
    bool _fastMode;

    float _width;
    float _cellWidth;

    int _maxLayer;
    int _size;
    int _sizeOfVertices;

    QMap<Face, Face> _alternateFace;
    QList<Color> _colorByFace;
    QMap<RotationComponent, int> _insideOffset;
    QMap<RotationComponent, int> _insidePosition;

    QOpenGLShaderProgram *_cubeShaderProgram;
    QOpenGLShaderProgram *_stripeShaderProgram;
    QOpenGLShaderProgram *_debugShaderProgram;

    OpenGL3DModel *_cubeModel;
    OpenGL3DModel *_equatorFillModel;
    OpenGL3DModel *_middleFillModel;
    OpenGL3DModel *_standingFillModel;
    OpenGL3DModel *_debugModel;

    float _currentRotation;
    float _targetRotation;

    QVector3D _rotationVector;
    QMatrix4x4 _layerRotation;
    QMap<int, Color> _rotating;
    QMatrix4x4 _stripTranslation;

    QMatrix4x4 _debugCubeTranslation;
};

#endif // RUBIKSCUBE_H
