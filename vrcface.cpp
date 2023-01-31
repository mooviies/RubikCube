#include "vrcface.h"

VRCFace::VRCFace(uint size, Side side)
{
    _initialSide = side;
    _size = size;
    _arraySize = _size * _size;
    _pieces = new Piece[_arraySize];
    for(uint i = 0; i < _arraySize; i++)
    {
        _pieces[i]._side = side;
        _pieces[i]._initialSide = side;
        _pieces[i]._initialPosition = i;
    }
}

double VRCFace::getCost()
{
    double cost = 0;
    for(uint row = 0; row < _size; row++)
    {
        for(uint col = 0; col < _size; col++)
        {
            uint position = getPosition(row, col);
            if(_pieces[position]._initialSide != _pieces[position]._side)
                cost += 1.0;
            else if(_pieces[position]._initialPosition != position)
                cost += 0.5;
        }
    }
    return cost;
}

void VRCFace::rotate(VRCAction::Rotation rotation)
{
    auto buffer = new Side[_arraySize];
    for(uint i = 0; i < _arraySize; i++)
    {
        buffer[i] = _pieces[i]._side;
    }

    for(uint row = 0; row < _size; row++)
    {
        for(uint col = 0; col < _size; col++)
        {

            _pieces[getPosition(row, col)].setSide(buffer[getPosition(row, col, rotation)]);
        }
    }

    delete[] buffer;
}

uint VRCFace::getPosition(uint row, uint col, VRCAction::Rotation rotation) const
{
    switch(rotation)
    {
    case VRCAction::Rotation::Clockwise:
        return getPosition(col, _size - row - 1);
    case VRCAction::Rotation::CounterClockwise:
        return getPosition(_size - col - 1, row);
    case VRCAction::Rotation::Turn180:
        return getPosition(_size - row - 1, _size - col - 1);
    }
}
