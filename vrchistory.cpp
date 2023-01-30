#include "vrchistory.h"

void VRCHistory::log(VRCAction action)
{
    if(action.isIdentity() || !action.log()) return;

    _actionsToRedo.clear();
    _actionsToUndo.push(action);
}

VRCAction VRCHistory::undo()
{
    if(_actionsToUndo.isEmpty())
        return VRCAction::getIdentity();

    auto action = _actionsToUndo.pop();
    _actionsToRedo.push(action);

    return action.reversed().withLog(false);
}

VRCAction VRCHistory::redo()
{
    if(_actionsToRedo.isEmpty())
        return VRCAction::getIdentity();

    auto action = _actionsToRedo.pop();
    _actionsToUndo.push(action);

    return action.withLog(false);
}

void VRCHistory::clear()
{
    _actionsToRedo.clear();
    _actionsToUndo.clear();
}
