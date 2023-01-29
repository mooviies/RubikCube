#include "vrcview.h"

#include <QOpenGLFunctions>
#include <QColor>
#include <QWidget>
#include <stdexcept>
#include <QGuiApplication>

#include "vertex.h"
#include "constants.h"
#include "vrcmodel.h"

typedef VRCFace::Side Side;
typedef VRCFace::Color Color;

const float VRCView::BORDER_WIDTH = 0.015f;

VRCView::VRCView(const VRCModel &model)
{
    _size = model.getSize();
    _cubeShaderProgram = nullptr;
    _colorBySide.insert(Side::Left, Color::Orange);
    _colorBySide.insert(Side::Front, Color::Green);
    _colorBySide.insert(Side::Right, Color::Red);
    _colorBySide.insert(Side::Back, Color::Blue);
    _colorBySide.insert(Side::Up, Color::White);
    _colorBySide.insert(Side::Down, Color::Yellow);
}

void VRCView::init(const QMatrix4x4 &camera, const QMatrix4x4 &projection)
{
    _currentRotation = 0;
    _targetRotation = 0;
    _layerRotation.setToIdentity();
    _isAnimating = false;
    _sizeOfVertices = 0;

    _cubeShaderProgram = new QOpenGLShaderProgram();
    _cubeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    _cubeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");

    _stripeShaderProgram = new QOpenGLShaderProgram();
    _stripeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/border.vert");
    _stripeShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/border.frag");

    _cubeShaderProgram->link();
    _stripeShaderProgram->link();

    create(camera, projection);
}

void VRCView::update(const VRCModel& model)
{
    _size = model.getSize();
    _cubeModel->bindBuffer();
    for(auto face : model)
    {
        for(uint r = 0; r < _size; r++)
        {
            for(uint c = 0; c < _size; c++)
            {
                setSide(face->getInitialSide(), r, c, face->getSide(r + 1, c + 1));
            }
        }
    }
    _cubeModel->releaseBuffer();
}

void VRCView::draw()
{
    /*if(_isAnimating)
    {
        _currentRotation += ROTATION_SPEED;
        if(_currentRotation >= _targetRotation)
        {
            completeRotation();
        }
        else
        {
            _layerRotation.setToIdentity();
            _layerRotation.rotate(_currentRotation, _rotationVector);
        }

        int nbLayer = (_rotationFlags & RotationComponent::NbLayerMask) >> LAYER_MASK_SHIFT;
        if(nbLayer > 1 && nbLayer < _size && !(_rotationFlags & RotationComponent::Wide))
        {
            int flags = _rotationFlags;
            int rotation = flags & RotationComponent::LayerMask;
            switch(rotation)
            {
            case RotationComponent::Up:
                rotation = RotationComponent::Down;
                nbLayer = _size - nbLayer + 1;
                break;
            case RotationComponent::Right:
                rotation = RotationComponent::Left;
                nbLayer = _size - nbLayer + 1;
                break;
            case RotationComponent::Front:
                rotation = RotationComponent::Back;
                nbLayer = _size - nbLayer + 1;
                break;
            default:
                break;
            }

            switch(rotation)
            {
            case RotationComponent::Equator:
                nbLayer = _size / 2;
                rotation = RotationComponent::Down;
                break;
            case RotationComponent::Middle:
                nbLayer = _size / 2;
                rotation = RotationComponent::Left;
                break;
            case RotationComponent::Standing:
                nbLayer = _size / 2;
                rotation = RotationComponent::Back;
                break;
            }

            _stripTranslation.setToIdentity();

            switch(rotation)
            {
            case RotationComponent::Down:
                _stripTranslation.translate(0, _cellWidth * (nbLayer - 1), 0);
                _equatorFillModel->draw();
                break;
            case RotationComponent::Left:
                _stripTranslation.translate(_cellWidth * (nbLayer - 1), 0, 0);
                _middleFillModel->draw();
                break;
            case RotationComponent::Back:
                _stripTranslation.translate(0, 0, _cellWidth * (nbLayer - 1));
                _standingFillModel->draw();
                break;
            default:
                break;
            }
        }
    }*/

    _cubeModel->draw();
    //_debugModel->draw();

    //if(!_isAnimating && _currentCommand < _commands.size())
    //    rotate(_commands[_currentCommand++], _fastMode);
}

