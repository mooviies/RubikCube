#include "vrccontroller.h"

VRCController::VRCController(VRCModel *model, VRCView *view) : _model(model), _view(view)
{

}

void VRCController::setModelView(VRCModel *model, VRCView *view)
{
    _model = model;
    _view = view;
    _history.clear();
    _actions.clear();
}

void VRCController::execute(VRCAction action)
{
    if(action.isIdentity()) return;
    _actions.enqueue(action);
}

void VRCController::execute(const QList<VRCAction>& actions)
{
    for(auto action : actions)
        execute(action);
}

void VRCController::update()
{
    if(!_view->isAnimating() && !_actions.isEmpty())
    {
        auto action = _actions.head();
        if(_model->execute(action))
            _history.log(_actions.dequeue());

        if(_actions.isEmpty())
            emit actionQueueEmptied();
    }
}

void VRCController::undo()
{
    execute(_history.undo());
}

void VRCController::redo()
{
    execute(_history.redo());
}
