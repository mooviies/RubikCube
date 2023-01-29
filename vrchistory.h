#ifndef VRCHISTORY_H
#define VRCHISTORY_H

#include <QStack>

#include "vrcaction.h"

class VRCHistory
{
public:
    VRCHistory() {}

    void log(VRCAction action);
    VRCAction undo();
    VRCAction redo();
    void clear();

private:
    QStack<VRCAction> _actionsToUndo;
    QStack<VRCAction> _actionsToRedo;
};

#endif // VRCHISTORY_H
