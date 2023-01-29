#include "vrccontroller.h"

VRCController::VRCController(VRCModel *model) : _model(model)
{

}

void VRCController::setModel(VRCModel *model)
{
    _model = model;
    _history.clear();
}

void VRCController::execute(VRCAction action)
{
    if(_model->execute(action))
        _history.log(action);
}
