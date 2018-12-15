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

#include "rubikscubeview.h"

#include <QOpenGLFunctions>
#include <QKeyEvent>

#include "vertex.h"
#include "constants.h"
#include "cube.h"

RubiksCubeView::RubiksCubeView(QWidget *parent) : QOpenGLWidget(parent)
{
    _cube = nullptr;
    _mouseIsInside = false;
    setMouseTracking(true);
}

RubiksCubeView::~RubiksCubeView()
{
    clean();
}

void RubiksCubeView::setCube(RubiksCube *cube)
{
    _cube = cube;
}

void RubiksCubeView::initializeGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_BLEND);
    f->glEnable(GL_POLYGON_SMOOTH);
    f->glDepthFunc(GL_LEQUAL);

    f->glClearColor(0.96f, 0.96f, 1.0f, 1.0f);
}

void RubiksCubeView::resizeGL(int w, int h)
{
    _aspectRatio = w / float(h);
    _projection.setToIdentity();
    _projection.perspective(45.0f, _aspectRatio, DEPTH_NEAR, DEPTH_FAR);
}

void RubiksCubeView::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    if(_cube != nullptr)
    {
        if(!_cube->isInitialized())
        {
            _cube->init(_camera, _projection);
            _camera.setToIdentity();
            _camera.translate(0, 0, -float(_cube->width()) * (2.8));
            _camera.rotate(45, 0, 1, 0);
            _camera.rotate(25, 1, 0, 1);
        }
    }

    f->glClear(GL_COLOR_BUFFER_BIT);
    _cube->draw();

    update();
}

void RubiksCubeView::enterEvent(QEvent *event)
{
}

void RubiksCubeView::leaveEvent(QEvent *event)
{
    _mouseIsInside = false;
}

void RubiksCubeView::mouseMoveEvent(QMouseEvent *event)
{
    _mouseIsInside = true;
    _mousePosition = event->pos();
}

void RubiksCubeView::mousePressEvent(QMouseEvent *event)
{

}

void RubiksCubeView::mouseReleaseEvent(QMouseEvent *event)
{
    _cube->mouseReleaseEvent(event, _projection, _camera);
}

void RubiksCubeView::clean()
{

}
