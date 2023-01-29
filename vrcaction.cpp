#include "vrcaction.h"

#include <QString>
#include "constexpr.h"

const uint VRCAction::LAYER_MASK_SHIFT = countConsecutiveSetBit((uint)GeneralMask::Operation);

VRCAction::VRCAction(uint flags)
{
    if(flags == 0)
    {
        _flags = 0;
        return;
    }

    auto layer = (Layer)(flags & (uint)LayerMask::All);
    auto option = (Option)(flags & (uint)OptionMask::All);
    auto rotation = (Rotation)(flags &(uint)RotationMask::All);

    _flags = (flags & (uint)GeneralMask::LayerNumber) | (((uint)layer | (uint)option | (uint)rotation) & (uint)GeneralMask::Operation);
}

VRCAction::VRCAction(Layer layer, Option option, Rotation rotation, ushort layerNumber)
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

VRCAction VRCAction::with(Layer layer, Option option, Rotation rotation, ushort layerNumber)
{
    return VRCAction(getFlags(layer, option, rotation, layerNumber));
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
