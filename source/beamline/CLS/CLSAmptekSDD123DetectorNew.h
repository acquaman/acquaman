#ifndef CLSAMPTEKSDD123DETECTORNEW_H
#define CLSAMPTEKSDD123DETECTORNEW_H

#include "beamline/AMDetector.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

class CLSAmptekSDD123DetectorNew : public AMDetector
{
Q_OBJECT
public:
	/// Default constructor. Requires the name and base PV of the detector. It builds all the PV's and connects them accordingly.
	CLSAmptekSDD123DetectorNew(const QString &name, const QString &description, const QString &baseName, QObject *parent = 0);

	/// The Ampteks don't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	///// Returns true if the detector is currently acquiring by checking the status control.
	//virtual bool isAcquiring() const;
	/// Cancelling is not implemented for the Amptek detectors
	virtual bool canCancel() const { return false; }
	/// Clearing is not currently supported for the Amptek detectors
	virtual bool canClear() const { return false; }

	/// Ampteks are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// Returns the current acquisition dwell time from the integration time control
	virtual double acquisitionTime() const;

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

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Returns the total count (sum of all bins) as the single reading
	virtual AMNumber singleReading() const;

	/// Returns false, because the Amptek detectors do not support continuous reads
	virtual bool lastContinuousReading(double *outputValues) const;

	/// Returns a (hopefully) valid pointer to a block of detector data in row-major order (first axis varies slowest)
	virtual const double* data() const;

	/// Returns a AM1DProcessVariableDataSource suitable for viewing
	virtual AMDataSource* dataSource() const { return spectrumDataSource_; }

	/// Creates an action to enable or disable this amptek for in the array.
	AMAction3* createEnableAction3(bool setEnabled);
	bool isEnabled() const;

	double detectorTemperature() const;

public slots:
	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);

	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Amptek detectors do not support clearing
	virtual bool clear() { return false; }

signals:
	void detectorTemperatureChanged(double newTemperature);

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out
	void onControlsTimedOut();

	/// Handles changes in the spectrum control
	void onSpectrumControlChanged();

	/// Handles changes in the status control
	void onStatusControlChanged();

protected:
	bool initializeImplementation();
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	bool cleanupImplementation();

protected:
	/// Bool handling whether the detector was connected.
	bool wasConnected_;

	/// The status control
	AMReadOnlyPVControl *statusControl_;
	/// The number of channels control
	AMReadOnlyPVControl *mcaChannelsControl_;
	/// The integration time control
	AMPVControl *integrationTimeControl_;
	/// The detector temperature control
	AMReadOnlyPVControl *detectorTemperatureControl_;
	/// The detector start control
	AMPVControl *startAcquisitionControl_;
	/// The detector spectrum control
	AMReadOnlyPVControl *spectrumControl_;
	/// A binned version of the detector spectrum control
	AMReadOnlyPVControl *binnedSpectrumControl_;
	/// The enable/disable state for this amptek in the array
	AMPVControl *isRequestedControl_;

	/// The master set of controls
	AMControlSet *allControls_;

	/// The list of all the raw spectrum data sources.
	AM1DProcessVariableDataSource *spectrumDataSource_;

	/// PV basename for the detector instance
	QString baseName_;

	/// Memory storage for values (used mainly for the data call).
	QVector<double> data_;
};

#endif // CLSAMPTEKSDD123DETECTORNEW_H
