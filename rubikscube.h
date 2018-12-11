#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QMap>
#include <cmath>

#include "constants.h"
#include "vertex.h"

class RubiksCube
{
public:
    RubiksCube(int size);
    ~RubiksCube();

    int size() const { return _size; }
    float width() const { return _width; }
    inline float getID(int i, int j, int k) const { return j + k * _size + i * _size * _size; }

    inline bool isInitialized() const { return _shaderProgram != nullptr; }

    void init();
    void reset();
    bool rotate(Layer layer, Rotation rotation, int layerPos = 1, bool wide = true);
    void display(QOpenGLFunctions *f, const QMatrix4x4& projection, const QMatrix4x4& camera);

protected:
    void rotateFace(Face faceID, Rotation rotation);
    void rotateLayer(Face faceID, int layer, Rotation rotation);

private:
    void create();
    void createModel();
    void buildBuffer();
    void clean();

    inline static float getWidth(float size) { return 1.6f * log(size) - 0.7f; }

    void setColor(Face face, int i, int j, Color color);

private:
    Face ***_cube;
    Vertex *_vertices;

    bool _isAnimating;

    float _width;
    float _cellWidth;

    int _size;
    int _sizeOfVertices;

    QMap<Face, Face> _alternateFace;
    QMap<Face, Color> _colorByFace;

    QOpenGLShaderProgram *_shaderProgram;
    QOpenGLBuffer _buffer;
    QOpenGLVertexArrayObject _vao;

    float _currentRotation;
    float _targetRotation;

    QMatrix4x4 _global;
    QMatrix4x4 _local;

    int _projectionMatrixID;
    int _cameraMatrixID;
    int _rotationMatrixID;
    int _borderWidthMatrixID;
};

#endif // RUBIKSCUBE_H
