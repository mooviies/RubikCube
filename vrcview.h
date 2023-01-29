#ifndef VRCVIEW_H
#define VRCVIEW_H

#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QMap>
#include <QVector3D>
#include <QMouseEvent>
#include <QQueue>
#include <cmath>

#include "meshopengl.h"
#include "vrcface.h"

class VRCModel;

class VRCView
{
public:
    VRCView(VRCModel *model);

    uint getSize() const { return _size; }

    void init(const QMatrix4x4 &projection, const QMatrix4x4 &camera, const QMatrix4x4 &world, const QMatrix4x4 &model);
    void setModel(VRCModel *model);

    bool isInitialized() const { return _cubeShaderProgram != nullptr; }
    bool isAnimating() const { return _isAnimating; }

    bool rotate(const QList<int>& flagsList, bool fast = false);
    bool rotate(int flags, bool fast = false);

    void update(const VRCAction& lastAction);
    void draw();

private:
    void create(const QMatrix4x4 &projection, const QMatrix4x4 &camera, const QMatrix4x4 &world, const QMatrix4x4 &model);
    static float getWidth(float size) { return 1.6f * log(size) - 0.7f; }
    uint getID(uint i, uint j, uint k) const { return j + k * _size + i * _size * _size; }
    void updateVisualModel();
    void setColor(int offset, VRCFace::Color color);
    void setColor(VRCFace::Side face, int i, int j, VRCFace::Color color);
    std::array<Vertex, 4> getFaceVertices(VRCFace::Side side, uint color, QVector3D position, float cellWidth);
    VRCAction::Layer getFillLayer(VRCAction::Layer layer);
    VRCAction::Layer getRotationFace(VRCAction::Layer layer);

private:
    const static float BORDER_WIDTH;
    uint _size;

    VRCModel* _model;
    VRCAction _animatingAction;
    bool _isAnimating;
    bool _fastMode;

    float _width;
    float _cellWidth;

    int _maxLayer;
    int _sizeOfVertices;

    QMap<VRCFace::Side, VRCFace::Color> _colorBySide;

    QOpenGLShaderProgram *_cubeShaderProgram;
    QOpenGLShaderProgram *_stripeShaderProgram;

    MeshOpenGL *_cubeModel;
    MeshOpenGL *_equatorFillModel;
    MeshOpenGL *_middleFillModel;
    MeshOpenGL *_standingFillModel;

    float _currentRotation;
    float _targetRotation;

    QVector3D _rotationVector;
    QMatrix4x4 _layerRotation;
    QMap<int, VRCFace::Color> _rotating;
    QMatrix4x4 _stripTranslation;
};

#endif // VRCVIEW_H
