#ifndef VESPERS13ELEMENTGEDETECTOR_H
#define VESPERS13ELEMENTGEDETECTOR_H

#include "beamline/AMXRFDetector.h"

/// Implementation of the AMXRFDetector for the 13 element germanium detector on VESPERS (shared with IDEAS).
class VESPERS13ElementGeDetector : public AMXRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector.  It builds all the necessary components.
	VESPERS13ElementGeDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERS13ElementGeDetector();

	/// The germanium detector doesn't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	/// Cancelling is implemented for the germanium detectors
	virtual bool canCancel() const { return true; }
	/// Clearing is not currently supported for the germanium detectors
	virtual bool canClear() const { return false; }

	/// Germanium detectors are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// The germanium is not configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns the an empty string.
	virtual QString synchronizedDwellKey() const;

	/// The germanium detector doesn't share a triggering source
	virtual bool sharesDetectorTriggerSource() const;
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
	/// Returns AMPVControl value for peaking time
	double peakingTime() const { return peakingTimeControl_->value(); }

signals:
	/// Notifier that the maximum energy has changed.  Value passed in keV.
	void maximumEnergyChanged(double);
	/// Notifier that the peaking time has changed.  Value passed in us.
	void peakingTimeChanged(double);

public slots:
	/// The read mode cannot be changed for Amptek detectors
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

#endif // VESPERS13ELEMENTGEDETECTOR_H
