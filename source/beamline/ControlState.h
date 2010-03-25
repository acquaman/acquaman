#ifndef CONTROLSTATE_H
#define CONTROLSTATE_H

#include <QObject>
#include <QList>
#include <QPair>
#include <qdebug.h>
#include "Control.h"

class ControlState : public QObject
{
Q_OBJECT
public:
    explicit ControlState(Control *ctrl, QObject *parent = 0);

signals:

public slots:
    void vomit();

protected:
    QList< QPair<QString, QString> > state_;
    QList< QPair<int, ControlState*> > subState_;

    bool searchChildren(Control *ctrl);
};

#endif // CONTROLSTATE_H
