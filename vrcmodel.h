#ifndef VRCMODEL_H
#define VRCMODEL_H

#include <QMap>
#include "vrcaction.h"
#include "vrcface.h"
#include "vrcview.h"

class VRCModel
{
public:
    VRCModel(uint size);
    ~VRCModel();
    const VRCFace getFace(VRCFace::Side side) const { return *_cube[(uint)side]; }
    uint getSize() const { return _size; }
    uint getMaxLayerNumber() const { return _maxLayerNumber; }

    QList<VRCFace*>::ConstIterator begin() const { return _cube.constBegin(); }
    QList<VRCFace*>::ConstIterator end() const { return _cube.constEnd(); }

    double getCost();
    void setView(VRCView *view);
    bool execute(VRCAction action);

private:
    VRCFace& getFace(VRCFace::Side side) { return *_cube[(uint)side]; }
    bool parseLayer(VRCAction::Layer &layer, VRCAction::Option &option, ushort &layerNumber);
    bool applyRotation(VRCAction::Layer layer, VRCAction::Option option, VRCAction::Rotation rotation, VRCAction::Rotation rotationReversed, ushort layerNumber);

private:
    uint _size;
    uint _maxLayerNumber;
    QList<VRCFace*> _cube;

    VRCView *_view;
};

#endif // VRCMODEL_H
