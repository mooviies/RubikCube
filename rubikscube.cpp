#include "rubikscube.h"

#include <QOpenGLFunctions>
#include <QColor>
#include <stdexcept>

#include "cube.h"

RubiksCube::RubiksCube(int size)
{
    if(size < MIN_SIZE)
        _size = MIN_SIZE;
    else if(size > MAX_SIZE)
        _size = MAX_SIZE;
    else
        _size = size;

    _shaderProgram = nullptr;
}

RubiksCube::~RubiksCube()
{
    clean();
}

void RubiksCube::init()
{
    _alternateFace.clear();
    for(int i = 0; i < NUMBER_SIDE; i++)
        _alternateFace.insert((Face)i, (Face)i);

    _colorByFace.clear();
    _colorByFace[Face::Back] = Color::Blue;
    _colorByFace[Face::Front] = Color::Green;
    _colorByFace[Face::Left] = Color::Orange;
    _colorByFace[Face::Right] = Color::Red;
    _colorByFace[Face::Up] = Color::White;
    _colorByFace[Face::Down] = Color::Yellow;

    _currentRotation = 0;
    _targetRotation = 0;
    _global.setToIdentity();
    _local.setToIdentity();
    _isAnimating = false;
    _sizeOfVertices = 0;
    _shaderProgram = new QOpenGLShaderProgram();
    _shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    _shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
    _shaderProgram->link();
    _projectionMatrixID = _shaderProgram->uniformLocation("projection");
    _cameraMatrixID = _shaderProgram->uniformLocation("camera");
    _rotationMatrixID = _shaderProgram->uniformLocation("rotation");
    _borderWidthMatrixID = _shaderProgram->uniformLocation("borderWidth");
    create();
}

void RubiksCube::reset()
{
    clean();
    init();
}

bool RubiksCube::rotate(Layer layer, Rotation rotation, int layerPos, bool wide)
{
    if(_isAnimating)
        return false;

    int centerLayers = Layer::Equator | Layer::Middle | Layer::Horizontal | Layer::Standing;

    if(_size % 2 == 0 && (layer & centerLayers))
        return false;

    if(layerPos <= 0 || layerPos > _size / 2)
        return false;

    _buffer.bind();

    if(layer & centerLayers)
    {
        int cl = _size / 2 + 1;
        switch(layer)
        {
        case Layer::Equator:
            rotateLayer(Face::Down, cl, rotation);
            break;
        case Layer::Horizontal:
            rotateLayer(Face::Up, cl, rotation);
            break;
        case Layer::Middle:
            break;
        case Layer::Standing:
            break;
        default:
            break;
        }
    }
    else
    {
        for(int i = 0; i < NUMBER_SIDE; i++)
        {
            if(layer & 1 << i)
            {
                if((wide || layerPos == 1))
                {
                    rotateFace((Face)i, rotation);
                }

                for(int j = wide ? 1: layerPos; j <= layerPos; j++)
                {
                    rotateLayer((Face)i, j, rotation);
                }
                break;
            }
        }
    }

    _buffer.release();

    return true;
}

void RubiksCube::rotateFace(Face faceID, Rotation rotation)
{
    faceID = _alternateFace[faceID];
    Face **face = _cube[(int)faceID];

    Face** buffer = new Face*[_size];
    for(int i = 0; i < _size; i++)
    {
        buffer[i] = new Face[_size];
        for(int j = 0; j < _size; j++)
        {
            buffer[i][j] = face[i][j];
        }
    }

    for(int i = 0; i < _size; i++)
    {
        for(int j = 0; j < _size; j++)
        {
            switch(rotation)
            {
            case Rotation::Clockwise:
                face[i][j] = buffer[_size - j - 1][i];
                break;
            case Rotation::CounterClockwise:
                face[i][j] = buffer[j][_size - i - 1];
                break;
            case Rotation::Turn180:
                face[i][j] = buffer[_size - i - 1][_size - j - 1];
                break;
            }
            setColor(faceID, i, j, _colorByFace[face[i][j]]);
        }
        delete[] buffer[i];
    }
    delete[] buffer;
}

