#ifndef ACQMAN_XASSCANCONFIGURATION_H
#define ACQMAN_XASSCANCONFIGURATION_H

#include <QObject>
#include "AMScanConfiguration.h"
#include "beamline/AMSGMBeamline.h"

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

class AMXASScanConfiguration : public AMScanConfiguration
{
    Q_OBJECT
public:    
    AMXASScanConfiguration(QObject *parent = 0);
    double start(size_t index) const;
    double delta(size_t index) const;
    double end(size_t index) const;
    AMXASRegion* region(size_t index) const;
    QList<AMXASRegion*> regions() { return regions_;}

    double exitSlitGap() const { return exitSlitGap_;}
    AMSGMBeamline::sgmGrating grating() const { return grating_;}
    bool undulatorTracking() { return undulatorTracking_;}
    bool monoTracking() { return monoTracking_;}
    bool exitSlitTracking() { return exitSlitTracking_;}

public slots:
    bool setStart(size_t index, double start);
    bool setDelta(size_t index, double delta);
    bool setEnd(size_t index, double end);
    bool setRegion(size_t index, AMXASRegion *region);
    bool addRegion(size_t index, AMXASRegion *region);
    bool addRegion(size_t index, double start, double delta, double end);
    bool deleteRegion(size_t index);
    bool setExitSlitGap(double exitSlitGap);
    bool setGrating(AMSGMBeamline::sgmGrating grating) {grating_ = grating; return TRUE;}
    bool setUndulatorTracking(bool track){undulatorTracking_ = track; return TRUE;}
    bool setMonoTracking(bool track){monoTracking_ = track; return TRUE;}
    bool setExitSlitTracking(bool track){exitSlitTracking_ = track; return TRUE;}

protected:
    QList<AMXASRegion*> regions_;
    QList<AMControlSet*> groups_;
    double exitSlitGap_;
    AMSGMBeamline::sgmGrating grating_;
    bool undulatorTracking_;
    bool monoTracking_;
    bool exitSlitTracking_;
};

#endif // XASSCANCONFIGURATION_H
