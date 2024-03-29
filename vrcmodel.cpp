#include "vrcmodel.h"

typedef VRCAction::Layer Layer;
typedef VRCAction::Option Option;
typedef VRCAction::Rotation Rotation;

typedef VRCAction::LayerMask LayerMask;
typedef VRCAction::OptionMask OptionMask;
typedef VRCAction::RotationMask RotationMask;
typedef VRCAction::GeneralMask GeneralMask;

typedef VRCFace::Side Side;

VRCModel::VRCModel(uint size)
{
    _view = nullptr;
    _size = size;
    _maxLayerNumber = _size / 2 + 1;
    for(auto i = (uchar)Side::Left; i <= (uchar)Side::Down; i++)
    {
        _cube.append(new VRCFace(size, (Side)i));
    }
}

VRCModel::~VRCModel()
{
    for(auto face : qAsConst(_cube))
        delete face;
}

double VRCModel::getCost()
{
    double cost = 0;
    for(auto face : _cube)
    {
        cost += face->getCost();
    }

    return cost;
}

void VRCModel::setView(VRCView *view)
{
    _view = view;
}

bool VRCModel::execute(VRCAction action)
{
    auto layerNumber = action.getLayerNumber();

    if(layerNumber == 0)
        layerNumber = 1;
    else if(layerNumber > _size)
        layerNumber = _size;

    auto layer = action.getLayer();
    auto option = action.getOption();
    auto rotation = action.getRotation();
    auto rotationReversed = action.getRotation(true);

    if(!parseLayer(layer, option, layerNumber))
        return false;

    if(!applyRotation(layer, option, rotation, rotationReversed, layerNumber))
        return false;

    if(_view != nullptr)
    {
        _view->update(action);
    }

    return true;
}

bool VRCModel::parseLayer(VRCAction::Layer &layer, VRCAction::Option &option, ushort &layerNumber)
{
    if((uint)layer & (uint)LayerMask::Center)
    {
        if(_size % 2 == 0)
            return false;

        layerNumber = ceil(_size / 2.0);
        switch(layer)
        {
            case Layer::Middle:
                layer = Layer::Left;
                break;
            case Layer::Equator:
                layer = Layer::Down;
                break;
            case Layer::Standing:
                layer = Layer::Front;
                break;
            default:
                return false;
        }
    }
    else if((uint)layer & (uint)LayerMask::Cube)
    {
        layerNumber = _size;
        option = Option::Wide;

        switch(layer)
        {
            case Layer::CubeX:
                layer = Layer::Right;
                break;
            case Layer::CubeY:
                layer = Layer::Up;
                break;
            case Layer::CubeZ:
                layer = Layer::Front;
                break;
            default:
                return false;
        }
    }

    return true;
}

bool VRCModel::applyRotation(VRCAction::Layer layer, VRCAction::Option option, VRCAction::Rotation rotation, VRCAction::Rotation rotationReversed, ushort layerNumber)
{
    ushort startLayer = option == Option::Wide ? 1 : layerNumber;
    switch(layer)
    {
        case Layer::Left:
            if(startLayer == 1)
                getFace(Side::Left).rotate(rotation);

            if(layerNumber == _size)
                getFace(Side::Right).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                auto lastCol = _size - i + 1;
                VRCFace::rotate(
                    getFace(Side::Front).beginCol(i),
                    getFace(Side::Front).endCol(i),
                    getFace(Side::Down).beginCol(i),
                    getFace(Side::Back).rbeginCol(lastCol),
                    getFace(Side::Up).beginCol(i),
                    rotation);
            }
            break;
        case Layer::Front:
            if(startLayer == 1)
                getFace(Side::Front).rotate(rotation);

            if(layerNumber == _size)
                getFace(Side::Back).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                auto lastCol = _size - i + 1;
                auto lastRow = lastCol;
                VRCFace::rotate(
                    getFace(Side::Up).beginRow(i),
                    getFace(Side::Up).endRow(i),
                    getFace(Side::Right).rbeginCol(i),
                    getFace(Side::Down).rbeginRow(lastRow),
                    getFace(Side::Left).beginCol(lastCol),
                    rotation);
            }
            break;
        case Layer::Right:
            if(startLayer == 1)
                getFace(Side::Right).rotate(rotation);

            if(layerNumber == _size)
                getFace(Side::Left).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                auto lastCol = _size - i + 1;
                VRCFace::rotate(
                    getFace(Side::Front).beginCol(lastCol),
                    getFace(Side::Front).endCol(lastCol),
                    getFace(Side::Up).beginCol(lastCol),
                    getFace(Side::Back).rbeginCol(i),
                    getFace(Side::Down).beginCol(lastCol),
                    rotation);
            }
            break;
        case Layer::Back:
            if(startLayer == 1)
                getFace(Side::Back).rotate(rotation);

            if(layerNumber == _size)
                getFace(Side::Front).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                auto lastCol = _size - i + 1;
                auto lastRow = lastCol;
                VRCFace::rotate(
                    getFace(Side::Up).beginRow(lastRow),
                    getFace(Side::Up).endRow(lastRow),
                    getFace(Side::Left).beginCol(i),
                    getFace(Side::Down).rbeginRow(i),
                    getFace(Side::Right).rbeginCol(lastCol),
                    rotation);
            }
            break;
        case Layer::Up:
            if(startLayer == 1)
                getFace(Side::Up).rotate(rotation);

            if(layerNumber == _size)
                getFace(Side::Down).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                auto lastRow = _size - i + 1;
                VRCFace::rotate(
                    getFace(Side::Front).beginRow(lastRow),
                    getFace(Side::Front).endRow(lastRow),
                    getFace(Side::Left).beginRow(lastRow),
                    getFace(Side::Back).beginRow(lastRow),
                    getFace(Side::Right).beginRow(lastRow),
                    rotation);
            }
            break;
        case Layer::Down:
            if(startLayer == 1)
                getFace(Side::Down).rotate(rotation);

            if(layerNumber == _size)
                getFace(Side::Up).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                VRCFace::rotate(
                    getFace(Side::Front).beginRow(i),
                    getFace(Side::Front).endRow(i),
                    getFace(Side::Right).beginRow(i),
                    getFace(Side::Back).beginRow(i),
                    getFace(Side::Left).beginRow(i),
                    rotation);
            }
            break;
        default:
            return false;
    }

    return true;
}
