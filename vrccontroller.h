#ifndef VRCCONTROLLER_H
#define VRCCONTROLLER_H

#include <QQueue>

#include "vrcmodel.h"
#include "vrchistory.h"
#include "vrcaction.h"

class VRCController
{
public:
    VRCController(VRCModel *model, VRCView *view);
    void setModel(VRCModel *model);
    void execute(VRCAction action);
    void update();

private:
    VRCModel *_model;
    VRCView *_view;
    VRCHistory _history;
    QQueue<VRCAction> _actions;
};

#endif // VRCCONTROLLER_H
