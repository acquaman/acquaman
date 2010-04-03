#ifndef ACQMAN_SGMXASSCANCONFIGURATION_H
#define ACQMAN_SGMXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"
#include "SGMScanConfiguration.h"

class SGMXASScanConfiguration : public AMXASScanConfiguration, public SGMScanConfiguration
{
    Q_OBJECT
public:
    SGMXASScanConfiguration(QObject *parent=0);

	AMControlSet *fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet *trackingSet() const { return trackingSet_;}

public slots:
    virtual bool addRegion(size_t index, AMXASRegion *region) {return AMXASScanConfiguration::addRegion(index, region);}
    virtual bool addRegion(size_t index, double start, double delta, double end);

    bool setExitSlitGap(double exitSlitGap) { bool rVal = SGMScanConfiguration::setExitSlitGap(exitSlitGap); emit exitSlitGapChanged(exitSlitGap); return rVal; }
    bool setGrating(SGMBeamline::sgmGrating grating) { bool rVal = SGMScanConfiguration::setGrating(grating); emit gratingChanged(grating); return rVal; }
    bool setGrating(int grating) { return setGrating(grating); }
    bool setUndulatorTracking(bool track){ bool rVal = SGMScanConfiguration::setUndulatorTracking(track); emit undulatorTrackingChanged(track); return rVal; }
    bool setUndulatorTracking(int track){ return setUndulatorTracking((bool)track); }
    bool setMonoTracking(bool track){ bool rVal = SGMScanConfiguration::setMonoTracking(track); emit monoTrackingChanged(track); return rVal; }
    bool setMonoTracking(int track){ return setMonoTracking(track); }
    bool setExitSlitTracking(bool track){ bool rVal = SGMScanConfiguration::setExitSlitTracking(track); emit exitSlitTrackingChanged(track); return rVal; }
    bool setExitSlitTracking(int track){ return setExitSlitTracking(track); }

signals:
    void exitSlitGapChanged(double exitSlitGap);
    void gratingChanged(int grating);
    void undulatorTrackingChanged(int track);
    void monoTrackingChanged(int track);
    void exitSlitTrackingChanged(int track);

protected:
	AMControlSet *fluxResolutionSet_;
	AMControlSet *trackingSet_;

};

#endif // ACQMAN_SGMXASSCANCONFIGURATION_H