void VRCView::create(const QMatrix4x4 &camera, const QMatrix4x4 &projection)
{
    int nbCubeVertices = NUMBER_SIDE * _size * _size * SQUARE_VERTICES_COUNT;
    int nbVertices = nbCubeVertices;// + nbInsideVertices * 3;
    _sizeOfVertices = sizeof(Vertex) * nbVertices;

    Vertex *vertices = new Vertex[nbVertices];
    _width = getWidth(_size);
    _cellWidth = _width / float(_size);
    float halfCellWidth = _cellWidth / 2.0;
    float halfWidth = _width / 2.0;
    float widthLess = halfWidth - halfCellWidth;

    for(auto i = (uchar)Side::Left; i <= (uchar)Side::Down; i++)
    {
        Side side = (Side)i;

        float x, y, z;
        float xi = 0, yi = 0, zi = 0;
        switch(side)
        {
        case Side::Front:
            x = -widthLess;
            y = -widthLess;
            z = widthLess;
            xi = _cellWidth;
            yi = _cellWidth;
            break;
        case Side::Back:
            x = widthLess;
            y = -widthLess;
            z = -widthLess;
            xi = -_cellWidth;
            yi = _cellWidth;
            break;
        case Side::Left:
            x = -widthLess;
            y = -widthLess;
            z = -widthLess;
            yi = _cellWidth;
            zi = _cellWidth;
            break;
        case Side::Right:
            x = widthLess;
            y = -widthLess;
            z = widthLess;
            yi = _cellWidth;
            zi = -_cellWidth;
            break;
        case Side::Up:
            x = -widthLess;
            y = widthLess;
            z = widthLess;
            xi = _cellWidth;
            zi = -_cellWidth;
            break;
        case Side::Down:
            x = -widthLess;
            y = -widthLess;
            z = -widthLess;
            xi = _cellWidth;
            zi = _cellWidth;
            break;
        }

        for(uint col = 0; col < _size; col++)
        {
            for(uint row = 0; row < _size; row++)
            {
                QVector3D position;
                if(xi == 0)
                    position = QVector3D(x, y + yi * row, z + zi * col);
                else if(yi == 0)
                    position = QVector3D(x + xi * col, y, z + zi * row);
                else
                    position = QVector3D(x + xi * col, y + yi * row, z);

                auto faceVertices = getFaceVertices(side, (uint)_colorBySide[side], position, _cellWidth);
                for(int m = 0; m < SQUARE_VERTICES_COUNT; m++)
                {
                    vertices[m + getID(i, row, col) * SQUARE_VERTICES_COUNT] = faceVertices[m];
                }
            }
        }
    }

    _cubeModel = new MeshOpenGL(_cubeShaderProgram, vertices, _sizeOfVertices / sizeof(Vertex), QOpenGLBuffer::DynamicDraw);
    _cubeModel->addUniform("projection", &projection);
    _cubeModel->addUniform("camera", &camera);
    _cubeModel->addUniform("rotation", &_layerRotation);
    _cubeModel->addUniform("borderWidth", &BORDER_WIDTH);

    delete[] vertices;

    float secondPos = -halfWidth + _cellWidth;
    auto blackColor = (uint) Color::Black;
    Vertex equatorVertices[] = {
        Vertex(QVector3D(-halfWidth, -halfWidth, -halfWidth), QVector2D(0, 0), blackColor),
        Vertex(QVector3D(halfWidth, -halfWidth, -halfWidth), QVector2D(1, 0), blackColor),
        Vertex(QVector3D(halfWidth, -halfWidth, halfWidth), QVector2D(1, 1), blackColor),
        Vertex(QVector3D(-halfWidth, -halfWidth, halfWidth), QVector2D(0, 1), blackColor),
        Vertex(QVector3D(-halfWidth, secondPos, halfWidth), QVector2D(0, 0), blackColor),
        Vertex(QVector3D(halfWidth, secondPos, halfWidth), QVector2D(1, 0), blackColor),
        Vertex(QVector3D(halfWidth, secondPos, -halfWidth), QVector2D(1, 1), blackColor),
        Vertex(QVector3D(-halfWidth, secondPos, -halfWidth), QVector2D(0, 1), blackColor)
    };

    Vertex middleVertices[] = {
        Vertex(QVector3D(-halfWidth, -halfWidth, -halfWidth), QVector2D(0, 0), blackColor),
        Vertex(QVector3D(-halfWidth, -halfWidth, halfWidth), QVector2D(1, 0), blackColor),
        Vertex(QVector3D(-halfWidth, halfWidth, halfWidth), QVector2D(1, 1), blackColor),
        Vertex(QVector3D(-halfWidth, halfWidth, -halfWidth), QVector2D(0, 1), blackColor),
        Vertex(QVector3D(secondPos, -halfWidth, halfWidth), QVector2D(0, 0), blackColor),
        Vertex(QVector3D(secondPos, -halfWidth, -halfWidth), QVector2D(1, 0), blackColor),
        Vertex(QVector3D(secondPos, halfWidth, -halfWidth), QVector2D(1, 1), blackColor),
        Vertex(QVector3D(secondPos, halfWidth, halfWidth), QVector2D(0, 1), blackColor)
    };

    Vertex standingVertices[] = {
        Vertex(QVector3D(halfWidth, -halfWidth, -halfWidth), QVector2D(0, 0), blackColor),
        Vertex(QVector3D(-halfWidth, -halfWidth, -halfWidth), QVector2D(1, 0), blackColor),
        Vertex(QVector3D(-halfWidth, halfWidth, -halfWidth), QVector2D(1, 1), blackColor),
        Vertex(QVector3D(halfWidth, halfWidth, -halfWidth), QVector2D(0, 1), blackColor),
        Vertex(QVector3D(-halfWidth, -halfWidth, secondPos), QVector2D(0, 0), blackColor),
        Vertex(QVector3D(halfWidth, -halfWidth, secondPos), QVector2D(1, 0), blackColor),
        Vertex(QVector3D(halfWidth, halfWidth, secondPos), QVector2D(1, 1), blackColor),
        Vertex(QVector3D(-halfWidth, halfWidth, secondPos), QVector2D(0, 1), blackColor)
    };

    _equatorFillModel  = new MeshOpenGL(_stripeShaderProgram, equatorVertices, sizeof(equatorVertices) / sizeof(Vertex), QOpenGLBuffer::StaticDraw);
    _equatorFillModel->addUniform("projection", &projection);
    _equatorFillModel->addUniform("camera", &camera);
    _equatorFillModel->addUniform("rotation", &_layerRotation);
    _equatorFillModel->addUniform("translation", &_stripTranslation);

    _middleFillModel  = new MeshOpenGL(_stripeShaderProgram, middleVertices, sizeof(middleVertices) / sizeof(Vertex), QOpenGLBuffer::StaticDraw);
    _middleFillModel->addUniform("projection", &projection);
    _middleFillModel->addUniform("camera", &camera);
    _middleFillModel->addUniform("rotation", &_layerRotation);
    _middleFillModel->addUniform("translation", &_stripTranslation);

    _standingFillModel  = new MeshOpenGL(_stripeShaderProgram, standingVertices, sizeof(standingVertices) / sizeof(Vertex), QOpenGLBuffer::StaticDraw);
    _standingFillModel->addUniform("projection", &projection);
    _standingFillModel->addUniform("camera", &camera);
    _standingFillModel->addUniform("rotation", &_layerRotation);
    _standingFillModel->addUniform("translation", &_stripTranslation);
}

