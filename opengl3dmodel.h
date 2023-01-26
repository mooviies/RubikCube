#ifndef OPENGL3DMODEL_H
#define OPENGL3DMODEL_H

#include <QMap>
#include <QMatrix4x4>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>

#include "vertex.h"

class OpenGL3DModel
{
public:
    OpenGL3DModel(QOpenGLShaderProgram* shader, const Vertex* vertices, int nbVertices, QOpenGLBuffer::UsagePattern usage, GLenum mode = GL_QUADS);
    ~OpenGL3DModel();

    void addUniform(const QString& name, const QMatrix4x4* m);
    void addUniform(const QString& name, const float* f);

    void setVertices(const Vertex* vertices, int nbVertices);

    inline void bindBuffer() { _buffer.bind(); }
    inline void releaseBuffer() { _buffer.release(); }

    void write(int offset, const void* data, int count);
    void draw();

private:
    void build(QOpenGLBuffer::UsagePattern usage);

private:
    QOpenGLShaderProgram* _shader;

    QMap<int, const QMatrix4x4*> _uniformMatrices;
    QMap<int, const float*> _uniformFloats;

    Vertex* _vertices;
    int _nbVertices;

    QOpenGLBuffer _buffer;
    QOpenGLVertexArrayObject _vao;

    GLenum _mode;
};

#endif // OPENGL3DMODEL_H
