#ifndef XASSCANCONFIGURATION_H
#define XASSCANCONFIGURATION_H

#include <QObject>
#include "ScanConfiguration.h"

/*
  FIX ME UP! I want to be able to have elastic regions ... start value is last end plus this delta.
class ContinuousRegion
{
public:
    ContinuousRegion(){};
    double startVal_;
    QList<double> values_;
};
*/

class XASRegion : public QObject
{
Q_OBJECT
Q_PROPERTY(double start READ start WRITE setStart)
Q_PROPERTY(double delta READ delta WRITE setDelta)
Q_PROPERTY(double end READ end WRITE setEnd)

public:
XASRegion(QObject *parent = 0) : QObject(parent) {;}
    double start() const { return start_;}
    double delta() const { return delta_;}
    double end() const { return end_;}

public slots:
    void setStart(double start) { start_ = start;}
    void setDelta(double delta) { delta_ = delta;}
    void setEnd(double end) { end_ = end;}

protected:
    double start_;
    double delta_;
    double end_;
};

class XASScanConfiguration : public ScanConfiguration
{
    Q_OBJECT
public:
    XASScanConfiguration(QObject *parent = 0);
    double start(size_t index) const;
    double delta(size_t index) const;
    double end(size_t index) const;
    XASRegion* region(size_t index) const;

public slots:
    bool setStart(size_t index, double start);
    bool setDelta(size_t index, double delta);
    bool setEnd(size_t index, double end);
    bool setRegion(size_t index, XASRegion *region);
    bool addRegion(size_t index, XASRegion *region);
    bool addRegion(size_t index, double start, double delta, double end);
    bool deleteRegion(size_t index);

protected:
    QList<XASRegion*> regions_;
};

#endif // XASSCANCONFIGURATION_H
