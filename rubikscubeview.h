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

#ifndef RUBIKCUBEVIEW_H
#define RUBIKCUBEVIEW_H

#include <QMainWindow>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include "rubikscube.h"

class RubiksCubeView : public QOpenGLWidget
{
    Q_OBJECT
public:
    RubiksCubeView(QWidget *parent);
    ~RubiksCubeView();

    void setCube(RubiksCube* cube);
    void keyPressEvent(QKeyEvent *event) override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void clean();

private:
    RubiksCube* _cube;

    QMatrix4x4 _camera;
    QMatrix4x4 _projection;

    float _aspectRatio;
};

#endif // RUBIKCUBEVIEW_H
