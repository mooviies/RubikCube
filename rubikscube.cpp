/*
    Virtual Rubik's Cube is a tool to explore Rubik's cubes of any sizes.
    Copyright (C) 2018 mooviies

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    https://github.com/mooviies/RubikCube
*/

#include "rubikscube.h"

#include <QOpenGLFunctions>
#include <QColor>
#include <QWidget>
#include <stdexcept>
#include <QGuiApplication>

#include "cube.h"
#include "rubikscubeview.h"

RubiksCube::RubiksCube(RubiksCubeView* parent, int size) : _parent(parent)
{
    if(size < MIN_SIZE)
        _size = MIN_SIZE;
    else if(size > MAX_SIZE)
        _size = MAX_SIZE;
    else
        _size = size;

    _currentCommand = 0;
    _maxLayer = _size / 2;
    _cubeShaderProgram = nullptr;
    _borderWidth = 0.015f;
}

RubiksCube::~RubiksCube()
{
    clean();
}

void RubiksCube::init(const QMatrix4x4 &camera, const QMatrix4x4 &projection)
{
    _alternateFace.clear();
    for(int i = 0; i < NUMBER_SIDE; i++)
        _alternateFace.insert((Face)i, (Face)i);

    _colorByFace.clear();
    _colorByFace.append(Color::Orange);
    _colorByFace.append(Color::Green);
    _colorByFace.append(Color::Red);
    _colorByFace.append(Color::Blue);
    _colorByFace.append(Color::White);
    _colorByFace.append(Color::Yellow);

    _currentCommand = 0;
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

    _debugShaderProgram = new QOpenGLShaderProgram();
    _debugShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/debug.vert");
    _debugShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/debug.frag");

    _cubeShaderProgram->link();
    _stripeShaderProgram->link();
    _debugShaderProgram->link();

    create(camera, projection);
}

void RubiksCube::reset(const QMatrix4x4 &camera, const QMatrix4x4 &projection)
{
    clean();
    init(camera, projection);
}

bool RubiksCube::rotate(const QList<int>& flagsList, bool fastMode)
{
    if(flagsList.isEmpty() || _isAnimating)
        return false;

    _fastMode = fastMode;
    _currentCommand = 0;
    _commands = flagsList;

    return true;
}

bool RubiksCube::rotate(int flags, bool fastMode)
{
    if(_isAnimating)
        return false;

    _fastMode = fastMode;

    int layerPos = flags & RotationComponent::NbLayerMask;
    int components = flags & RotationComponent::ComponentsMask;

    if(components == 0)
        return false;

    if(!(components & RotationComponent::MandatoryMaskA) || !(components & RotationComponent::MandatoryMaskB))
        return false;

    if(_size % 2 == 0 && (components & RotationComponent::CenterLayers))
        return false;

    _rotationFlags = flags;

    if(layerPos <= 0)
        layerPos = 1;
    else if(layerPos > _size / 2 + 1)
        return false;

    Face rotationFace = Face::Up;
    _rotating.clear();

    _cubeModel->bindBuffer();

    if(components & RotationComponent::WholeCube)
    {
        Face inverseRotationFace = Face::Up;
        if(components & RotationComponent::CubeZ)
        {
            rotationFace = Face::Front;
            inverseRotationFace = Face::Back;
        }
        else if(components & RotationComponent::CubeX)
        {
            rotationFace = Face::Right;
            inverseRotationFace = Face::Left;
        }
        else if(components & RotationComponent::CubeY)
        {
            rotationFace = Face::Up;
            inverseRotationFace = Face::Down;
        }

        rotateFace(rotationFace, components);
        for(int i = 1; i <= _size; i++)
        {
            rotateLayer(rotationFace, components + i);
        }

        if(components & RotationComponent::Clockwise)
            rotateFace(inverseRotationFace, RotationComponent::CounterClockwise);
        else if(components & RotationComponent::CounterClockwise)
            rotateFace(inverseRotationFace, RotationComponent::Clockwise);
        else
            rotateFace(inverseRotationFace, RotationComponent::Turn180);

    }
    else if(components & RotationComponent::CenterLayers)
    {
        int cl = _size / 2 + 1;
        if(components & RotationComponent::Equator)
        {
            rotationFace = Face::Down;
        }
        else if(components & RotationComponent::Horizontal)
        {
            rotationFace = Face::Up;
        }
        else if(components & RotationComponent::Middle)
        {
            rotationFace = Face::Left;
        }
        else if(components & RotationComponent::Standing)
        {
            rotationFace = Face::Front;
        }
        rotateLayer(rotationFace, components + cl);
    }
    else
    {
        for(int i = 0; i < NUMBER_SIDE; i++)
        {
            if(components & (0x100 << i))
            {
                rotationFace = (Face)i;
                if((components & RotationComponent::Wide || layerPos == 1))
                {
                    rotateFace(rotationFace, components);
                }

                for(int j = (components & RotationComponent::Wide) ? 1: layerPos; j <= layerPos; j++)
                {
                    rotateLayer(rotationFace, components + j);
                }
                break;
            }
        }
    }

    _cubeModel->releaseBuffer();

    if(_fastMode)
        return true;

    switch(rotationFace)
    {
    case Face::Back:
        _rotationVector = QVector3D(0, 0, 1);
        break;
    case Face::Front:
        _rotationVector = QVector3D(0, 0, -1);
        break;
    case Face::Left:
        _rotationVector = QVector3D(1, 0, 0);
        break;
    case Face::Right:
        _rotationVector = QVector3D(-1, 0, 0);
        break;
    case Face::Up:
        _rotationVector = QVector3D(0, -1, 0);
        break;
    case Face::Down:
        _rotationVector = QVector3D(0, 1, 0);
        break;
    }

    if(components & RotationComponent::Clockwise)
    {
        _targetRotation = 90;
    }
    else if(components & RotationComponent::CounterClockwise)
    {
        _targetRotation = 90;
        _rotationVector *= -1;
    }
    else
    {
        _targetRotation = 180;
    }

    _currentRotation = 0;
    _isAnimating = true;

    return true;
}