void RubiksCube::rotateLayer(Face faceID, int layer, Rotation rotation)
{
    bool isFaceUp = faceID == Face::Up;
    if(isFaceUp || faceID == Face::Down)
    {
        int j = isFaceUp ? _size - layer : layer - 1;
        bool clockwiseUpView = (rotation == Rotation::Clockwise && isFaceUp) || (rotation == Rotation::CounterClockwise && !isFaceUp);
        bool counterClockwiseUpView = (rotation == Rotation::CounterClockwise && isFaceUp) || (rotation == Rotation::Clockwise && !isFaceUp);

        Face** front = _cube[(int)Face::Front];
        Face** left = _cube[(int)Face::Left];
        Face** back = _cube[(int)Face::Back];
        Face** right = _cube[(int)Face::Right];

        for(int i = 0; i < _size; i++)
        {
            Face buffer = front[i][j];
            if(clockwiseUpView)
            {
                front[i][j] = right[i][j];
                right[i][j] = back[i][j];
                back[i][j] = left[i][j];
                left[i][j] = buffer;
            }
            else if(counterClockwiseUpView)
            {
                front[i][j] = left[i][j];
                left[i][j] = back[i][j];
                back[i][j] = right[i][j];
                right[i][j] = buffer;
            }
            else
            {
                front[i][j] = back[i][j];
                back[i][j] = buffer;

                Face buffer2 = left[i][j];
                left[i][j] = right[i][j];
                right[i][j] = buffer2;
            }
            setColor(Face::Front, i, j, _colorByFace[front[i][j]]);
            setColor(Face::Right, i, j, _colorByFace[right[i][j]]);
            setColor(Face::Back, i, j, _colorByFace[back[i][j]]);
            setColor(Face::Left, i, j, _colorByFace[left[i][j]]);
        }

        return;
    }

    bool isFaceLeft = faceID == Face::Left;
    if(isFaceLeft || faceID == Face::Right)
    {
        int i = isFaceLeft ? layer - 1: _size - layer;
        bool clockwiseUpView = (rotation == Rotation::Clockwise && isFaceLeft) || (rotation == Rotation::CounterClockwise && !isFaceLeft);
        bool counterClockwiseUpView = (rotation == Rotation::CounterClockwise && isFaceLeft) || (rotation == Rotation::Clockwise && !isFaceLeft);

        Face** up = _cube[(int)Face::Up];
        Face** front = _cube[(int)Face::Front];
        Face** down = _cube[(int)Face::Down];
        Face** back = _cube[(int)Face::Back];

        for(int j = 0; j < _size; j++)
        {
            Face buffer = up[i][j];
            if(clockwiseUpView)
            {
                up[i][j] = back[_size - i - 1][_size - j - 1];
                back[_size - i - 1][_size - j - 1] = down[i][j];
                down[i][j] = front[i][j];
                front[i][j] = buffer;
            }
            else if(counterClockwiseUpView)
            {
                up[i][j] = front[i][j];
                front[i][j] = down[i][j];
                down[i][j] = back[_size - i - 1][_size - j - 1];
                back[_size - i - 1][_size - j - 1] = buffer;
            }
            else
            {
                Face buffer2 = back[_size - i - 1][_size - j - 1];
                up[i][j] = down[i][j];
                down[i][j] = buffer;
                back[_size - i - 1][_size - j - 1] = front[i][j];
                front[i][j] = buffer2;
            }
            setColor(Face::Front, i, j, _colorByFace[front[i][j]]);
            setColor(Face::Up, i, j, _colorByFace[up[i][j]]);
            setColor(Face::Back, _size - i - 1, _size - j - 1, _colorByFace[back[_size - i - 1][_size - j - 1]]);
            setColor(Face::Down, i, j, _colorByFace[down[i][j]]);
        }
        return;
    }

    bool isFaceFront = faceID == Face::Front;
    if(isFaceFront || faceID == Face::Back)
    {
        int i = isFaceFront ? layer - 1: _size - layer;
        bool clockwiseUpView = (rotation == Rotation::Clockwise && isFaceFront) || (rotation == Rotation::CounterClockwise && !isFaceFront);
        bool counterClockwiseUpView = (rotation == Rotation::CounterClockwise && isFaceFront) || (rotation == Rotation::Clockwise && !isFaceFront);

        Face** up = _cube[(int)Face::Up];
        Face** right = _cube[(int)Face::Right];
        Face** down = _cube[(int)Face::Down];
        Face** left = _cube[(int)Face::Left];

        for(int j = 0; j < _size; j++)
        {
            Face buffer = up[_size - j - 1][i];
            if(clockwiseUpView)
            {
                up[_size - j - 1][i] = left[_size - i - 1][_size - j - 1];
                left[_size - i - 1][_size - j - 1] = down[j][_size - i - 1];
                down[j][_size - i - 1] = right[i][j];
                right[i][j] = buffer;
            }
            else if(counterClockwiseUpView)
            {
                up[_size - j - 1][i] = right[i][j];
                right[i][j] = down[j][_size - i - 1];
                down[j][_size - i - 1] = left[_size - i - 1][_size - j - 1];
                left[_size - i - 1][_size - j - 1] = buffer;
            }
            else
            {
                Face buffer2 = left[_size - i - 1][_size - j - 1];
                up[_size - j - 1][i] = down[j][_size - i - 1];
                down[j][_size - i - 1] = buffer;
                left[_size - i - 1][_size - j - 1] = right[i][j];
                right[i][j] = buffer2;
            }
            setColor(Face::Up, _size - j - 1, i, _colorByFace[up[_size - j - 1][i]]);
            setColor(Face::Left, _size - i - 1, _size - j - 1, _colorByFace[left[_size - i - 1][_size - j - 1]]);
            setColor(Face::Down, j, _size - i - 1, _colorByFace[down[j][_size - i - 1]]);
            setColor(Face::Right, i, j, _colorByFace[right[i][j]]);
        }
        return;
    }
}

