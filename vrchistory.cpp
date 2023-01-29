#include "vrchistory.h"

void VRCHistory::log(VRCAction action)
{
    if(action.isIdentity()) return;

    _actionsToRedo.clear();
    _actionsToUndo.push(action);
}

VRCAction VRCHistory::undo()
{
    if(_actionsToUndo.isEmpty())
        return VRCAction::getIdentity();

    auto action = _actionsToUndo.pop();
    _actionsToRedo.push(action);

    return action;
}

VRCAction VRCHistory::redo()
{
    if(_actionsToRedo.isEmpty())
        return VRCAction::getIdentity();

    auto action = _actionsToRedo.pop();
    _actionsToUndo.push(action);

    return action;
}

void VRCHistory::clear()
{
    _actionsToRedo.clear();
    _actionsToUndo.clear();
}
