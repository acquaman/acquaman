#ifndef CONTROLSTATE_H
#define CONTROLSTATE_H

#include <QObject>
#include <QList>
#include <QMap>
#include <qdebug.h>
#include "Control.h"

class ControlState : public QObject
{
Q_OBJECT
public:
    explicit ControlState(Control *ctrl, QObject *parent = 0);

signals:

public slots:

protected:
    QList< QMap<QString, QString> > state_;

    void searchChildren(Control *ctrl);
};

#endif // CONTROLSTATE_H
