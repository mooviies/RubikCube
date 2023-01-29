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

void VRCModel::setView(VRCView *view)
{
    _view = view;
}

bool VRCModel::execute(VRCAction action)
{
    auto layerNumber = action.getLayerNumber();

    if(layerNumber == 0)
        layerNumber = 1;
    else if(layerNumber > _maxLayerNumber)
        throw std::invalid_argument(QString("The layer number %1 is invalid for a cube of size %2. The maximum value is %3.")
                                    .arg(layerNumber, _size, _maxLayerNumber).toStdString());

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
        _view->update(*this);
    }

    return true;
}

bool VRCModel::parseLayer(VRCAction::Layer &layer, VRCAction::Option &option, ushort &layerNumber)
{
    if((uint)layer & (uint)LayerMask::Center)
    {
        if(_size % 2 == 0)
            return false;

        layerNumber = _size / 2;
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
    else if((uint)layer & (uint)LayerMask::Center)
    {
        layerNumber = _size - 1;
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
            getFace(Side::Left).rotate(rotation);
            if(_maxLayerNumber == _size)
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
            getFace(Side::Front).rotate(rotation);
            if(_maxLayerNumber == _size)
                getFace(Side::Back).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                auto lastCol = _size - i + 1;
                auto lastRow = lastCol;
                VRCFace::rotate(
                    getFace(Side::Left).rbeginCol(lastCol),
                    getFace(Side::Left).rendCol(lastCol),
                    getFace(Side::Up).beginRow(lastRow),
                    getFace(Side::Right).beginCol(i),
                    getFace(Side::Down).rbeginRow(i),
                    rotation);
            }
            break;
        case Layer::Right:
            getFace(Side::Right).rotate(rotation);
            if(_maxLayerNumber == _size)
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
            getFace(Side::Back).rotate(rotation);
            if(_maxLayerNumber == _size)
                getFace(Side::Front).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                auto lastCol = _size - i + 1;
                auto lastRow = lastCol;
                VRCFace::rotate(
                    getFace(Side::Left).rbeginCol(i),
                    getFace(Side::Left).rendCol(i),
                    getFace(Side::Down).rbeginRow(lastRow),
                    getFace(Side::Right).beginCol(lastCol),
                    getFace(Side::Up).beginRow(i),
                    rotation);
            }
            break;
        case Layer::Up:
            getFace(Side::Up).rotate(rotation);

            if(_maxLayerNumber == _size)
                getFace(Side::Down).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                VRCFace::rotate(
                    getFace(Side::Front).beginRow(i),
                    getFace(Side::Front).endRow(i),
                    getFace(Side::Left).beginRow(i),
                    getFace(Side::Back).beginRow(i),
                    getFace(Side::Right).beginRow(i),
                    rotation);
            }
            break;
        case Layer::Down:
            getFace(Side::Down).rotate(rotation);

            if(_maxLayerNumber == _size)
                getFace(Side::Up).rotate(rotationReversed);

            for(auto i = startLayer; i <= layerNumber; i++)
            {
                auto lastRow = _size - i + 1;
                VRCFace::rotate(
                    getFace(Side::Front).beginRow(lastRow),
                    getFace(Side::Front).endRow(lastRow),
                    getFace(Side::Right).beginRow(lastRow),
                    getFace(Side::Back).beginRow(lastRow),
                    getFace(Side::Left).beginRow(lastRow),
                    rotation);
            }
            break;
        default:
            return false;
    }

    return true;
}
