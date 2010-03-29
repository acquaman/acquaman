#ifndef ACQMAN_CONTROLSTATE_H
#define ACQMAN_CONTROLSTATE_H

#include <QObject>
#include <QList>
#include <QPair>
#include <qdebug.h>
#include "AMControl.h"

class AMControlStatus : public QObject
{
Q_OBJECT
public:
    explicit AMControlStatus(QString name = "", int can = 0, int should = 0, double value = -1, double tolerance = 0, QObject *parent = 0) : QObject(parent){
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

class AMControlState : public QObject
{
Q_OBJECT
public:
    explicit AMControlState(AMControl *ctrl, QObject *parent = 0);

signals:

public slots:
    void vomit();
    bool restore(AMControl *ctrl);
    bool restoreList(AMControl *ctrl, QList<QString> *maskList);

protected:
//    QList< QPair<QString, QString> > state_;
    QString name_;
    QList<AMControlStatus*> state_;
    QList< QPair<int, AMControlState*> > subState_;

    bool searchChildren(AMControl *ctrl);
    bool restoreListInternal(AMControl * ctrl, QList<QString> *maskList = NULL, bool useList = FALSE);
};

#endif // ACQMAN_CONTROLSTATE_H
