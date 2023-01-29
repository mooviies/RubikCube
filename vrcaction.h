#ifndef VRCACTION_H
#define VRCACTION_H

#include <QtGlobal>

class VRCAction
{
public:
    enum class Layer
    {
        Left     = 0x00000001,
        Front    = 0x00000002,
        Right    = 0x00000004,
        Back     = 0x00000008,
        Up       = 0x00000010,
        Down     = 0x00000020,
        Middle   = 0x00000040,
        Equator  = 0x00000080,
        Standing = 0x00000100,
        CubeX    = 0x00000200,
        CubeY    = 0x00000400,
        CubeZ    = 0x00000800
    };

    enum class LayerMask
    {
        Face   = (uint)Layer::Left | (uint)Layer::Front | (uint)Layer::Right | (uint)Layer::Back | (uint)Layer::Up | (uint)Layer::Down,
        Center = (uint)Layer::Middle | (uint)Layer::Equator | (uint)Layer::Standing,
        Cube   = (uint)Layer::CubeX | (uint)Layer::CubeY | (uint)Layer::CubeZ,
        All    = Face | Center | Cube
    };

    enum class Option
    {
        None = 0x00000000,
        Wide = 0x00001000
    };

    enum class OptionMask
    {
        All = (uint)Option::Wide
    };

    enum class Rotation
    {
        Clockwise        = 0x00004000,
        CounterClockwise = 0x00008000,
        Turn180          = Clockwise | CounterClockwise,
    };

    enum class RotationMask
    {
        All = (uint)Rotation::Clockwise | (uint)Rotation::CounterClockwise | (uint)Rotation::Turn180
    };

    enum class GeneralMask
    {
        Operation   = (uint)LayerMask::All | (uint)OptionMask::All | (uint)RotationMask::All,
        LayerNumber = ~Operation
    };

    static VRCAction getIdentity() { return VRCAction(0); }

    VRCAction(uint flags);
    VRCAction(Layer layer, Option option, Rotation rotation, ushort layerNumber);
    VRCAction(const VRCAction &other) { _flags = other._flags; }

    VRCAction& operator=(const VRCAction &other);
    bool operator==(const VRCAction &other) const { return _flags == other._flags; }
    bool operator!=(const VRCAction &other) const { return _flags == other._flags; }

    bool isIdentity() const { return _flags == 0; }
    Layer getLayer() const { return (Layer)(_flags & (uint)LayerMask::All); }
    Option getOption() const { return (Option)(_flags & (uint)OptionMask::All); }
    Rotation getRotation(bool reversed = false) const;
    ushort getLayerNumber() const { return (ushort)(_flags >> LAYER_MASK_SHIFT); }

    // All the following operations will return the identity if executed on the identity
    // Create a new VRCAction in this case
    VRCAction reversed() const;
    VRCAction withLayer(Layer layer) const;
    VRCAction withOption(Option option) const;
    VRCAction withRotation(Rotation rotation) const;
    VRCAction withLayerNumber(ushort layerNumber) const;

private:
    const static uint LAYER_MASK_SHIFT;
    uint _flags;
};

#endif // VRCACTION_H