void VRCView::completeRotation()
{
    /*_cubeModel->bindBuffer();
    foreach(int key, _rotating.keys())
    {
        setSide(key, _rotating[key]);
    }
    _cubeModel->releaseBuffer();
    _isAnimating = false;
    _rotating.clear();*/
}

void VRCView::setSide(int offset, VRCFace::Side side)
{
    QColor c((uint)_colorBySide[side]);
    QVector3D cv(c.redF(), c.greenF(), c.blueF());
    int tb = 0;

    int sizeofQVecto3D = sizeof(cv);
    int sizeOfInt = sizeof(tb);

    for(int i = 0; i < SQUARE_VERTICES_COUNT; i++)
    {
        _cubeModel->write(offset + Vertex::colorOffset(), &cv, sizeofQVecto3D);
        _cubeModel->write(offset + Vertex::rotatingOffset(), &tb, sizeOfInt);
        offset += Vertex::stride();
    }
}

void VRCView::setSide(Side face, int i, int j, VRCFace::Side side)
{
    int baseOffset = Vertex::stride() * getID((int)face, i, j) * SQUARE_VERTICES_COUNT;
    setSide(baseOffset, side);

    /*int baseOffset = Vertex::stride() * getID((int)side, i, j) * SQUARE_VERTICES_COUNT;

    if(_fastMode)
    {
        setColor(baseOffset, color);
        return;
    }

    int tb = 1;
    int sizeOfInt = sizeof(tb);

    _rotating.insert(baseOffset, color);

    for(int i = 0; i < SQUARE_VERTICES_COUNT; i++)
    {
        _cubeModel->write(baseOffset + Vertex::rotatingOffset(), &tb, sizeOfInt);
        baseOffset += Vertex::stride();
    }*/
}

