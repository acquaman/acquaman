/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef REIXSXESSCANCONFIGURATION_H
#define REIXSXESSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "dataman/REIXS/REIXSXESMCPDetectorInfo.h"

/// This class specifies the parameters that completely describe how to run an XES scan on the REIXS beamline/spectrometer
/*! \todo Move some of this information up to a general AMXESScanConfiguration class. What would be useful for others? */
class REIXSXESScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int gratingNumber READ gratingNumber WRITE setGratingNumber)
	Q_PROPERTY(double centerEV READ centerEV WRITE setCenterEV)
	Q_PROPERTY(double maximumTotalCounts READ maximumTotalCounts WRITE setMaximumTotalCounts)
	Q_PROPERTY(double maximumDurationSeconds READ maximumDurationSeconds WRITE setMaximumDurationSeconds)
	Q_PROPERTY(double defocusDistanceMm READ defocusDistanceMm WRITE setDefocusDistanceMm)
	Q_PROPERTY(int spectrometerCalibrationId READ spectrometerCalibrationId WRITE setSpectrometerCalibrationId)
	Q_PROPERTY(bool detectorOrientation READ detectorOrientation WRITE setDetectorOrientation)
	Q_PROPERTY(double detectorTiltOffset READ detectorTiltOffset WRITE setDetectorTiltOffset)
	Q_PROPERTY(bool shouldStartFromCurrentPosition READ shouldStartFromCurrentPosition WRITE setShouldStartFromCurrentPosition)
	Q_PROPERTY(bool doNotClearExistingCounts READ doNotClearExistingCounts WRITE setDoNotClearExistingCounts)
	Q_PROPERTY(AMDbObject* mcpDetectorInfo READ dbGetMcpDetectorInfo WRITE dbLoadMcpDetectorInfo)

public:
	/// Default Constructor
	Q_INVOKABLE explicit REIXSXESScanConfiguration(QObject *parent = 0);

	virtual QString description() const { return QString("XES Scan: %1 eV").arg(centerEV()); }

	/// The number of the grating to use for this scan
	int gratingNumber() const { return gratingNumber_; }
	/// Where we should place the detector (ie: the energy at the center of the detector)
	double centerEV() const { return centerEV_; }
	/// How long to count for? We should stop this scan when we get this many counts. (Using double so that we can go higher than maximum int value)
	double maximumTotalCounts() const { return maximumTotalCounts_; }
	/// How long to count for? We should stop this scan after this many seconds have elapsed
	int maximumDurationSeconds() const { return maximumDurationSeconds_; }
	/// Any lateral offset we should introduce along the angle at this energy, to slide the detector into or out of the focus position (Useful for calibration and testing)
	double defocusDistanceMm() const { return defocusDistanceMm_; }
	/// The database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	int spectrometerCalibrationId() const { return spectrometerCalibrationId_; }
	/// The orientation of the detector: 0 for horizontal (wide window, low resolution), 1 for vertical (narrow window, high resolution)
	bool detectorOrientation() const { return detectorOrientation_; }
	/// Normally, the detector should be tangent to the rowland circle for best focussing.  This is an offset tilt, in degrees, where positive means more normal; negative means more grazing.
	double detectorTiltOffset() const { return detectorTiltOffset_; }
	/// A flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move it)
	bool shouldStartFromCurrentPosition() const { return shouldStartFromCurrentPosition_; }
	/// A flag indicating that we should start the scan without clearing the existing counts on the detector.
	bool doNotClearExistingCounts() const { return doNotClearExistingCounts_; }
	/// Configuration information for the MCP detector itself
	const REIXSXESMCPDetectorInfo* mcpDetectorInfo() const { return &mcpDetectorInfo_; }
	REIXSXESMCPDetectorInfo* mcpDetectorInfo() { return &mcpDetectorInfo_; }



	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this scan configuration.
	/*! \todo This could be const, but then AMScanConfiguration() constructor would need to accept a const AMScanConfiguration.*/
	virtual AMScanController* createController();

	// Support for database loading and storing operations
	////////////////
	/// used to access the MCP detector information for storing to the database
	AMDbObject* dbGetMcpDetectorInfo() { return &mcpDetectorInfo_; }
	/// used to load the MCP detector information from the database
	void dbLoadMcpDetectorInfo(AMDbObject* newObject) { REIXSXESMCPDetectorInfo* d; if((d = qobject_cast<REIXSXESMCPDetectorInfo*>(newObject))) mcpDetectorInfo_ = *d; setModified(true); }


