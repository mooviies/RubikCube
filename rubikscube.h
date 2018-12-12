#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QMap>
#include <QVector3D>
#include <cmath>

#include "constants.h"
#include "vertex.h"

class RubiksCube
{
public:
    RubiksCube(int size);
    ~RubiksCube();

    int maxLayer() const { return _maxLayer; }
    int size() const { return _size; }
    float width() const { return _width; }
    inline float getID(int i, int j, int k) const { return j + k * _size + i * _size * _size; }

    inline bool isInitialized() const { return _shaderProgram != nullptr; }

    void init();
    void reset();
    bool rotate(const QList<int>& flagsList, bool fast = false);
    bool rotate(int flags, bool fast = false);
    void display(QOpenGLFunctions *f, const QMatrix4x4& projection, const QMatrix4x4& camera);

protected:
    void rotateFace(Face faceID, int flags);
    void rotateLayer(Face faceID, int flags);
    void completeRotation();

private:
    void create();
    void createModel();
    void buildBuffer();
    void clean();

    inline static float getWidth(float size) { return 1.6f * log(size) - 0.7f; }

    void setColor(int offset, Color color);
    void setColor(Face face, int i, int j, Color color);

private:
    Face ***_cube;
    Vertex *_vertices;

    QList<int> _commands;
    int _currentCommand;

    bool _isAnimating;
    bool _fastMode;

    float _width;
    float _cellWidth;

    int _maxLayer;
    int _size;
    int _sizeOfVertices;

    QMap<Face, Face> _alternateFace;
    QList<Color> _colorByFace;
    QMap<RotationComponent, int> _insideOffset;
    QMap<RotationComponent, int> _insidePosition;

    QOpenGLShaderProgram *_shaderProgram;
    QOpenGLBuffer _buffer;
    QOpenGLVertexArrayObject _vao;

    float _currentRotation;
    float _targetRotation;

    QVector3D _rotationVector;
    QMatrix4x4 _layerRotation;
    QMap<int, Color> _rotating;

    int _projectionMatrixID;
    int _cameraMatrixID;
    int _rotationMatrixID;
    int _borderWidthMatrixID;
};

#endif // RUBIKSCUBE_H
