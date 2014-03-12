#ifndef VESPERSSINGLEELEMENTVORTEXDETECTOR_H
#define VESPERSSINGLEELEMENTVORTEXDETECTOR_H

#include "beamline/AMXRFDetector.h"

/// Implentation of AMXRFDetector for the single element vortex detector used on VESPERS.
class VESPERSSingleElementVortexDetector : public AMXRFDetector
{
	Q_OBJECT

public:
	/// Default constructor. Requires the name and description of the detector. It builds all the PV's and connects them accordingly.
 	virtual ~VESPERSSingleElementVortexDetector();
	VESPERSSingleElementVortexDetector(const QString &name, const QString &description, QObject *parent = 0);

	/// The Vortex doesn't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	/// Cancelling is implemented for the Vortex detectors
	virtual bool canCancel() const { return true; }
	/// Clearing is not currently supported for the Vortex detectors
	virtual bool canClear() const { return false; }

	/// Vortex detectors are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// The Vortex can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return true; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// The Vortex detectors share a triggering source  sometimes uses the synchronized dwell time object
	virtual bool sharesDetectorTriggerSource();
	/// Returns the synchronized dwell time trigger source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns false, because the Vortex detectors do not support continuous reads
	virtual bool lastContinuousReading(double *outputValues) const;

	/// The vortex detectors support elapsed time.
	virtual bool supportsElapsedTime() const { return true; }

	/// Returns the maximum energy for this detector.  Result returned in keV.
	double maximumEnergy() const { return maximumEnergyControl_->value(); }
	/// Returns the peaking time for this detector.  Result returned in us.
	double peakingTime() const { return peakingTimeControl_->value(); }

signals:
	/// Notifier that the maximum energy has changed.  Value passed in keV.
	void maximumEnergyChanged(double);
	/// Notifier that the peaking time has changed.  Value passed in us.
	void peakingTimeChanged(double);

public slots:

	/// The read mode cannot be changed for Vortex detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Vortex detectors do not support clearing
	virtual bool clear() { return false; }

	/// Sets the maximum energy.  Expects \param energy in keV.
	void setMaximumEnergy(double energy);
	/// Sets the peaking time.  Expects \param time in us.
	void setPeakingTime(double time);

protected slots:
	/// Handles changing the scale for the raw spectra sources when the maximum energy changes.
	void onMaximumEnergyChanged(double newMaximum);

protected:
	/// The maximum energy control.
	AMPVControl *maximumEnergyControl_;
	/// The peaking time control.
	AMPVControl *peakingTimeControl_;
};

#endif // VESPERSSINGLEELEMENTVORTEXDETECTOR_H