void RubiksCube::display(QOpenGLFunctions *f, const QMatrix4x4 &projection, const QMatrix4x4 &camera)
{
    _shaderProgram->bind();
    _vao.bind();

    _shaderProgram->setUniformValue(_projectionMatrixID, projection);
    _shaderProgram->setUniformValue(_cameraMatrixID, camera);
    _shaderProgram->setUniformValue(_rotationMatrixID, _global);
    _shaderProgram->setUniformValue(_borderWidthMatrixID, 0.015f);

    f->glDrawArrays(GL_QUADS, 0, _sizeOfVertices / sizeof(_vertices[0]));

    _vao.release();
    _shaderProgram->release();
}

void RubiksCube::create()
{
    createModel();
    buildBuffer();
}

void RubiksCube::createModel()
{
    int nbVertices = NUMBER_SIDE * _size * _size * SQUARE_VERTICES_COUNT;
    _vertices = new Vertex[nbVertices];
    _sizeOfVertices = sizeof(Vertex) * nbVertices;
    _width = getWidth(_size);
    _cellWidth = _width / float(_size);
    float halfCellWidth = _cellWidth / 2.0;
    float halfWidth = _width / 2.0;
    float widthLess = halfWidth - halfCellWidth;

    Cube cube;
    cube.setSize(_cellWidth);

    _cube = new Face**[NUMBER_SIDE];
    for(int i = 0; i < NUMBER_SIDE; i++)
    {
        Face face = (Face)(i);
        cube.setColor(face, _colorByFace[face]);

        float x, y, z;
        float xi = 0, yi = 0, zi = 0;
        switch(face)
        {
        case Face::Front:
            x = -widthLess;
            y = -widthLess;
            z = widthLess;
            xi = _cellWidth;
            yi = _cellWidth;
            break;
        case Face::Back:
            x = widthLess;
            y = -widthLess;
            z = -widthLess;
            xi = -_cellWidth;
            yi = _cellWidth;
            break;
        case Face::Left:
            x = -widthLess;
            y = -widthLess;
            z = -widthLess;
            yi = _cellWidth;
            zi = _cellWidth;
            break;
        case Face::Right:
            x = widthLess;
            y = -widthLess;
            z = widthLess;
            yi = _cellWidth;
            zi = -_cellWidth;
            break;
        case Face::Up:
            x = -widthLess;
            y = widthLess;
            z = widthLess;
            xi = _cellWidth;
            zi = -_cellWidth;
            break;
        case Face::Down:
            x = -widthLess;
            y = -widthLess;
            z = -widthLess;
            xi = _cellWidth;
            zi = _cellWidth;
            break;
        }

        _cube[i] = new Face*[_size];
        for(int j = 0; j < _size; j++)
        {
            _cube[i][j] = new Face[_size];
            for(int k = 0; k < _size; k++)
            {
                if(xi == 0)
                    cube.setPosition(QVector3D(x, y + yi * k, z + zi * j));
                else if(yi == 0)
                    cube.setPosition(QVector3D(x + xi * j, y, z + zi * k));
                else if(zi == 0)
                    cube.setPosition(QVector3D(x + xi * j, y + yi * k, z));

                int id = getID(i, j, k);
                cube.setID(id);
                cube.generate();

                const Vertex* faceVertices = cube.vertices(face);
                for(int m = 0; m < SQUARE_VERTICES_COUNT; m++)
                {
                    _vertices[m + id * SQUARE_VERTICES_COUNT] = faceVertices[m];
                }

                _cube[i][j][k] = face;
            }
        }
    }
}