void RubiksCube::rotateFace(Face faceID, int flags)
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

    bool clockwise = flags & RotationComponent::Clockwise, counterClockwise = flags & RotationComponent::CounterClockwise,
            turn180 = flags & RotationComponent::Turn180;

    for(int i = 0; i < _size; i++)
    {
        for(int j = 0; j < _size; j++)
        {
            if(clockwise)
            {
                face[i][j] = buffer[_size - j - 1][i];
            }
            else if(counterClockwise)
            {
                face[i][j] = buffer[j][_size - i - 1];
            }
            else if(turn180)
            {
                face[i][j] = buffer[_size - i - 1][_size - j - 1];
            }
            int test = (int)face[i][j];
            setColor(faceID, i, j, _colorByFace[test]);
        }
    }

    for(int i = 0; i < _size; i++)
        delete[] buffer[i];
    delete[] buffer;
}

void RubiksCube::rotateLayer(Face faceID, int flags)
{
    bool clockwise = flags & RotationComponent::Clockwise, counterClockwise = flags & RotationComponent::CounterClockwise;
    int layer = flags & RotationComponent::NbLayerMask;

    bool isFaceUp = faceID == Face::Up;
    if(isFaceUp || faceID == Face::Down)
    {
        int j = isFaceUp ? _size - layer : layer - 1;
        bool clockwiseUpView = (clockwise && isFaceUp) || (counterClockwise && !isFaceUp);
        bool counterClockwiseUpView = (counterClockwise && isFaceUp) || (clockwise && !isFaceUp);

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
            setColor(Face::Front, i, j, _colorByFace[(int)front[i][j]]);
            setColor(Face::Right, i, j, _colorByFace[(int)right[i][j]]);
            setColor(Face::Back, i, j, _colorByFace[(int)back[i][j]]);
            setColor(Face::Left, i, j, _colorByFace[(int)left[i][j]]);
        }

        return;
    }

    bool isFaceLeft = faceID == Face::Left;
    if(isFaceLeft || faceID == Face::Right)
    {
        int i = isFaceLeft ? layer - 1: _size - layer;
        bool clockwiseUpView = (clockwise && isFaceLeft) || (counterClockwise && !isFaceLeft);
        bool counterClockwiseUpView = (counterClockwise && isFaceLeft) || (clockwise && !isFaceLeft);

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
            setColor(Face::Front, i, j, _colorByFace[(int)front[i][j]]);
            setColor(Face::Up, i, j, _colorByFace[(int)up[i][j]]);
            setColor(Face::Back, _size - i - 1, _size - j - 1, _colorByFace[(int)back[_size - i - 1][_size - j - 1]]);
            setColor(Face::Down, i, j, _colorByFace[(int)down[i][j]]);
        }
        return;
    }

    bool isFaceFront = faceID == Face::Front;
    if(isFaceFront || faceID == Face::Back)
    {
        int i = isFaceFront ? layer - 1: _size - layer;
        bool clockwiseUpView = (clockwise && isFaceFront) || (counterClockwise && !isFaceFront);
        bool counterClockwiseUpView = (counterClockwise && isFaceFront) || (clockwise && !isFaceFront);

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
            setColor(Face::Up, _size - j - 1, i, _colorByFace[(int)up[_size - j - 1][i]]);
            setColor(Face::Left, _size - i - 1, _size - j - 1, _colorByFace[(int)left[_size - i - 1][_size - j - 1]]);
            setColor(Face::Down, j, _size - i - 1, _colorByFace[(int)down[j][_size - i - 1]]);
            setColor(Face::Right, i, j, _colorByFace[(int)right[i][j]]);
        }
        return;
    }
}

