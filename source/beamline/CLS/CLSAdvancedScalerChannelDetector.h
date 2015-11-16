/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef CLSADVANCEDSCALERCHANNELDETECTOR_H
#define CLSADVANCEDSCALERCHANNELDETECTOR_H

#include "beamline/CLS/CLSBasicScalerChannelDetector.h"

class CLSScalerChannelDetector : public AMDetector
{
Q_OBJECT
public:
	/// Constructor takes a name and description as well as the scaler object pointer and the channel index to use (index 0 - 31 for SIS3820)
	CLSScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSScalerChannelDetector();

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

	/// STARTING TESTING WITH THIS AS FALSE, NEEDS TO BE FIXED IN THE FUTURE
	virtual bool sharesDetectorTriggerSource() const { return true; }
	/// FOR TESTING, THIS WILL RETURN A NULL POINTER
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// FOR TESTING THIS WILL RETURN A NULL POINTER
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns the current readMode
	virtual AMDetectorDefinitions::ReadMode readMode() const;

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Returns the channel count
	virtual AMNumber singleReading() const;

	/// Fills a (hopefully) valid pointer to a single double with our current value
	virtual bool data(double *outputValues) const;

	/// Implemented to support returning data from the last acquire(AMDetectorDefinitions::ContinuousMode) call
	virtual AMDSClientDataRequest* lastContinuousData(double seconds) const;
	virtual bool setContinuousDataWindow(double continuousDataWindowSeconds);

	/// Returns a AM1DProcessVariableDataSource suitable for viewing
	virtual AMDataSource* dataSource() const { return 0; }

	/// Returns the channel index - this detector is associated with one index on the scaler
	int channelIndex() const;
	/// Returns the iterative channel index accounting for any disabled channels that are numerically lower
	int enabledChannelIndex() const;

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
	/// Updates the detector status from the scaler's scanning status
	virtual void onScalerScanningChanged(bool isScanning);

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
	CLSSIS3820Scaler *scaler_;
	/// The channel index
	int channelIndex_;
};

class CLSAdvancedScalerChannelDetector : public CLSBasicScalerChannelDetector
{
Q_OBJECT
public:
	/// Constructor takes a name and description as well as the scaler object pointer and the channel index to use (index 0 - 31 for SIS3820)
 	virtual ~CLSAdvancedScalerChannelDetector();
	CLSAdvancedScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex, QObject *parent = 0);

	/// Advanced scaler channels are designed to implement continuous acquire.
	virtual bool canContinuousAcquire() const { return true; }

	/// Returns SingleRead or ContinuousRead depending on the current mode
	virtual AMDetectorDefinitions::ReadMode readMode() const { return readMode_; }

	/// Fills the (hopefully) valid pointer to a single double with our current value
	virtual bool data(double *outputValues) const;

	/// Implemented to support returning data from the last acquire(AMDetectorDefinitions::ContinuousMode) call
	virtual AMDSClientDataRequest* lastContinuousData(double seconds) const;

	/// Returns boolean indicating that this particular implementation of AMDetector does not support dark current correction.
	virtual bool canDoDarkCurrentCorrection() const { return false; }

	/// A HACK, NEED TO FIX THIS
	virtual bool sharesDetectorTriggerSource() const { return false; }
	/// A HACK, NEED TO FIX THIS
	virtual AMDetectorTriggerSource* detectorTriggerSource() { return 0; }
	/// A HACK, NEED TO FIX THIS
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource() { return 0; }

	int channelIndex() const { return channelIndex_; }
	int enabledChannelIndex() const;

public slots:
	/// Note that this will change the read mode for the entire scaler, not just this channel. The read mode can be changed to either SingleRead or ContinuousRead. Requesting to change to the same readMode returns true and the readModeChanged() signal is immediately emitted
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

protected slots:
	/// Handles watching signals from the scaler while we're switching modes
	void onModeSwitchSignal();

	/// Updates the detector status from the scaler's scanning status
	virtual void onScalerScanningChanged(bool isScanning);
	virtual void onReadingChanged();

	void onScansPerBufferChanged(int scansPerBuffer);
	void onTotalScansChanged(int totalScans);

	bool triggerChannelAcquisition();

	void onScalerConnectedConfirmReadMode(bool connected);

	void onScalerAMDSDataReady();

protected:
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);

	virtual void checkReadyForAcquisition();

protected:
	AMDetectorDefinitions::ReadMode readMode_;

	bool switchingReadModes_;

	QVector<double> continuousData_;
	int continuousSize_;
};

#endif // CLSADVANCEDSCALERCHANNELDETECTOR_H
