#ifndef ACQMAN_XASSCANCONFIGURATION_H
#define ACQMAN_XASSCANCONFIGURATION_H

#include <QObject>
#include "ScanConfiguration.h"
#include "beamline/SGMBeamline.h"

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
XASRegion(QObject *parent = 0) : QObject(parent) {bl_ = SGMBeamline::sgm();}
    double start() const { return start_;}
    double delta() const { return delta_;}
    double end() const { return end_;}

public slots:
    bool setStart(double start) {
        if(bl_->energy()->valueOutOfRange(start))
            return FALSE;
        start_ = start;
        return TRUE;
    }
    void setDelta(double delta) { delta_ = delta;}
    bool setEnd(double end) {
        if(bl_->energy()->valueOutOfRange(end))
            return FALSE;
        end_ = end;
        return TRUE;
    }

protected:
    double start_;
    double delta_;
    double end_;
    SGMBeamline *bl_;
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
    double exitSlitGap() const { return exitSlitGap_;}
    SGMBeamline::sgmGrating grating() const { return grating_;}
    bool undulatorTracking() { return undulatorTracking_;}
    bool monoTracking() { return monoTracking_;}
    bool exitSlitTracking() { return exitSlitTracking_;}

public slots:
    bool setStart(size_t index, double start);
    bool setDelta(size_t index, double delta);
    bool setEnd(size_t index, double end);
    bool setRegion(size_t index, XASRegion *region);
    bool addRegion(size_t index, XASRegion *region);
    bool addRegion(size_t index, double start, double delta, double end);
    bool deleteRegion(size_t index);
    bool setExitSlitGap(double exitSlitGap);
    bool setGrating(SGMBeamline::sgmGrating grating) {grating_ = grating; return TRUE;}
    bool setUndulatorTracking(bool track){undulatorTracking_ = track; return TRUE;}
    bool setMonoTracking(bool track){monoTracking_ = track; return TRUE;}
    bool setExitSlitTracking(bool track){exitSlitTracking_ = track; return TRUE;}

protected:
    QList<XASRegion*> regions_;
    double exitSlitGap_;
    SGMBeamline::sgmGrating grating_;
    bool undulatorTracking_;
    bool monoTracking_;
    bool exitSlitTracking_;
};

#endif // XASSCANCONFIGURATION_H
