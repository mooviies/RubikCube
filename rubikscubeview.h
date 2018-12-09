#ifndef RUBIKCUBEVIEW_H
#define RUBIKCUBEVIEW_H

#include <QMainWindow>
#include <QOpenGLWidget>
#include <QMatrix4x4>

#include "rubikscube.h"

class RubiksCubeView : public QOpenGLWidget
{
    Q_OBJECT
public:
    RubiksCubeView(QWidget *parent);

    void setCube(const RubiksCube* cube);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QMatrix4x4 _projection;
    const RubiksCube* _cube;
};

#endif // RUBIKCUBEVIEW_H
