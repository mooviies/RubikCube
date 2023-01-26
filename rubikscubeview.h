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
#include <QtOpenGLWidgets/QtOpenGLWidgets>
#include <QMatrix4x4>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QtOpenGL/QOpenGLShaderProgram>

#include "rubikscube.h"

class RubiksCubeView : public QOpenGLWidget
{
    Q_OBJECT
public:
    RubiksCubeView(QWidget *parent);
    ~RubiksCubeView();

    void setCube(RubiksCube* cube);
    bool mouseIsInside() const { return _mouseIsInside; }
    QPoint mousePosition() const { return _mousePosition; }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void clean();

private:
    RubiksCube* _cube;

    QMatrix4x4 _camera;
    QMatrix4x4 _projection;

    float _aspectRatio;

    bool _mouseIsInside;
    QPoint _mousePosition;
};

#endif // RUBIKCUBEVIEW_H
