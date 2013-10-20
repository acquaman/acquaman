#ifndef CLSBASICSCALERCHANNELDETECTOR_H
#define CLSBASICSCALERCHANNELDETECTOR_H

#include "beamline/AMDetector.h"

class CLSSIS3820Scaler;

class CLSBasicScalerChannelDetector : public AMDetector
{
Q_OBJECT
public:
	/// Constructor takes a name and description as well as the scaler object pointer and the channel index to use (index 0 - 31 for SIS3820)
	CLSBasicScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex, QObject *parent = 0);

	/// Returns 0, because there are no axes for the single point detector
	virtual int size(int axisNumber) const;

	/// The basic scaler channels don't require additional power
	virtual bool requiresPower() const { return false; }

	/// Cancelling is not yet implemented for the scaler channels (but it can be in the future)
	virtual bool canCancel() const { return false; }
	/// Clearing is not yet implemented for the scaler channels (but it can be in the future)
	virtual bool canClear() const { return false; }

	/// Basic scaler channels cannot continuous acquire. This needs to be implemented in a subclass.
	virtual bool canContinuousAcquire() const { return false; }

	/// Returns the current acquisition dwell time from the scaler
	virtual double acquisitionTime() const;

	/// The scaler channels can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return true; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// The scaler channels always share a triggering source (with eachother through the main scaler trigger) and sometimes uses the synchronized dwell time object
	virtual bool sharesDetectorTriggerSource() { return true; }
	/// Returns the current shared detector trigger source (either the main scaler trigger source or the synchronized dwell time trigger source)
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the current shared detector dwell time source (either the main scaler dwell time source or the synchronized dwell time dwell time source)
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type (support for ContinuousRead will be done in a subclass)
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Returns the channel count
	virtual AMNumber singleReading() const;

	/// Returns false, because the scaler channels do not support continuous reads (in this class)
	virtual bool lastContinuousReading(double *outputValues) const;

	/// Returns a (hopefully) valid pointer to a single double with our current value
	virtual const double* data() const;

	/// Returns a AM1DProcessVariableDataSource suitable for viewing
	virtual AMDataSource* dataSource() const { return 0; }

public slots:
	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);

	/// The read mode cannot be changed for basic scaler channels
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Scaler channels do not support clearing yet
	virtual bool clear() { return false; }

protected slots:
	/// Determines if the detector is connected from the scaler signals
	void onScalerConnected(bool connected);
	/// Updates the detector status from the scaler's scanning status
	virtual void onScalerScanningChanged(bool isScanning);

	/// Handles triggering the actual acquisition even if the scaler needs to switch to single read from continuous
	bool triggerScalerAcquisition(bool isContinuous);

protected:
	bool initializeImplementation();
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	bool cleanupImplementation();

	/// Checks all of the scaler settings to make sure we're ready for an acquisition
	void checkReadyForAcquisition();

protected:
	/// Bool handling whether the detector was connected.
	bool wasConnected_;

	/// The pointer to the scaler object
	CLSSIS3820Scaler *scaler_;
	/// The channel index
	int channelIndex_;

	///// The list of all the raw spectrum data sources.
	//AM1DProcessVariableDataSource *spectrumDataSource_;

	/// Memory storage for values (used mainly for the data call).
	double *data_;
};

#endif // CLSBASICSCALERCHANNELDETECTOR_H