void RubiksCube::completeRotation()
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

void RubiksCube::draw()
{
    if(_isAnimating)
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

        int nbLayer = _rotationFlags & RotationComponent::NbLayerMask;
        if(nbLayer > 1 && nbLayer < _size && !(_rotationFlags & RotationComponent::Wide))
        {
            int flags = _rotationFlags;
            int rotation = flags & RotationComponent::MandatoryMaskA;
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
            case RotationComponent::Horizontal:
                rotation = RotationComponent::Equator;
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
    }

    _cubeModel->draw();
    //_debugModel->draw();

    if(!_isAnimating && _currentCommand < _commands.size())
        rotate(_commands[_currentCommand++], _fastMode);
}

void RubiksCube::mouseReleaseEvent(QMouseEvent* event, const QMatrix4x4& projection, const QMatrix4x4& camera)
{
    QPoint mousePos = event->pos();
    QVector4D rayClip((2.0f * float(mousePos.x())) / float(_parent->width()) - 1.0f,
                      1.0f - (2.0f * float(mousePos.y())) / float(_parent->height()),
                      -1.0f, 1.0f);

    QVector4D rayEye = projection.inverted() * rayClip;
    rayEye.setZ(-1.0f);
    rayEye.setW(0.0f);

    QVector3D rayWor = (camera.inverted() * rayEye).toVector3D().normalized();

    float h = _width / 2.0f;
    QVector3D origin = camera.inverted() * QVector3D(0, 0, 0);
    QVector3D pointOnCube;
    bool isOnCube = false;

    QVector2D posOnFace;
    QVector3D xpos = getHitPoint(QVector3D(-1, 0, 0), QVector3D(-h, 0, 0), origin, rayWor);
    int flags = 0;

    if(event->button() & Qt::LeftButton)
        flags |= RotationComponent::Clockwise;
    else if(event->button() & Qt::RightButton)
        flags |= RotationComponent::CounterClockwise;
    else if(event->button() & Qt::MidButton)
        flags |= RotationComponent::Turn180;

    int middleLayer =  _size / 2 + 1;

    if(xpos.y() >= -h && xpos.y() <= h && xpos.z() >= -h && xpos.z() <= h)
    {
        isOnCube = true;
        pointOnCube = xpos;
        posOnFace = QVector2D(xpos.z() + h, xpos.y() + h) / _cellWidth;

        if(QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
        {
            int layer = _size - int(posOnFace.x());
            if(_size - int(posOnFace.y()) > middleLayer)
                flags ^= RotationComponent::ReverseMask;

            if(layer > middleLayer)
            {
                flags |= RotationComponent::Back;
                flags ^= RotationComponent::ReverseMask;
                layer = _size - layer + 1;
            }
            else
                flags |= RotationComponent::Front;

            flags += layer;
        }
        else
        {
            int layer = _size - int(posOnFace.y());
            if(_size - int(posOnFace.x()) < middleLayer)
                flags ^= RotationComponent::ReverseMask;

            if(layer > middleLayer)
            {
                flags |= RotationComponent::Down;
                flags ^= RotationComponent::ReverseMask;
                layer = _size - layer + 1;
            }
            else
                flags |= RotationComponent::Up;

            flags += layer;
        }
    }
    else
    {
        QVector3D ypos = getHitPoint(QVector3D(0, 1, 0), QVector3D(0, h, 0), origin, rayWor);
        if(ypos.x() >= -h && ypos.x() <= h && ypos.z() >= -h && ypos.z() <= h)
        {
            isOnCube = true;
            pointOnCube = ypos;
            posOnFace = QVector2D(ypos.x() + h, h - ypos.z()) / _cellWidth;

            if(QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
            {
                int layer = posOnFace.x() + 1;
                if(_size - int(posOnFace.y()) < middleLayer)
                    flags ^= RotationComponent::ReverseMask;

                if(layer > middleLayer)
                {
                    flags |= RotationComponent::Right;
                    flags ^= RotationComponent::ReverseMask;
                    layer = _size - layer + 1;
                }
                else
                    flags |= RotationComponent::Left;

                flags += layer;
            }
            else
            {
                int layer = posOnFace.y() + 1;
                if(_size - int(posOnFace.x()) > middleLayer)
                    flags ^= RotationComponent::ReverseMask;

                if(layer > middleLayer)
                {
                    flags |= RotationComponent::Back;
                    flags ^= RotationComponent::ReverseMask;
                    layer = _size - layer + 1;
                }
                else
                    flags |= RotationComponent::Front;

                flags += layer;
            }
        }
        else
        {
            QVector3D zpos = getHitPoint(QVector3D(0, 0, 1), QVector3D(0, 0, h), origin, rayWor);
            if(zpos.x() >= -h && zpos.x() <= h && zpos.y() >= -h && zpos.y() <= h)
            {
                isOnCube = true;
                pointOnCube = zpos;
                posOnFace = QVector2D(zpos.x() + h, zpos.y() + h) / _cellWidth;

                if(QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
                {
                    int layer = posOnFace.x() + 1;
                    if(_size - int(posOnFace.y()) < middleLayer)
                        flags ^= RotationComponent::ReverseMask;

                    if(layer > middleLayer)
                    {
                        flags |= RotationComponent::Right;
                        flags ^= RotationComponent::ReverseMask;
                        layer = _size - layer + 1;
                    }
                    else
                        flags |= RotationComponent::Left;

                    flags += layer;
                }
                else
                {
                    int layer = _size - int(posOnFace.y());
                    if(_size - int(posOnFace.x()) < middleLayer)
                        flags ^= RotationComponent::ReverseMask;

                    if(layer > middleLayer)
                    {
                        flags |= RotationComponent::Down;
                        flags ^= RotationComponent::ReverseMask;
                        layer = _size - layer + 1;
                    }
                    else
                        flags |= RotationComponent::Up;

                    flags += layer;
                }
            }
        }
    }

    //_debugCubeTranslation.setToIdentity();
    if(isOnCube)
    {
        //_debugCubeTranslation.translate(pointOnCube);
        rotate(flags);
    }

    event->accept();
}

void RubiksCube::create(const QMatrix4x4 &camera, const QMatrix4x4 &projection)
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

    Cube cube;
    cube.setSize(_cellWidth);

    _cube = new Face**[NUMBER_SIDE];
    for(int i = 0; i < NUMBER_SIDE; i++)
    {
        Face face = (Face)(i);
        cube.setColor(face, _colorByFace[(int)face]);

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
                    vertices[m + id * SQUARE_VERTICES_COUNT] = faceVertices[m];
                }

                _cube[i][j][k] = face;
            }
        }
    }

    _cubeModel = new OpenGL3DModel(_cubeShaderProgram, vertices, _sizeOfVertices / sizeof(Vertex), QOpenGLBuffer::DynamicDraw);
    _cubeModel->addUniform("projection", &projection);
    _cubeModel->addUniform("camera", &camera);
    _cubeModel->addUniform("rotation", &_layerRotation);
    _cubeModel->addUniform("borderWidth", &_borderWidth);

    delete[] vertices;

    float secondPos = -halfWidth + _cellWidth;
    Vertex equatorVertices[] = {
        Vertex(QVector3D(-halfWidth, -halfWidth, -halfWidth), QVector2D(0, 0), Color::Black),
        Vertex(QVector3D(halfWidth, -halfWidth, -halfWidth), QVector2D(1, 0), Color::Black),
        Vertex(QVector3D(halfWidth, -halfWidth, halfWidth), QVector2D(1, 1), Color::Black),
        Vertex(QVector3D(-halfWidth, -halfWidth, halfWidth), QVector2D(0, 1), Color::Black),
        Vertex(QVector3D(-halfWidth, secondPos, halfWidth), QVector2D(0, 0), Color::Black),
        Vertex(QVector3D(halfWidth, secondPos, halfWidth), QVector2D(1, 0), Color::Black),
        Vertex(QVector3D(halfWidth, secondPos, -halfWidth), QVector2D(1, 1), Color::Black),
        Vertex(QVector3D(-halfWidth, secondPos, -halfWidth), QVector2D(0, 1), Color::Black)
    };

    Vertex middleVertices[] = {
        Vertex(QVector3D(-halfWidth, -halfWidth, -halfWidth), QVector2D(0, 0), Color::Black),
        Vertex(QVector3D(-halfWidth, -halfWidth, halfWidth), QVector2D(1, 0), Color::Black),
        Vertex(QVector3D(-halfWidth, halfWidth, halfWidth), QVector2D(1, 1), Color::Black),
        Vertex(QVector3D(-halfWidth, halfWidth, -halfWidth), QVector2D(0, 1), Color::Black),
        Vertex(QVector3D(secondPos, -halfWidth, halfWidth), QVector2D(0, 0), Color::Black),
        Vertex(QVector3D(secondPos, -halfWidth, -halfWidth), QVector2D(1, 0), Color::Black),
        Vertex(QVector3D(secondPos, halfWidth, -halfWidth), QVector2D(1, 1), Color::Black),
        Vertex(QVector3D(secondPos, halfWidth, halfWidth), QVector2D(0, 1), Color::Black)
    };

    Vertex standingVertices[] = {
        Vertex(QVector3D(halfWidth, -halfWidth, -halfWidth), QVector2D(0, 0), Color::Black),
        Vertex(QVector3D(-halfWidth, -halfWidth, -halfWidth), QVector2D(1, 0), Color::Black),
        Vertex(QVector3D(-halfWidth, halfWidth, -halfWidth), QVector2D(1, 1), Color::Black),
        Vertex(QVector3D(halfWidth, halfWidth, -halfWidth), QVector2D(0, 1), Color::Black),
        Vertex(QVector3D(-halfWidth, -halfWidth, secondPos), QVector2D(0, 0), Color::Black),
        Vertex(QVector3D(halfWidth, -halfWidth, secondPos), QVector2D(1, 0), Color::Black),
        Vertex(QVector3D(halfWidth, halfWidth, secondPos), QVector2D(1, 1), Color::Black),
        Vertex(QVector3D(-halfWidth, halfWidth, secondPos), QVector2D(0, 1), Color::Black)
    };

    _equatorFillModel  = new OpenGL3DModel(_stripeShaderProgram, equatorVertices, sizeof(equatorVertices) / sizeof(Vertex), QOpenGLBuffer::StaticDraw);
    _equatorFillModel->addUniform("projection", &projection);
    _equatorFillModel->addUniform("camera", &camera);
    _equatorFillModel->addUniform("rotation", &_layerRotation);
    _equatorFillModel->addUniform("translation", &_stripTranslation);

    _middleFillModel  = new OpenGL3DModel(_stripeShaderProgram, middleVertices, sizeof(middleVertices) / sizeof(Vertex), QOpenGLBuffer::StaticDraw);
    _middleFillModel->addUniform("projection", &projection);
    _middleFillModel->addUniform("camera", &camera);
    _middleFillModel->addUniform("rotation", &_layerRotation);
    _middleFillModel->addUniform("translation", &_stripTranslation);

    _standingFillModel  = new OpenGL3DModel(_stripeShaderProgram, standingVertices, sizeof(standingVertices) / sizeof(Vertex), QOpenGLBuffer::StaticDraw);
    _standingFillModel->addUniform("projection", &projection);
    _standingFillModel->addUniform("camera", &camera);
    _standingFillModel->addUniform("rotation", &_layerRotation);
    _standingFillModel->addUniform("translation", &_stripTranslation);

    Vertex debugVertices[CUBE_VERTICES_COUNT];
    cube.setPosition(QVector3D(0, 0, 0));
    cube.setSize(_width / 10.0);
    cube.generate();
    for(int i = 0; i < CUBE_VERTICES_COUNT; i++)
        debugVertices[i] = cube.vertices()[i];

    _debugModel  = new OpenGL3DModel(_debugShaderProgram, debugVertices, CUBE_VERTICES_COUNT, QOpenGLBuffer::StaticDraw);
    _debugModel->addUniform("projection", &projection);
    _debugModel->addUniform("camera", &camera);
    _debugModel->addUniform("translation", &_debugCubeTranslation);

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
    delete _cubeModel;
    delete _equatorFillModel;
    delete _middleFillModel;
    delete _standingFillModel;
    delete _debugModel;
    delete _cubeShaderProgram;
    _cubeShaderProgram = nullptr;
}

void RubiksCube::setColor(int offset, Color color)
{
    QColor c(color);
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

void RubiksCube::setColor(Face face, int i, int j, Color color)
{
    int baseOffset = Vertex::stride() * getID((int)face, i, j) * SQUARE_VERTICES_COUNT;

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

QVector3D RubiksCube::getHitPoint(const QVector3D& n, const QVector3D& p0, const QVector3D& l0, const QVector3D& ray)
{
    float t = QVector3D::dotProduct(p0 - l0, n) / QVector3D::dotProduct(ray, n);
    return l0 + ray * t;
}
