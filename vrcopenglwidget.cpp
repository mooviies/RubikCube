#include "vrcopenglwidget.h"

#include <QKeyEvent>

#include "constants.h"

const float VRCOpenGLWidget::BORDER_LIMIT_DISTANCE = 10;

VRCOpenGLWidget::VRCOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    _mouseIsInside = false;
    _mouseIsPressed = false;
    _openGLIsInitialized = false;
    _verticalRotation = 0;
    setMouseTracking(true);
}

VRCOpenGLWidget::~VRCOpenGLWidget()
{
    delete _cubeShaderProgram;
    delete _stripeShaderProgram;
}

void VRCOpenGLWidget::reset()
{
    _camera.setToIdentity();
    _camera.lookAt(QVector3D(0, 0, 1).normalized() * _view->getSize(), QVector3D(), QVector3D(0, 1, 0));
    _world.setToIdentity();
    _world.rotate(-38, 0, 1, 0);
    _camera.rotate(22.5, 1, 0, 0);
}

void VRCOpenGLWidget::setView(VRCView *view)
{
    _view = view;
    _modelView.setToIdentity();
    reset();
    _openGLIsInitialized = false;
}

void VRCOpenGLWidget::initializeGL()
{
    auto *glFunctions = QOpenGLContext::currentContext()->functions();

    glFunctions->glEnable(GL_DEPTH_TEST);
    glFunctions->glEnable(GL_BLEND);
    glFunctions->glEnable(GL_POLYGON_SMOOTH);
    glFunctions->glDepthFunc(GL_LEQUAL);
    glFunctions->glClearColor(0.23, 0.23, 0.23, 1);

    _cubeShaderProgram = new QOpenGLShaderProgram();
    _cubeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    _cubeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");

    _stripeShaderProgram = new QOpenGLShaderProgram();
    _stripeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/border.vert");
    _stripeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/border.frag");

    _cubeShaderProgram->link();
    _stripeShaderProgram->link();

    _view->init(_projection, _camera, _world, _modelView, _cubeShaderProgram, _stripeShaderProgram);

    _openGLIsInitialized = true;
}

void VRCOpenGLWidget::resizeGL(int w, int h)
{
    _aspectRatio = w / float(h);
    _projection.setToIdentity();
    _projection.perspective(45.0f, _aspectRatio, DEPTH_NEAR, DEPTH_FAR);
}

void VRCOpenGLWidget::paintGL()
{
    if(!_openGLIsInitialized)
    {
        _view->init(_projection, _camera, _world, _modelView, _cubeShaderProgram, _stripeShaderProgram);
        _openGLIsInitialized = true;
    }

    QOpenGLContext::currentContext()->functions()->glClear(GL_COLOR_BUFFER_BIT);
    _view->draw();

    update();
}

void VRCOpenGLWidget::enterEvent(QEnterEvent *event)
{
}

void VRCOpenGLWidget::leaveEvent(QEvent *event)
{
    _mouseIsInside = false;
}

void VRCOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    _mouseIsInside = true;
    _mousePosition = event->pos();

    if(_mouseIsPressed) {
        auto widgetRect = this->rect();
        auto maxX = widgetRect.width() - BORDER_LIMIT_DISTANCE;
        auto maxY = widgetRect.height() - BORDER_LIMIT_DISTANCE;
        auto diff = _lastMousePosition - _mousePosition;
        if(diff.isNull())
            return;

        bool mousePositionChanged = false;
        if(_mousePosition.x() > maxX)
        {
            _mousePosition.setX(BORDER_LIMIT_DISTANCE);
            mousePositionChanged = true;
        }

        else if(_mousePosition.x() < BORDER_LIMIT_DISTANCE)
        {
            _mousePosition.setX(maxX);
            mousePositionChanged = true;
        }

        if(_mousePosition.y() > maxY)
        {
            _mousePosition.setY(BORDER_LIMIT_DISTANCE);
            mousePositionChanged = true;
        }
        else if(_mousePosition.y() < BORDER_LIMIT_DISTANCE)
        {
            _mousePosition.setY(maxY);
            mousePositionChanged = true;
        }

        if(mousePositionChanged)
            QCursor::setPos(this->mapToGlobal(_mousePosition));

        _lastMousePosition = _mousePosition;
        updateRotation(diff);
    }
}

void VRCOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    _mouseIsPressed = true;
    _lastMousePosition = event->pos();
}

void VRCOpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _mouseIsPressed = false;
    //_cube->mouseReleaseEvent(event, _projection, _camera);
}

void VRCOpenGLWidget::updateRotation(const QPoint &diff)
{
    float dx = -diff.x();
    float dy = -diff.y();
    float scaleX = dx / this->width();
    float scaleY = dy / this->height();
    float rotSpeed = 350.0f;

    _world.rotate(rotSpeed * scaleX, 0, 1, 0);

    auto vRot = rotSpeed * scaleY;
    if(_verticalRotation + vRot > 40)
        vRot = -(_verticalRotation - 40);
    else if(_verticalRotation + vRot < -75)
        vRot = -(_verticalRotation + 75);

    _verticalRotation += vRot;
    _camera.rotate(vRot, 1, 0, 0);
}

QVector3D VRCOpenGLWidget::getArcballVector(QPoint mouseDiff)
{
    QVector3D result(- 1 * (mouseDiff.x() / 2.0), 1.0 - 2.0 * mouseDiff.y() / this->height(), 0);

    float square = result.x() * result.x() + result.y() * result.y();
    if(square <= 1)
        result.setZ(sqrtf(1 - square));
    else
        result.normalize();

    return result;
}
