#include "rubikscubeview.h"

#include <QOpenGLFunctions>

RubiksCubeView::RubiksCubeView(QWidget *parent) : QOpenGLWidget(parent)
{
    _cube = nullptr;
}

void RubiksCubeView::setCube(const RubiksCube* cube)
{
    _cube = cube;
}

void RubiksCubeView::initializeGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void RubiksCubeView::resizeGL(int w, int h)
{
    _projection.setToIdentity();
    _projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

void RubiksCubeView::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);
}
