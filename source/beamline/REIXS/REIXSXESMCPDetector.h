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


#ifndef REIXSXESMCPDETECTOR_H
#define REIXSXESMCPDETECTOR_H

#include "beamline/AMDetector.h"
#include "dataman/datasource/REIXS/REIXSXESMCPDataSource.h"
#include "util/AMTimer.h"

class AMControl;
class AMReadOnlyPVControl;
class AMSinglePVControl;
class AMControlSet;
class AMDetectorTriggerSource;
class AMDetectorDwellTimeSource;

class REIXSXESMCPDetector : public AMDetector
{
Q_OBJECT
public:
	enum XESMCPFinishedCondition{
		FinishedInvalid = 0x0,
		FinishedTotalCounts = 0x1,
		FinishedTotalTime = 0x2
	};
	Q_DECLARE_FLAGS(XESMCPFinishedConditions, XESMCPFinishedCondition)

	/// Default constructor. It builds all of the PVs and connec to them accordingly.
	virtual ~REIXSXESMCPDetector();
	REIXSXESMCPDetector(QObject *parent = 0);

	/// Returns the number of dimensions in the output of this detector. This is a image detector, so it has a rank of 2.
	virtual int rank() const { return 2; }
	/// Returns the size (ie: number of elements) along each dimension of the detector. Returned from the image dataSource.
	virtual AMnDIndex size() const { return image_->size(); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const { return image_->size(axisNumber); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo> axes() const { return image_->axes(); }

	/// The XES MCP explicitly require powering on with high voltage HOWEVER we're going to ignore that for the time being
	virtual bool requiresPower() const { return false; }

	/// Cancelling is not implemented for the XES MCP detector
	virtual bool canCancel() const { return true; }
	/// Clearing is supported for the XES MCP detector
	virtual bool canClear() const { return true; }

	/// XES MCP is not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// Returns the current acquisition dwell time from the detector
	virtual double acquisitionTime() const;

	/// The XES MCP cannot be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns a null string as this is not supported
	virtual QString synchronizedDwellKey() const { return QString(); }

	/// Returns the detector's trigger source
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the detector's dwell time source
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Returns the total count (all counts in image) as the single reading
	virtual AMNumber singleReading() const;

	/// Fills a (hopefully) valid pointer to a block of detector data in row-major order (first axis varies slowest)
	virtual bool data(double *outputValues) const;

	/// Returns a REIXSXESMCPDataSource suitable for viewing
	virtual AMDataSource* dataSource() const { return image_; }
	/// Access a data source corresponding to the detector's accumulated image
	AMDataSource* image() const { return image_; }
	/// Access a data source corresponding to the detector's real-time (persist-delay) image
	AMDataSource* instantaneousImage() const { return instantaneousImage_; }

	QVector<int> imageData() const;
	QVector<int> instantaneousImageData() const;

	/// Read the total counts in the image:
	double totalCounts() const;
	/// Access the counts per second at this instant in time
	double countsPerSecond() const;

	REIXSXESMCPDetector::XESMCPFinishedConditions finishedConditions() const;
	bool clearOnStart() const;
	int totalCountTarget() const;

	/// Sets the read mode for this detector (if possible, check with canContinuousAcquire)
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);
	/// Adds time to the  acquisition dwell time
	virtual bool addAcquisitionTime(double seconds);
	/// Returns the tolerance for the acquisition time.
	virtual double acquisitionTimeTolerance() const;

	AMControl* averagingPeriodControl();
	AMControl* persistDurationControl();

public slots:
	void setFinishedConditionTotalTime();
	void setFinishedConditionTotalCounts();
	void setFinishedConditionTotalTimeOrTotalCounts();
	void setClearOnStart(bool clearOnStart);
	void setTotalCountTarget(int totalCountTarget);

	/// slot to pause the dwelling
	void pauseDwelling();
	/// slot to resume the dwelling
	void resumeDwelling();

signals:
	/// Emitted whenever the countsPerSecond changes
	void countsPerSecondChanged(double);
	/// Emitted whenever the total number of counts changes
	void totalCountsChanged(double);
	/// Emitted when the image and imageData() changes (ie: has new values)
	void imageDataChanged();
	/// Emitted when the instantaneousImage() and instantaneousImageData() changes (ie: has new values)
	void instantaneousImageDataChanged();

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out
	void onControlsTimedOut();

	/// Handles requests for triggering from the AMDetectorTriggerSource
	void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);
	void onDwellTimeSourceSetDwellTime(double dwellSeconds);

	void onDwellTimeTimerTimeout();
	void onTotalCountsControlValueChanged(double totalCounts);

protected:
	bool initializeImplementation();
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	bool cancelAcquisitionImplementation();
	bool cleanupImplementation();

	bool clearImplementation();


	void setFinishedCondition(REIXSXESMCPDetector::XESMCPFinishedConditions finishedConditions);

	void acquisitionSucceededHelper();
	void acquisitionCancelledHelper();

	/// enable/disable MCP updating the count/total count
	void toggleMCPCountUpdate(bool on);

protected:
	/// The master set of controls
	AMControlSet *allControls_;

	/// Holds the dataSource for the detector
	REIXSXESMCPDataSource* image_, *instantaneousImage_;

	AMReadOnlyPVControl* totalCountsControl_;
	AMReadOnlyPVControl* countsPerSecondControl_;
	AMReadOnlyPVControl* imageControl_;
	AMReadOnlyPVControl* instantaneousImageControl_;
	AMReadOnlyPVControl* resolutionXControl_;
	AMReadOnlyPVControl* resolutionYControl_;

	AMSinglePVControl* clearControl_;
	AMSinglePVControl* averagingPeriodSecsControl_;
	AMSinglePVControl* persistTimeSecsControl_;

	/// control to turn on/off veto
	AMControl* vetoControl_;
	/// control to check veto state
	AMControl* vetoStateControl_;

	QString basePVName_;


	AMDetectorTriggerSource *triggerSource_;
	AMDetectorDwellTimeSource *dwellTimeSource_;

	REIXSXESMCPDetector::XESMCPFinishedConditions finishedConditions_;
	bool clearOnStart_;

	double dwellTime_;
	AMTimer *dwellTimeTimer_;
	int totalCountTarget_;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(REIXSXESMCPDetector::XESMCPFinishedConditions)

#endif // REIXSXESMCPDETECTOR_H
