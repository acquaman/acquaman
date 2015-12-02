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


#ifndef CLSAMPTEKSDD123DETECTORNEW_H
#define CLSAMPTEKSDD123DETECTORNEW_H

#include "beamline/AMXRFDetector.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

class AMDSClientAppController;
class AMDSClientRequest;
class AMDSClientDataRequest;
class AMDSClientRelativeCountPlusCountDataRequest;

class QSignalMapper;

/// Simple class that encapsulates a vector of general purpose counter data from the Amptek and holds the average as well (so computation only needs to be done once)
class CLSAmptekSDD123DetectorGeneralPurposeCounterData
{
public:
	/// Constructor takes the vector and the average value
	CLSAmptekSDD123DetectorGeneralPurposeCounterData(const QVector<int> &generalPurposeCounterVector = QVector<int>(), double averageValue = -1);

	/// Returns the vector of general purpose counter counts
	QVector<int> generalPurposeCounterVector() const { return generalPurposeCounterVector_; }
	/// Returns the averge value of the vector
	double averageValue() const { return averageValue_; }

protected:
	/// The vector of general purpose counter counts
	QVector<int> generalPurposeCounterVector_;
	/// The average value of the vector
	double averageValue_;
};

class CLSAmptekSDD123DetectorNew : public AMXRFDetector
{
Q_OBJECT
public:
	/// Default constructor. Requires the name and base PV of the detector. It builds all the PV's and connects them accordingly.
	CLSAmptekSDD123DetectorNew(const QString &name, const QString &description, const QString &baseName, const QString &amdsBufferName, QObject *parent = 0);
	virtual ~CLSAmptekSDD123DetectorNew();

	/// Returns the buffer name for this detector
	virtual QString amdsBufferName() const;
	/// Configures the server with the given identifier
	void configAMDSServer(const QString &amptekAMDSServerIdentifier);

	/// The Ampteks don't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	/// Cancelling is implemented for the Amptek detectors
	virtual bool canCancel() const { return true; }
	/// Clearing is not currently supported for the Amptek detectors
	virtual bool canClear() const { return false; }

	/// Ampteks are capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return true; }

	/// The ampteks can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return true; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// The Ampteks share a triggering source  sometimes uses the synchronized dwell time object
	virtual bool sharesDetectorTriggerSource() const;
	/// Returns the synchronized dwell time trigger source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }

	/// Returns the type of the last acquisition
	virtual AMDetectorDefinitions::ReadMode readMode() const { return lastReadMode_; }

	/// Implemented to support returning data from the last acquire(AMDetectorDefinitions::ContinuousMode) call
	virtual AMDSClientDataRequest* lastContinuousData(double seconds) const;
	virtual bool setContinuousDataWindow(double continuousDataWindowSeconds);
	virtual int amdsPollingBaseTimeMilliseconds() const;

	/// Creates an action to enable or disable this amptek for in the array.
	AMAction3* createEnableAction3(bool setEnabled);
	bool isEnabled() const;
	/// Returns the temperature of the detector
	double detectorTemperature() const;

	/// Returns the total gain of the detector
	double detectorTotalGain() const;

	/// Returns the MCA Channels
	double mcaChannels() const;

	/// Returns the Pile-up Rejection value
	double pileUpRejection() const;

	/// Returns the thermoelectric cooler value
	double thermoelectricCooler() const;

	/// Returns the fast threshold control value
	double fastThreshold() const;

	/// Returns the slow threshold control value
	double slowThreshold() const;

	/// Returns the peaking time control value
	double peakingTime() const;

	/// Returns the fast channel peaking time control value
	double fastChannelPeakingTime() const;

	/// Return the eV/bin ratio
	double eVPerBin() const;

	/// The ampteks do not support elapsed time.
	virtual bool supportsElapsedTime() const { return true; }

	/// Returns the ROI low index for a given ROI index
	int amptekLowROI(int index);
	/// Returns the ROI high index for a given ROI index
	int amptekHighROI(int index);

	/// Returns the ROI low ev value for a given ROI index
	double amptekLowROIEv(int index);

	/// Returns the ROI high rv value for a given ROI index
	double amptekHighROIEv(int index);

	/// Returns the AMControl for fastCounts if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* fastCountsControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for averageFastCounts if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* averageFastCountsControl(const QObject *privelegedCaller) const;

	/// Returns the AMControl for slowCounts if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* slowCountsControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for averageSlowCounts if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* averageSlowCountsControl(const QObject *privelegedCaller) const;

	/// Returns the AMControl for accumulationTime if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* accumulationTimeControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for averageAccumulationTime if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* averageAccumulationTimeControl(const QObject *privelegedCaller) const;

	/// Returns the AMControl for liveTime if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* liveTimeControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for averageLiveTime if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* averageLiveTimeControl(const QObject *privelegedCaller) const;

	/// Returns the AMControl for realTime if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* realTimeControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for averageRealTime if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* averageRealTimeControl(const QObject *privelegedCaller) const;

	/// Returns the AMControl for temperature if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* detectorTemperatureControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for high voltage if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* detectorHightVoltageControl(const QObject *privelegedCaller) const;

	/// Returns the AMControl for total gain if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* totalGainControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for MCA channel count if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* mcaChannelsControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for pile up rejection if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* pileUpRejectionControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for cooler if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* thermoelectricCoolerControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for fast threshold if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* fastThresholdControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for slow threshold if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* slowThresholdControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for peaking time if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* peakingTimeControl(const QObject *privelegedCaller) const;
	/// Returns the AMControl for fast channel peaking time if the QObject type passed casts to CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	AMControl* fastChannelPeakingTimeControl(const QObject *privelegedCaller) const;

