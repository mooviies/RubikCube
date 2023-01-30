#include "vrcaction.h"

#include <QString>
#include <QRandomGenerator>
#include "constexpr.h"

const uint VRCAction::LAYER_MASK_SHIFT = countConsecutiveSetBit((uint)GeneralMask::Operation);

VRCAction::VRCAction(uint flags, bool log)
{
    if(flags == 0)
    {
        _flags = 0;
        _log = false;
        return;
    }

    _log = log;
    auto layer = (Layer)(flags & (uint)LayerMask::All);
    auto option = (Option)(flags & (uint)OptionMask::All);
    auto rotation = (Rotation)(flags &(uint)RotationMask::All);

    _flags = (flags & (uint)GeneralMask::LayerNumber) | (((uint)layer | (uint)option | (uint)rotation) & (uint)GeneralMask::Operation);
}

VRCAction::VRCAction(Layer layer, Option option, Rotation rotation, ushort layerNumber, bool log) : _log(log)
{
    _flags = getFlags(layer, option, rotation, layerNumber);
}

VRCAction& VRCAction::operator=(const VRCAction &other)
{
    _flags = other._flags;
    return *this;
}

VRCAction::Rotation VRCAction::getRotation(bool reversed) const
{
    auto rotation = (Rotation)(_flags & (uint)RotationMask::All);
    if(rotation == Rotation::Turn180)
        return rotation;

    if(reversed)
        return (Rotation)((uint)rotation ^ (uint)RotationMask::All);

    return rotation;
}

VRCAction VRCAction::reversed() const
{
    if(_flags == 0) return VRCAction(0);

    VRCAction action(*this);
    if(action.getRotation() == Rotation::Turn180)
        return action;

    action._flags ^= (uint)RotationMask::All;
    return action;
}

VRCAction VRCAction::withOppositeLayer() const
{
    if(_flags == 0) return VRCAction(0);

    VRCAction action(*this);
    switch(action.getLayer())
    {
        case Layer::Left:
            return action.withLayer(Layer::Right);
        case Layer::Front:
            return action.withLayer(Layer::Back);
        case Layer::Right:
            return action.withLayer(Layer::Left);
        case Layer::Back:
            return action.withLayer(Layer::Front);
        case Layer::Up:
            return action.withLayer(Layer::Down);
        case Layer::Down:
            return action.withLayer(Layer::Up);
        case Layer::Middle:
        case Layer::Equator:
        case Layer::Standing:
        case Layer::CubeX:
        case Layer::CubeY:
        case Layer::CubeZ:
            return action;
    }
}

VRCAction VRCAction::with(Layer layer, Option option, Rotation rotation, ushort layerNumber)
{
    return VRCAction(getFlags(layer, option, rotation, layerNumber));
}

VRCAction VRCAction::withLog(bool log) const
{
    VRCAction action(*this);
    action._log = log;

    return action;
}

VRCAction VRCAction::withLayer(Layer layer) const
{
    if(_flags == 0) return VRCAction(0);

    VRCAction action(*this);
    if(action.getLayer() == layer)
        return action;

    action._flags &= ~(uint)LayerMask::All;
    action._flags |= (uint)layer;
    return action;
}

VRCAction VRCAction::withOption(Option option) const
{
    if(_flags == 0) return VRCAction(0);

    VRCAction action(*this);
    if(action.getOption() == option)
        return action;

    action._flags &= ~(uint)OptionMask::All;
    action._flags |= (uint)option;
    return action;
}

VRCAction VRCAction::withRotation(Rotation rotation) const
{
    if(_flags == 0) return VRCAction(0);

    VRCAction action(*this);
    if(action.getRotation() == rotation)
        return action;

    action._flags &= ~(uint)RotationMask::All;
    action._flags |= (uint)rotation;
    return action;
}

VRCAction VRCAction::withLayerNumber(ushort layerNumber) const
{
    if(_flags == 0) return VRCAction(0);

    VRCAction action(*this);
    if(action.getLayerNumber() == layerNumber)
        return action;

    action._flags &= ~(uint)GeneralMask::LayerNumber;
    action._flags |= ((uint)layerNumber) << LAYER_MASK_SHIFT;
    return action;
}

VRCAction VRCAction::random(uint modelSize)
{
    auto layer = Layer::Left;
    switch(QRandomGenerator::global()->bounded(6))
    {
        case 1:
            layer = Layer::Front;
            break;
        case 2:
            layer = Layer::Right;
            break;
        case 3:
            layer = Layer::Back;
            break;
        case 4:
            layer = Layer::Up;
            break;
        case 5:
            layer = Layer::Down;
            break;
        default:
            break;
    }

    auto rotation = Rotation::Clockwise;
    switch(QRandomGenerator::global()->bounded(3))
    {
        case 1:
            rotation = Rotation::CounterClockwise;
            break;
        case 2:
            rotation = Rotation::Turn180;
            break;
        default:
            break;
    }

    auto option = Option::None;
    if(modelSize >= 3 && QRandomGenerator::global()->generateDouble() < 0.25)
    {
        option = Option::Wide;
    }

    auto layerNumber = QRandomGenerator::global()->bounded((uint)1, modelSize / 2 + 2);

    return VRCAction(layer, option, rotation, layerNumber);
}