std::array<Vertex, 4> VRCView::getFaceVertices(VRCFace::Side side, uint color, QVector3D position, float cellWidth)
{
    float hsize = cellWidth / 2.0;
    std::array<Vertex, 4> vertices;

    switch(side)
    {
        case VRCFace::Side::Left:
            vertices[0] = Vertex(QVector3D(-hsize,  -hsize, -hsize) + position, QVector2D(0, 0), color);
            vertices[1] = Vertex(QVector3D(-hsize,  -hsize, hsize) + position, QVector2D(1, 0), color);
            vertices[2] = Vertex(QVector3D(-hsize,  hsize, hsize) + position, QVector2D(1, 1), color);
            vertices[3] = Vertex(QVector3D(-hsize,  hsize, -hsize) + position, QVector2D(0, 1), color);
            break;
        case VRCFace::Side::Front:
            vertices[0] = Vertex(QVector3D(-hsize,  -hsize, hsize) + position, QVector2D(0, 0), color);
            vertices[1] = Vertex(QVector3D(hsize,  -hsize, hsize) + position, QVector2D(1, 0), color);
            vertices[2] = Vertex(QVector3D(hsize,  hsize, hsize) + position, QVector2D(1, 1), color);
            vertices[3] = Vertex(QVector3D(-hsize,  hsize, hsize) + position, QVector2D(0, 1), color);
            break;
        case VRCFace::Side::Right:
            vertices[0] = Vertex(QVector3D(hsize,  -hsize, hsize) + position, QVector2D(0, 0), color);
            vertices[1] = Vertex(QVector3D(hsize,  -hsize, -hsize) + position, QVector2D(1, 0), color);
            vertices[2] = Vertex(QVector3D(hsize,  hsize, -hsize) + position, QVector2D(1, 1), color);
            vertices[3] = Vertex(QVector3D(hsize,  hsize, hsize) + position, QVector2D(0, 1), color);
            break;
        case VRCFace::Side::Back:
            vertices[0] = Vertex(QVector3D(hsize,  -hsize, -hsize) + position, QVector2D(0, 0), color);
            vertices[1] = Vertex(QVector3D(-hsize,  -hsize, -hsize) + position, QVector2D(1, 0), color);
            vertices[2] = Vertex(QVector3D(-hsize,  hsize, -hsize) + position, QVector2D(1, 1), color);
            vertices[3] = Vertex(QVector3D(hsize,  hsize, -hsize) + position, QVector2D(0, 1), color);
            break;
        case VRCFace::Side::Up:
            vertices[0] = Vertex(QVector3D(-hsize,  hsize, hsize) + position, QVector2D(0, 0), color);
            vertices[1] = Vertex(QVector3D(hsize,  hsize, hsize) + position, QVector2D(1, 0), color);
            vertices[2] = Vertex(QVector3D(hsize,  hsize, -hsize) + position, QVector2D(1, 1), color);
            vertices[3] = Vertex(QVector3D(-hsize,  hsize, -hsize) + position, QVector2D(0, 1), color);
            break;
        case VRCFace::Side::Down:
            vertices[0] = Vertex(QVector3D(-hsize,  -hsize, -hsize) + position, QVector2D(0, 0), color);
            vertices[1] = Vertex(QVector3D(hsize,  -hsize, -hsize) + position, QVector2D(1, 0), color);
            vertices[2] = Vertex(QVector3D(hsize,  -hsize, hsize) + position, QVector2D(1, 1), color);
            vertices[3] = Vertex(QVector3D(-hsize,  -hsize, hsize) + position, QVector2D(0, 1), color);
            break;
    }

    return vertices;
}
