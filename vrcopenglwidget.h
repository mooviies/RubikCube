#ifndef VRCOPENGLWIDGET_H
#define VRCOPENGLWIDGET_H

#include <QMainWindow>
#include <QtOpenGLWidgets/QtOpenGLWidgets>
#include <QMatrix4x4>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QtOpenGL/QOpenGLShaderProgram>

#include "vrcview.h"

class VRCOpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    VRCOpenGLWidget(QWidget *parent);
    ~VRCOpenGLWidget();

    void setView(VRCView* view);
    bool mouseIsInside() const { return _mouseIsInside; }
    QPoint mousePosition() const { return _mousePosition; }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    VRCView* _view;

    QMatrix4x4 _camera;
    QMatrix4x4 _projection;

    float _aspectRatio;

    bool _mouseIsInside;
    QPoint _mousePosition;
};

#endif // VRCOPENGLWIDGET_H
