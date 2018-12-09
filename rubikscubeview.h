#ifndef RUBIKCUBEVIEW_H
#define RUBIKCUBEVIEW_H

#include <QMainWindow>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include "rubikscube.h"

class RubiksCubeView : public QOpenGLWidget
{
    Q_OBJECT
public:
    RubiksCubeView(QWidget *parent);
    ~RubiksCubeView();

    void setCube(const RubiksCube* cube);
    void keyPressEvent(QKeyEvent *event) override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void clean();

private:
    const RubiksCube* _cube;

    QMatrix4x4 _camera;
    QMatrix4x4 _projection;
    QOpenGLBuffer _buffer;
    QOpenGLVertexArrayObject _vao;
    QOpenGLShaderProgram *_shaderProgram;

    float _aspectRatio;

    int _projectionMatrixID;
    int _cameraMatrixID;
    int _borderWidthID;
    int _aspectID;
};

#endif // RUBIKCUBEVIEW_H
