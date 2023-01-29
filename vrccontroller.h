#ifndef VRCCONTROLLER_H
#define VRCCONTROLLER_H

#include "vrcmodel.h"
#include "vrchistory.h"
#include "vrcaction.h"

class VRCController
{
public:
    VRCController(VRCModel *model);
    void setModel(VRCModel *model);
    void execute(VRCAction action);

private:
    VRCModel *_model;
    VRCHistory _history;
};

#endif // VRCCONTROLLER_H
