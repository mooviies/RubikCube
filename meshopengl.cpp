#include "meshopengl.h"
#include "qopenglfunctions.h"

MeshOpenGL::MeshOpenGL(QOpenGLShaderProgram *shader, const Vertex* vertices, int nbVertices, QOpenGLBuffer::UsagePattern usage, GLenum mode)
    : _shader(shader), _mode(mode)
{
    setVertices(vertices, nbVertices);
    build(usage);
}

MeshOpenGL::~MeshOpenGL()
{
    _buffer.destroy();
    _vao.destroy();
    delete[] _vertices;
}

void MeshOpenGL::addUniform(const QString& name, const QMatrix4x4* m)
{
    _uniformMatrices[_shader->uniformLocation(name)] = m;
}

void MeshOpenGL::addUniform(const QString& name, const float* f)
{
    _uniformFloats[_shader->uniformLocation(name)] = f;
}

void MeshOpenGL::setVertices(const Vertex* vertices, int nbVertices)
{
    _nbVertices = nbVertices;
    _vertices = new Vertex[nbVertices];
    for(int i = 0; i < nbVertices; i++)
    {
        _vertices[i] = vertices[i];
    }
}


void MeshOpenGL::write(int offset, const void* data, int count)
{
    _buffer.write(offset, data, count);
}

void MeshOpenGL::draw()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    _shader->bind();
    _vao.bind();

    foreach(int key, _uniformMatrices.keys())
    {
        _shader->setUniformValue(key, *_uniformMatrices[key]);
    }

    foreach(int key, _uniformFloats.keys())
    {
        _shader->setUniformValue(key, *_uniformFloats[key]);
    }

    f->glDrawArrays(_mode, 0, _nbVertices);

    _vao.release();
    _shader->release();
}

void MeshOpenGL::build(QOpenGLBuffer::UsagePattern usage)
{
    _buffer.create();
    _vao.create();
    _shader->bind();
    _buffer.bind();
    _vao.bind();

    _buffer.setUsagePattern(usage);
    _buffer.allocate(_vertices, _nbVertices * sizeof(Vertex));

    _shader->enableAttributeArray(0);
    _shader->enableAttributeArray(1);
    _shader->enableAttributeArray(2);
    _shader->enableAttributeArray(3);
    _shader->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::POSITION_TUPLE_SIZE, Vertex::stride());
    _shader->setAttributeBuffer(1, GL_FLOAT, Vertex::uvOffset(), Vertex::UV_TUPLE_SIZE, Vertex::stride());
    _shader->setAttributeBuffer(2, GL_FLOAT, Vertex::colorOffset(), Vertex::COLOR_TUPLE_SIZE, Vertex::stride());
    _shader->setAttributeBuffer(3, GL_FLOAT, Vertex::rotatingOffset(), Vertex::ROTATING_TUPLE_SIZE, Vertex::stride());

    _vao.release();
    _buffer.release();
    _shader->release();
}
