#include "vrccontroller.h"

VRCController::VRCController(VRCModel *model, VRCView *view) : _model(model), _view(view)
{

}

void VRCController::setModel(VRCModel *model)
{
    _model = model;
    _history.clear();
}

void VRCController::execute(VRCAction action)
{
    _actions.enqueue(action);
}

void VRCController::update()
{
    if(!_view->isAnimating() && !_actions.isEmpty())
    {
        auto action = _actions.head();
        if(_model->execute(action))
            _history.log(_actions.dequeue());
    }
}