signals:

public slots:

	/// Set the number of the grating to use for this scan
	void setGratingNumber(int gratingNumber) { gratingNumber_ = gratingNumber; setModified(true); }
	/// Where we should place the detector (ie: the energy at the center of the detector)
	void setCenterEV(double centerEV) { centerEV_ = centerEV; setModified(true); }
	/// Set how long to count for. We should stop this scan when we get this many counts. (Using double so that we can go higher than maximum int value)
	void setMaximumTotalCounts(double counts) { maximumTotalCounts_ = counts; setModified(true); }
	/// Set how long to count for. We should stop this scan after this many seconds have elapsed
	void setMaximumDurationSeconds(int seconds) { maximumDurationSeconds_ = seconds; setModified(true); }
	/// Set any lateral offset we should introduce along the angle at this energy, to slide the detector into or out of the focus position (Useful for calibration and testing)
	void setDefocusDistanceMm(double defocusDistanceMm) { defocusDistanceMm_ = defocusDistanceMm; setModified(true); }
	/// Set the database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	void setSpectrometerCalibrationId(int id) { spectrometerCalibrationId_ = id; setModified(true); }
	/// Set the orientation of the detector: 0 for horizontal (wide window, low resolution), 1 for vertical (narrow window, high resolution)
	void setDetectorOrientation(bool orientationIsVertical) { detectorOrientation_ = orientationIsVertical; setModified(true); }
	/// Set the detector incidence angle offset (tilt), in degrees, up from tangent to the rowland circle. Normally, the detector should be tangent to the rowland circle for best focussing.  This is an offset tilt, in degrees, where positive means more normal; negative means more grazing.
	void setDetectorTiltOffset(double detectorTilt) { detectorTiltOffset_ = detectorTilt; setModified(true); }
	/// Set a flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move things before starting the scan)
	void setShouldStartFromCurrentPosition(bool startInCurrentPosition) { shouldStartFromCurrentPosition_ = startInCurrentPosition; setModified(true); }
	/// Set a flag indicating that we should start the scan without clearing the existing counts on the detector.
	void setDoNotClearExistingCounts(bool doNotClear) { doNotClearExistingCounts_ = doNotClear; setModified(true); }

protected:

	/// The number of the grating to use for this scan
	int gratingNumber_;
	/// Where we should place the detector (ie: the energy at the center of the detector)
	double centerEV_;
	/// We should stop this scan when we get this many counts
	double maximumTotalCounts_;
	/// We should stop this scan after this many seconds have elapsed
	int maximumDurationSeconds_;
	/// Any lateral offset we should introduce along the angle at this energy, to slide the detector into or out of the focus position (Useful for calibration and testing)
	double defocusDistanceMm_;
	/// The orientation of the detector: 0 for horizontal (wide window, low resolution), 1 for vertical (narrow window, high resolution)
	bool detectorOrientation_;
	/// Normally, the detector should be tangent to the rowland circle for best focussing.  This is an offset tilt, in degrees, where positive means more normal; negative means more grazing, and 0 will place the detector tangent to the rowland circle.
	double detectorTiltOffset_;
	/// The database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	int spectrometerCalibrationId_;
	/// A flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move it)
	bool shouldStartFromCurrentPosition_;
	/// A flag indicating that we should start the scan without clearing the existing counts on the detector.
	bool doNotClearExistingCounts_;

	/// Detector configuration information:
	REIXSXESMCPDetectorInfo mcpDetectorInfo_;



};

#endif // REIXSXESSCANCONFIGURATION_H