	/// Returns the general purpose counter data from the AMDSClientDataRequest
	CLSAmptekSDD123DetectorGeneralPurposeCounterData retrieveGeneralPurposeCounterData(AMDSClientDataRequest *amptekDataRequest);

	/// Implemented to return a mapping from baseData to the applicable range data. Expects a single vector in the list.
	virtual AMDetectorContinuousMotionRangeData retrieveContinuousMotionRangeData(const QList<QVector<qint32> > &baseData, int expectedDuration = -1, int threshold = -1);

public slots:

	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Amptek detectors do not support clearing
	virtual bool clear() { return false; }

	/// Set the internal amptek ROI to a given low and high channel number
	void setAmptekROI(int index, int lowChannel, int highChannel);

	/// Set the internal amptek ROI to a given low and high eV range
	void setAmptekROIbyEv(int index, double lowEv, double highEv);

	/// Sets the eV/bin ratio
	void setEVPerBin(double eVPerBin);

signals:
	/// Emitted when the control for a low index ROI has changed
	void lowIndexValueChanged(int index);
	/// Emitted when the control for a high index ROI has changed
	void highIndexValueChanged(int index);

	void detectorTemperatureChanged(double newTemperature);

	/// Emitted when the eV/bin ratio changes
	void eVPerBinChanged(double eVPerBin);

protected slots:
	/// Handles changes from the low index controls
	void onLowIndexValueChanged(int index);
	/// Handles changes from the low index controls
	void onHighIndexValueChanged(int index);


	/// ============= SLOTs to handle AMDSClientAppController signals =========
	/// slot to handle the signal of request data ready
	void onRequestDataReady(AMDSClientRequest* clientRequest);
	/// slot to handle the signal of socketEror
	void onServerError(int errorCode, bool removeServer, const QString &serverIdentifier, const QString &errorMessage);

	void onLastContinuousDataRequestDestroyed();

protected:
	/// Basic initialization implementation for an XRF detector.  Subclass for more specific behaviour.
	virtual bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);

	/// Returns true if the QObject type casts to the priveleged type of CLSAmptekDetailedDetectorView or CLSAmptekDetectorConfigurationView
	bool isPrivelegedType(const QObject *privelegedCaller) const;

	/// Helper function to convert bin value to eV value using current evPerBin value
	double convertBinToEv(int binValue);

	/// Helper function to convert eV value to bin value using current evPerBin value
	int convertEvToBin(double eVValue);

protected:
	/// Control for the fast counts
	AMReadOnlyPVControl *fastCountsControl_;
	/// Control for the average fast counts
	AMReadOnlyPVControl *averageFastCountsControl_;
	/// Control for the slow counts
	AMReadOnlyPVControl *slowCountsControl_;

	/// A list of all of the low index ROI controls
	QList<AMSinglePVControl*> roiLowIndices_;
	/// A list of all of the high index ROI controls
	QList<AMSinglePVControl*> roiHighIndices_;
	/// Signal mapper for the low index controls
	QSignalMapper *lowIndicesSignalMapper_;
	/// Signal mapper for the high index controls
	QSignalMapper *highIndicesSignalMapper_;

	/// Control for the average slow counts
	AMReadOnlyPVControl *averageSlowCountsControl_;
	/// Control for the accumulation time
	AMReadOnlyPVControl *accumulationTimeControl_;
	/// Control for the average accumulation time
	AMReadOnlyPVControl *averageAccumulationTimeControl_;
	/// Control for the live time
	AMReadOnlyPVControl *liveTimeControl_;
	/// Control for the average live time
	AMReadOnlyPVControl *averageLiveTimeControl_;
	/// Control for the real time
	AMReadOnlyPVControl *realTimeControl_;
	/// Control for the average real time
	AMReadOnlyPVControl *averageRealTimeControl_;

	/// The detector temperature control
	AMReadOnlyPVControl *detectorTemperatureControl_;
	/// The detector high voltage control
	AMReadOnlyPVControl *detectorHightVoltageControl_;

	/// The total gain for the detector
	AMSinglePVControl *totalGainControl_;
	/// The number of channels control
	AMReadOnlyPVControl *mcaChannelsControl_;
	/// The pile up rejection is either on or off
	AMSinglePVControl *pileUpRejectionControl_;
	/// The temperature for the cooler
	AMSinglePVControl *thermoelectricCoolerControl_;
	/// The fast threshold for the detector
	AMSinglePVControl *fastThresholdControl_;
	/// The slow threshold for the detector
	AMSinglePVControl *slowThresholdControl_;
	/// The peaking time for the detector
	AMSinglePVControl *peakingTimeControl_;
	/// The fast channel peaking time for the detector
	AMSinglePVControl *fastChannelPeakingTimeControl_;

	/// The enable/disable state for this amptek in the array
	AMPVControl *isRequestedControl_;

	/// PV basename for the detector instance
	QString baseName_;

	/// The eV/bin ratio for this detector
	double eVPerBin_;


	/// the AMDS Amptek Server identifier
	QString amptekAMDSServerIdentifier_;
	/// The AMDS buffer name for this instance
	QString amdsBufferName_;

	/// The data returned from the last acquire(AMDetectorDefinitions::ContinuousMode) call
	AMDSClientRelativeCountPlusCountDataRequest *lastContinuousDataRequest_;
	/// The read mode type of the last acquire call
	AMDetectorDefinitions::ReadMode lastReadMode_;

	double continuousDataWindowSeconds_;
	int pollingRateMilliSeconds_;
};

#endif // CLSAMPTEKSDD123DETECTORNEW_H
