#ifndef ACQMAN_AMCONTROLSET_H
#define ACQMAN_AMCONTROLSET_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>
//#include "acquaman/AMScanConfiguration.h"
#include "acquaman/AMRegion.h"

class AMControlSet : public QObject
{
Q_OBJECT
public:
    explicit AMControlSet(QObject *parent = 0);

    QString name() const { return name_;}
    //const QList<const AMControl*> controls() const { return ctrls_;}
    QList<AMControl*> controls() { return ctrls_;}

signals:

public slots:
    void setName(const QString &name) { name_ = name;}
    bool addControl(AMControl* ctrl);
    bool removeControl(AMControl* ctrl);

protected:
    QString name_;
    QList<AMControl*> ctrls_;
};

class AMControlOptimization : public QObject
{
    Q_OBJECT
public:
    AMControlOptimization(QObject *parent=0) : QObject(parent){;}

    QString name() const { return name_;}
    virtual QMap<double, double> curve(QList<QVariant> stateParameters, QList<AMRegion*> contextParameters);

public slots:
    void setName(const QString &name) { name_ = name;}

protected:
    QString name_;
};

class AMControlOptimizationSet : public AMControlSet
{
    Q_OBJECT
public:
    explicit AMControlOptimizationSet(QObject *parent=0) : AMControlSet(parent){;}

protected:
    QList<AMControlOptimization*> outputs_;
};

#endif // ACQMAN_AMCONTROLSET_H
