#include "vrcopenglwidget.h"

#include <QKeyEvent>

#include "constants.h"

VRCOpenGLWidget::VRCOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    _mouseIsInside = false;
    setMouseTracking(true);
}

VRCOpenGLWidget::~VRCOpenGLWidget()
{

}

void VRCOpenGLWidget::setView(VRCView *view)
{
    _view = view;
    _camera.setToIdentity();
    _camera.translate(0, 0, -float(_view->getSize()));
    _camera.rotate(-40, 0, 1, 0);
    _camera.rotate(-22.5, -0.8, 0, 0.8);
}

void VRCOpenGLWidget::initializeGL()
{
    auto *glFunctions = QOpenGLContext::currentContext()->functions();

    glFunctions->glEnable(GL_DEPTH_TEST);
    glFunctions->glEnable(GL_BLEND);
    glFunctions->glEnable(GL_POLYGON_SMOOTH);
    glFunctions->glDepthFunc(GL_LEQUAL);
    glFunctions->glClearColor(0.23, 0.23, 0.23, 1);

    _view->init(_camera, _projection);
}

void VRCOpenGLWidget::resizeGL(int w, int h)
{
    _aspectRatio = w / float(h);
    _projection.setToIdentity();
    _projection.perspective(45.0f, _aspectRatio, DEPTH_NEAR, DEPTH_FAR);
}

void VRCOpenGLWidget::paintGL()
{
    if(_view == nullptr) return;

    QOpenGLContext::currentContext()->functions()->glClear(GL_COLOR_BUFFER_BIT);
    _view->draw();

    update();
}

void VRCOpenGLWidget::enterEvent(QEnterEvent *event)
{
}

void VRCOpenGLWidget::leaveEvent(QEvent *event)
{
    //_mouseIsInside = false;
}

void VRCOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    //_mouseIsInside = true;
    //_mousePosition = event->pos();
}

void VRCOpenGLWidget::mousePressEvent(QMouseEvent *event)
{

}

void VRCOpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //_cube->mouseReleaseEvent(event, _projection, _camera);
}
