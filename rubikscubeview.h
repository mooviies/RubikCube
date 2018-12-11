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

    void setCube(RubiksCube* cube);
    void keyPressEvent(QKeyEvent *event) override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void clean();

private:
    RubiksCube* _cube;

    QMatrix4x4 _camera;
    QMatrix4x4 _projection;

    float _aspectRatio;
};

#endif // RUBIKCUBEVIEW_H
