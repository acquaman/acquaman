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
	AMAbstractDetectorSet *detectorSet() const { return XASDetectors_;}

	QList<AMAbstractDetector*> usingDetectors() const;

public slots:
	virtual bool addRegion(size_t index, double start, double delta, double end) { return regions_->addRegion(index, start, delta, end);}

	bool setExitSlitGap(double exitSlitGap) { bool rVal = SGMScanConfiguration::setExitSlitGap(exitSlitGap); emit exitSlitGapChanged(exitSlitGap); return rVal; }
	bool setGrating(SGMBeamline::sgmGrating grating) { bool rVal = SGMScanConfiguration::setGrating(grating); emit gratingChanged(grating); return rVal; }
	bool setGrating(int grating) { return setGrating( (SGMBeamline::sgmGrating)grating ); }
	bool setHarmonic(SGMBeamline::sgmHarmonic harmonic) { bool rVal = SGMScanConfiguration::setHarmonic(harmonic); emit harmonicChanged(harmonic); return rVal;}
	bool setHarmonic(int harmonic) { return setHarmonic( (SGMBeamline::sgmHarmonic)harmonic ); }
	bool setUndulatorTracking(bool track){ bool rVal = SGMScanConfiguration::setUndulatorTracking(track); emit undulatorTrackingChanged(track); return rVal; }
	bool setUndulatorTracking(int track){ return setUndulatorTracking( (bool)track); }
	bool setMonoTracking(bool track){ bool rVal = SGMScanConfiguration::setMonoTracking(track); emit monoTrackingChanged(track); return rVal; }
	bool setMonoTracking(int track){ return setMonoTracking( (bool)track ); }
	bool setExitSlitTracking(bool track){ bool rVal = SGMScanConfiguration::setExitSlitTracking(track); emit exitSlitTrackingChanged(track); return rVal; }
	bool setExitSlitTracking(int track){ return setExitSlitTracking( (bool)track ); }
	bool setUsingTEY(bool active) { bool rVal = SGMScanConfiguration::setUsingTEY(active); emit usingTEYChanged(active); return rVal; }
	bool setUsingTEY(int checkedState) { return setUsingTEY( (bool)checkedState);}
	bool setUsingTFY(bool active) { bool rVal = SGMScanConfiguration::setUsingTFY(active); emit usingTFYChanged(active); return rVal; }
	bool setUsingTFY(int checkedState) { return setUsingTFY( (bool)checkedState);}
	bool setUsingPGT(bool active) { bool rVal = SGMScanConfiguration::setUsingPGT(active); emit usingPGTChanged(active); return rVal; }
	bool setUsingPGT(int checkedState) { return setUsingPGT( (bool)checkedState);}

signals:
	void exitSlitGapChanged(double exitSlitGap);
	void gratingChanged(int grating);
	void harmonicChanged(int harmonic);
	void undulatorTrackingChanged(int track);
	void monoTrackingChanged(int track);
	void exitSlitTrackingChanged(int track);
	void usingTEYChanged(bool active);
	void usingTFYChanged(bool active);
	void usingPGTChanged(bool active);

protected:
	AMControlSet *fluxResolutionSet_;
	AMControlSet *trackingSet_;
	AMAbstractDetectorSet *XASDetectors_;

};

#endif // ACQMAN_SGMXASSCANCONFIGURATION_H
