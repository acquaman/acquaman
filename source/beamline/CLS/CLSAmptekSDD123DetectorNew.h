#ifndef CLSAMPTEKSDD123DETECTORNEW_H
#define CLSAMPTEKSDD123DETECTORNEW_H

#include "beamline/AMXRFDetector.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

class CLSAmptekSDD123DetectorNew : public AMXRFDetector
{
Q_OBJECT
public:
	/// Default constructor. Requires the name and base PV of the detector. It builds all the PV's and connects them accordingly.
	CLSAmptekSDD123DetectorNew(const QString &name, const QString &description, const QString &baseName, QObject *parent = 0);

	/// The Ampteks don't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	/// Cancelling is not implemented for the Amptek detectors
	virtual bool canCancel() const { return false; }
	/// Clearing is not currently supported for the Amptek detectors
	virtual bool canClear() const { return false; }

	/// Ampteks are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// The ampteks can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return true; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// The Ampteks share a triggering source  sometimes uses the synchronized dwell time object
	virtual bool sharesDetectorTriggerSource();
	/// Returns the synchronized dwell time trigger source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns false, because the Amptek detectors do not support continuous reads
	virtual bool lastContinuousReading(double *outputValues) const;

	/// Creates an action to enable or disable this amptek for in the array.
	AMAction3* createEnableAction3(bool setEnabled);
	bool isEnabled() const;

	double detectorTemperature() const;

	/// The ampteks do not support elapsed time.
	virtual bool supportsElapsedTime() const { return true; }

public slots:

	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Amptek detectors do not support clearing
	virtual bool clear() { return false; }

signals:
	void detectorTemperatureChanged(double newTemperature);

protected slots:


protected:



	/// The number of channels control
	AMReadOnlyPVControl *mcaChannelsControl_;
	/// The detector temperature control
	AMReadOnlyPVControl *detectorTemperatureControl_;
	/// A binned version of the detector spectrum control
	AMReadOnlyPVControl *binnedSpectrumControl_;
	/// The enable/disable state for this amptek in the array
	AMPVControl *isRequestedControl_;

	/// PV basename for the detector instance
	QString baseName_;
};

#endif // CLSAMPTEKSDD123DETECTORNEW_H
