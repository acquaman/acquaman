#ifndef CLSAMDSSCALERCHANNELDETECTOR_H
#define CLSAMDSSCALERCHANNELDETECTOR_H

class CLSAMDSScaler;

#include "beamline/AMDetector.h"

class CLSAMDSScalerChannelDetector : public AMDetector
{
Q_OBJECT
public:
	/// Constructor takes a name and description as well as the scaler object pointer and the channel index to use (index 0 - 31 for SIS3820)
	CLSAMDSScalerChannelDetector(const QString &name, const QString &description, CLSAMDSScaler *scaler, int channelIndex, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSAMDSScalerChannelDetector();

	/// Returns 0, because there are no axes for the single point detector
	virtual int size(int axisNumber) const;

	/// The basic scaler channels don't require additional power
	virtual bool requiresPower() const { return false; }

	/// Cancelling is not yet implemented for the scaler channels (but it can be in the future)
	virtual bool canCancel() const { return false; }
	/// Clearing is not yet implemented for the scaler channels (but it can be in the future)
	virtual bool canClear() const { return false; }

	/// This has not been implemented yet for this version of the detector
	virtual bool canDoDarkCurrentCorrection() const { return false; }

	/// This detector is built around the continuous ability
	virtual bool canContinuousAcquire() const { return true; }

	/// Returns the current acquisition dwell time from the scaler
	virtual double acquisitionTime() const;
	/// Returns the tolerance for the acquisition time.  Returns the tolerance from the dwellTimeControl_.
	virtual double acquisitionTimeTolerance() const;

	/// No longer important
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns a null string
	virtual QString synchronizedDwellKey() const;

	/// Scaler channels share scaler as their trigger source
	virtual bool sharesDetectorTriggerSource() const { return true; }
	/// Returns the scaler's trigger source
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the scaler's dwell time source
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns the current readMode
	virtual AMDetectorDefinitions::ReadMode readMode() const;

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Returns the channel count
	virtual AMNumber singleReading() const;

	/// Fills a (hopefully) valid pointer to a single double with our current value
	virtual bool data(double *outputValues) const;

	/// Implemented to support returning data from the last acquire(AMDetectorDefinitions::ContinuousMode) call
	virtual AMDSClientDataRequest* lastContinuousData(double seconds) const;
	/// Sets the continuous data window for the next AMDSClientDataRequest
	virtual bool setContinuousDataWindow(double continuousDataWindowSeconds);
	/// Returns the AMDS Buffer Name of this detector if there is one, should not be empty.
	virtual QString amdsBufferName() const;
	virtual int amdsPollingBaseTimeMilliseconds() const;

	/// Returns a AM1DProcessVariableDataSource suitable for viewing
	virtual AMDataSource* dataSource() const { return 0; }

	/// Returns the channel index - this detector is associated with one index on the scaler
	int channelIndex() const;
	/// Returns the iterative channel index accounting for any disabled channels that are numerically lower
	int enabledChannelIndex() const;

	/// Implemented to return a mapping from baseData to the applicable range data. Expects a two vectors in the list.
	virtual AMDetectorContinuousMotionRangeData retrieveContinuousMotionRangeData(const QList<QVector<qint32> > &baseData, int expectedDuration = -1, int threshold = -1);

public slots:
	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);

	/// Sets the readMode
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Scaler channels do not support clearing yet
	virtual bool clear() { return false; }

protected slots:
	/// Determines if the detector is connected from the scaler signals
	void onScalerConnected(bool connected);

	/// Handles changes coming from the scaler's dwell state
	void onScalerDwellStateChanged(double dwellState);

	/// Handles continuous data requests ready in the scaler
	void onScalerAMDSDataReady();

protected:
	/// Simply does a setInitialized() and returns true
	bool initializeImplementation();
	/// Handles acquisition ... only doing continuous right now
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	/// Simply does a setCleanedUp() and returns true
	bool cleanupImplementation();

	/// Checks all of the scaler settings to make sure we're ready for an acquisition
	virtual void checkReadyForAcquisition();

protected:
	/// Holds the current readMode
	AMDetectorDefinitions::ReadMode readMode_;

	/// The pointer to the scaler object
	CLSAMDSScaler *scaler_;
	/// The channel index
	int channelIndex_;
};

#endif // CLSAMDSSCALERCHANNELDETECTOR_H
