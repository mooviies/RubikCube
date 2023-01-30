#ifndef VRCCONTROLLER_H
#define VRCCONTROLLER_H

#include <QObject>
#include <QQueue>
#include <QList>

#include "vrcmodel.h"
#include "vrchistory.h"
#include "vrcaction.h"

class VRCController : public QObject
{
    Q_OBJECT

public:
    VRCController(VRCModel *model, VRCView *view);
    void setModelView(VRCModel *model, VRCView *view);
    void execute(VRCAction action);
    void execute(const QList<VRCAction>& actions);
    void update();
    void undo();
    void redo();

signals:
    void actionQueueEmptied();

private:
    VRCModel *_model;
    VRCView *_view;
    VRCHistory _history;
    QQueue<VRCAction> _actions;
};

#endif // VRCCONTROLLER_H
