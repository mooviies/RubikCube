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
}

RubiksCubeView::~RubiksCubeView()
{
    clean();
}

void RubiksCubeView::setCube(RubiksCube *cube)
{
    _cube = cube;
}

void RubiksCubeView::keyPressEvent(QKeyEvent *event)
{
}

void RubiksCubeView::initializeGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_BLEND);
    //f->glEnable(GL_MULTISAMPLE);
    f->glEnable(GL_POLYGON_SMOOTH);

    //f->glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    f->glDepthFunc(GL_LESS);

    f->glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    f->glClearColor(0.95f, 0.95f, 0.99f, 1.0f);
    f->glClearDepthf(2000.0f);
}

void RubiksCubeView::resizeGL(int w, int h)
{
    _aspectRatio = w / float(h);
    _projection.setToIdentity();
    _projection.perspective(45.0f, _aspectRatio, 0.01f, 100.0f);
}

void RubiksCubeView::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    if(_cube != nullptr)
    {
        if(!_cube->isInitialized())
        {
            _cube->init();
            _camera.setToIdentity();
            _camera.translate(0, 0, -float(_cube->width()) * (2.8));
            _camera.rotate(45, 0, 1, 0);
            _camera.rotate(25, 1, 0, 1);
        }
    }

    f->glClear(GL_COLOR_BUFFER_BIT);
    _cube->display(f, _projection, _camera);

    update();
}

void RubiksCubeView::clean()
{

}
