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
typedef VRCAction::Rotation Rotation;
typedef VRCAction::Layer Layer;
typedef VRCAction::LayerMask LayerMask;
typedef VRCAction::Option Option;

const float VRCView::BORDER_WIDTH = 0.015f;

VRCView::VRCView(VRCModel *model)
{
    _model = model;
    _size = _model->getSize();
    _fastMode = false;
    _isAnimating = false;

    _cubeShaderProgram = nullptr;
    _colorBySide.insert(Side::Left, Color::Orange);
    _colorBySide.insert(Side::Front, Color::Green);
    _colorBySide.insert(Side::Right, Color::Red);
    _colorBySide.insert(Side::Back, Color::Blue);
    _colorBySide.insert(Side::Up, Color::White);
    _colorBySide.insert(Side::Down, Color::Yellow);
}

void VRCView::init(const QMatrix4x4 &projection, const QMatrix4x4 &camera, const QMatrix4x4 &world, const QMatrix4x4 &model)
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

    create(projection, camera, world, model);
}

void VRCView::setModel(VRCModel *model)
{
    _model = model;
}

void VRCView::update(const VRCAction &lastAction)
{
    _size = _model->getSize();

    _cubeModel->bindBuffer();
    for(auto face : *_model)
    {
        for(uint r = 0; r < _size; r++)
        {
            for(uint c = 0; c < _size; c++)
            {
                if(!face->wasModified(r + 1, c + 1))
                    continue;

                setColor(face->getInitialSide(), r, c, _colorBySide[face->getSide(r + 1, c + 1)]);
            }
        }
    }
    _cubeModel->releaseBuffer();

    _animatingAction = lastAction;
    if(_fastMode)
        updateVisualModel();
    else
    {
        switch(getRotationFace(_animatingAction.getLayer()))
        {
            case Layer::Back:
                _rotationVector = QVector3D(0, 0, 1);
                break;
            case Layer::Front:
                _rotationVector = QVector3D(0, 0, -1);
                break;
            case Layer::Left:
                _rotationVector = QVector3D(1, 0, 0);
                break;
            case Layer::Right:
                _rotationVector = QVector3D(-1, 0, 0);
                break;
            case Layer::Up:
                _rotationVector = QVector3D(0, -1, 0);
                break;
            case Layer::Down:
                _rotationVector = QVector3D(0, 1, 0);
                break;
            default:
                break;
        }

        switch(_animatingAction.getRotation())
        {

            case VRCAction::Rotation::Clockwise:
                _targetRotation = 90;
                break;
            case VRCAction::Rotation::CounterClockwise:
                _targetRotation = 90;
                _rotationVector *= -1;
                break;
            case VRCAction::Rotation::Turn180:
                _targetRotation = 180;
                break;
        }

        _currentRotation = 0;
        _isAnimating = true;
    }
}

void VRCView::draw()
{
    if(_isAnimating)
    {
        _currentRotation += ROTATION_SPEED;
        if(_currentRotation >= _targetRotation)
        {
            updateVisualModel();
        }
        else
        {
            _layerRotation.setToIdentity();
            _layerRotation.rotate(_currentRotation, _rotationVector);
        }

        auto layerNumber = _animatingAction.getLayerNumber();
        if(layerNumber > 1 && layerNumber < _size && _animatingAction.getOption() == Option::None)
        {
            auto layer = getFillLayer(_animatingAction.getLayer());
            if((uint)layer & (uint)LayerMask::Face)
            {
                layerNumber = _size - layerNumber + 1;
            }
            else if((uint)layer & (uint)LayerMask::Center)
            {
                layerNumber = _size / 2;
            }

            _stripTranslation.setToIdentity();

            switch(layer)
            {
            case Layer::Down:
                _stripTranslation.translate(0, _cellWidth * (layerNumber - 1), 0);
                _equatorFillModel->draw();
                break;
            case Layer::Left:
                _stripTranslation.translate(_cellWidth * (layerNumber - 1), 0, 0);
                _middleFillModel->draw();
                break;
            case Layer::Back:
                _stripTranslation.translate(0, 0, _cellWidth * (layerNumber - 1));
                _standingFillModel->draw();
                break;
            default:
                break;
            }
        }
    }

    _cubeModel->draw();
}

void VRCView::create(const QMatrix4x4 &projection, const QMatrix4x4 &camera, const QMatrix4x4 &world, const QMatrix4x4 &model)
{
    auto size = _model->getSize();
    int nbCubeVertices = NUMBER_SIDE * size * size * SQUARE_VERTICES_COUNT;
    int nbVertices = nbCubeVertices;// + nbInsideVertices * 3;
    _sizeOfVertices = sizeof(Vertex) * nbVertices;

    Vertex *vertices = new Vertex[nbVertices];
    _width = getWidth(size);
    _cellWidth = _width / float(size);
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
    _cubeModel->addUniform("world", &world);
    _cubeModel->addUniform("model", &model);
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

void VRCView::updateVisualModel()
{
    _cubeModel->bindBuffer();
    foreach(int key, _rotating.keys())
    {
        setColor(key, _rotating[key]);
    }
    _cubeModel->releaseBuffer();
    _isAnimating = false;
    _rotating.clear();
}

void VRCView::setColor(int offset, VRCFace::Color color)
{
    QColor c((uint)color);
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

void VRCView::setColor(Side side, int i, int j, VRCFace::Color color)
{
    int baseOffset = Vertex::stride() * getID((int)side, i, j) * SQUARE_VERTICES_COUNT;

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
    }
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

VRCAction::Layer VRCView::getFillLayer(VRCAction::Layer layer)
{
    switch(layer)
    {
        case Layer::Up:
        case Layer::Equator:
            return Layer::Down;
        case Layer::Right:
        case Layer::Middle:
            return Layer::Left;
        case Layer::Front:
        case Layer::Standing:
            return Layer::Back;
        default:
            return layer;
    }
}

VRCAction::Layer VRCView::getRotationFace(VRCAction::Layer layer)
{
    switch(layer)
    {
        case VRCAction::Layer::Left:
        case VRCAction::Layer::Front:
        case VRCAction::Layer::Right:
        case VRCAction::Layer::Back:
        case VRCAction::Layer::Up:
        case VRCAction::Layer::Down:
            return layer;
        case VRCAction::Layer::Middle:
            return Layer::Left;
        case VRCAction::Layer::Equator:
        case VRCAction::Layer::CubeX:
            return Layer::Right;
        case VRCAction::Layer::CubeY:
            return Layer::Up;
        case VRCAction::Layer::Standing:
        case VRCAction::Layer::CubeZ:
            return Layer::Front;
    }
}
