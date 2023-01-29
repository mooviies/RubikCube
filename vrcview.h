#ifndef VRCVIEW_H
#define VRCVIEW_H

#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QMap>
#include <QVector3D>
#include <QMouseEvent>
#include <cmath>

#include "meshopengl.h"
#include "vrcmeshface.h"

class VRCModel;

class VRCView
{
public:
    VRCView();

    uint getSize() const { return _size; }

    void init(const QMatrix4x4 &camera, const QMatrix4x4 &projection);
    bool isInitialized() const { return _cubeShaderProgram != nullptr; }

    void reset(const QMatrix4x4 &camera, const QMatrix4x4 &projection);
    bool rotate(const QList<int>& flagsList, bool fast = false);
    bool rotate(int flags, bool fast = false);

    void update(const VRCModel& model);
    void draw();

private:
    void create(const QMatrix4x4 &camera, const QMatrix4x4 &projection);
    static float getWidth(float size) { return 1.6f * log(size) - 0.7f; }
    uint getID(uint i, uint j, uint k) const { return j + k * _size + i * _size * _size; }
    void completeRotation();
    void setColor(int offset, VRCMeshCube::Color color);
    void setColor(VRCFace::Side side, int i, int j, VRCMeshCube::Color color);

private:
    const static float BORDER_WIDTH;
    uint _size;

    int _rotationFlags;
    bool _isAnimating;
    bool _fastMode;

    float _width;
    float _cellWidth;

    int _maxLayer;
    int _sizeOfVertices;

    QMap<VRCFace::Side, VRCMeshCube::Color> _colorBySide;

    QOpenGLShaderProgram *_cubeShaderProgram;
    QOpenGLShaderProgram *_stripeShaderProgram;
    QOpenGLShaderProgram *_debugShaderProgram;

    MeshOpenGL *_cubeModel;
    MeshOpenGL *_equatorFillModel;
    MeshOpenGL *_middleFillModel;
    MeshOpenGL *_standingFillModel;
    MeshOpenGL *_debugModel;

    float _currentRotation;
    float _targetRotation;

    QVector3D _rotationVector;
    QMatrix4x4 _layerRotation;
    QMap<int, VRCMeshCube::Color> _rotating;
    QMatrix4x4 _stripTranslation;

    QMatrix4x4 _debugCubeTranslation;
};

#endif // VRCVIEW_H
