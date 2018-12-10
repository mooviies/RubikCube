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

void RubiksCubeView::setCube(const RubiksCube* cube)
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

    _camera.setToIdentity();
    _camera.translate(0, 0, -float(_cube->width()) * (3));
    _camera.rotate(45, 0, 1, 0);
    _camera.rotate(25, 1, 0, 1);

    // Create Shader (Do not release until VAO is created)
    _shaderProgram = new QOpenGLShaderProgram();
    _shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    _shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
    _shaderProgram->link();
    _shaderProgram->bind();

    // Create Buffer (Do not release until VAO is created)
    _buffer.create();
    _buffer.bind();
    _buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _buffer.allocate(_cube->vertices(), _cube->getSizeOfVertices());
    // Create Vertex Array Object
    _vao.create();
    _vao.bind();
    _shaderProgram->enableAttributeArray(0);
    _shaderProgram->enableAttributeArray(1);
    _shaderProgram->enableAttributeArray(2);
    _shaderProgram->enableAttributeArray(3);
    _shaderProgram->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    _shaderProgram->setAttributeBuffer(1, GL_FLOAT, Vertex::uvOffset(), Vertex::UVTupleSize, Vertex::stride());
    _shaderProgram->setAttributeBuffer(2, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());
    _shaderProgram->setAttributeBuffer(3, GL_INT, Vertex::idOffset(), Vertex::IDTupleSize, Vertex::stride());

    // Release (unbind) all
    _vao.release();
    _buffer.release();
    _shaderProgram->release();

    _projectionMatrixID = _shaderProgram->uniformLocation("projection");
    _cameraMatrixID = _shaderProgram->uniformLocation("camera");
    _borderWidthID = _shaderProgram->uniformLocation("borderWidth");
    _aspectID = _shaderProgram->uniformLocation("aspect");
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

    // Clear
    f->glClear(GL_COLOR_BUFFER_BIT);

    // Render using our shader
    _shaderProgram->bind();
    {
    _vao.bind();

    _shaderProgram->setUniformValue(_projectionMatrixID, _projection);
    _shaderProgram->setUniformValue(_cameraMatrixID, _camera);
    _shaderProgram->setUniformValue(_borderWidthID, 0.015f);
    f->glDrawArrays(GL_QUADS, 0, _cube->getSizeOfVertices() / sizeof(_cube->vertices()[0]));
    _vao.release();
    }
    _shaderProgram->release();
    update();
}

void RubiksCubeView::clean()
{
    _buffer.destroy();
    _vao.destroy();
    delete _shaderProgram;
}
