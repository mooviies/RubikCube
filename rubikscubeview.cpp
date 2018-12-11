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
    switch(event->key())
    {
    case Qt::Key_D:
        _camera.rotate(5, 0, 1, 0);
        break;
    case Qt::Key_A:
        _camera.rotate(-5, 0, 1, 0);
        break;
    case Qt::Key_W:
        _camera.rotate(5, 1, 0, 0);
        break;
    case Qt::Key_S:
        _camera.rotate(-5, 1, 0, 0);
        break;
    case Qt::Key_E:
        _cube->rotate(Layer::Up, Rotation::Clockwise);
        break;
    case Qt::Key_R:
        _cube->rotate(Layer::Up, Rotation::Clockwise);
        break;
    case Qt::Key_T:
        _cube->rotate(Layer::Left, Rotation::Clockwise);
        break;
    case Qt::Key_Y:
        _cube->rotate(Layer::Right, Rotation::Clockwise);
        break;
    }
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

    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
            _camera.translate(0, 0, -float(_cube->width()) * (3));
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