void RubiksCube::buildBuffer()
{
    // Create Shader (Do not release until VAO is created)
    _shaderProgram->bind();

    // Create Buffer (Do not release until VAO is created)
    _buffer.create();
    _buffer.bind();
    _buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    _buffer.allocate(_vertices, _sizeOfVertices);

    // Create Vertex Array Object
    _vao.create();
    _vao.bind();
    _shaderProgram->enableAttributeArray(0);
    _shaderProgram->enableAttributeArray(1);
    _shaderProgram->enableAttributeArray(2);
    _shaderProgram->enableAttributeArray(3);
    _shaderProgram->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    _shaderProgram->setAttributeBuffer(1, GL_FLOAT, Vertex::uvOffset(), Vertex::UVTupleSize, Vertex::stride());
    _shaderProgram->setAttributeBuffer(2, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());
    _shaderProgram->setAttributeBuffer(3, GL_INT, Vertex::idOffset(), Vertex::IDTupleSize, Vertex::stride());

    // Release (unbind) all
    _vao.release();
    _buffer.release();
    _shaderProgram->release();
}

void RubiksCube::clean()
{
    for(int i = 0; i < NUMBER_SIDE; i++)
    {
        for(int j = 0; j < _size; j++)
        {
            delete[] _cube[i][j];
        }
        delete[] _cube[i];
    }
    delete[] _cube;
    delete[] _vertices;
    _buffer.destroy();
    _vao.destroy();
    delete _shaderProgram;
    _shaderProgram = nullptr;
}

void RubiksCube::setColor(Face face, int i, int j, Color color)
{
    QColor c(color);
    QVector3D cv(c.redF(), c.greenF(), c.blueF());
    int count = sizeof(cv);

    int baseOffset = Vertex::stride() * getID((int)face, i, j) * SQUARE_VERTICES_COUNT;
    _buffer.write(baseOffset + Vertex::colorOffset(), &cv, count);
    baseOffset += Vertex::stride();

    _buffer.write(baseOffset + Vertex::colorOffset(), &cv, count);
    baseOffset += Vertex::stride();

    _buffer.write(baseOffset + Vertex::colorOffset(), &cv, count);
    baseOffset += Vertex::stride();

    _buffer.write(baseOffset + Vertex::colorOffset(), &cv, count);
}
