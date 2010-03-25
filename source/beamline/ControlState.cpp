#include "ControlState.h"

ControlState::ControlState(Control *ctrl, QObject *parent) :
    QObject(parent)
{
    searchChildren(ctrl);
}

void ControlState::searchChildren(Control *ctrl)
{
    for(int x = 0; x < ctrl->numChildren(); x++)
        qDebug() << "Has child named " << ctrl->children().at(x)->objectName();
}
