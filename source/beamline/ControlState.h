#ifndef ACQMAN_CONTROLSTATE_H
#define ACQMAN_CONTROLSTATE_H

#include <QObject>
#include <QList>
#include <QPair>
#include <qdebug.h>
#include "Control.h"

class ControlStatus : public QObject
{
Q_OBJECT
public:
    explicit ControlStatus(QString name = "", int can = 0, int should = 0, double value = -1, double tolerance = 0, QObject *parent = 0) : QObject(parent){
        name_ = name;
        can_ = can;
        should_ = should;
        value_ = value;
        tolerance_ = tolerance;
    }

    QString name() const { return name_;}
    int can() const { return can_;}
    int should() const { return should_;}
    double value() const { return value_;}
    double tolerance() const { return tolerance_;}

protected:
    QString name_;
    int can_;
    int should_;
    double value_;
    double tolerance_;
};

class ControlState : public QObject
{
Q_OBJECT
public:
    explicit ControlState(Control *ctrl, QObject *parent = 0);

signals:

public slots:
    void vomit();
    bool restore(Control *ctrl);
    bool restoreList(Control *ctrl, QList<QString> *maskList);

protected:
//    QList< QPair<QString, QString> > state_;
    QString name_;
    QList<ControlStatus*> state_;
    QList< QPair<int, ControlState*> > subState_;

    bool searchChildren(Control *ctrl);
    bool restoreListInternal(Control * ctrl, QList<QString> *maskList = NULL, bool useList = FALSE);
};

#endif // CONTROLSTATE_H
