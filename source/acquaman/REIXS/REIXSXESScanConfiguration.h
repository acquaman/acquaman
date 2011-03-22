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
	Q_PROPERTY(double detectorTilt READ detectorTilt WRITE setDetectorTilt)
	Q_PROPERTY(bool shouldStartFromCurrentPosition READ shouldStartFromCurrentPosition WRITE setShouldStartFromCurrentPosition)
	Q_PROPERTY(AMDbObject* mcpDetectorInfo READ dbGetMcpDetectorInfo WRITE dbLoadMcpDetectorInfo)
	// Not here for now: Q_PROPERTY(AMDbObject* spectrometerPosition READ spectrometerPosition WRITE dbLoadSpectrometerPosition)

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
	/// The detector incidence angle (tilt), in degrees, up from perfectly grazing. (ie: 0 would line up the detector surface with the light direction, and you wouldn't see anything)
	double detectorTilt() const { return detectorTilt_; }
	/// A flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move it)
	bool shouldStartFromCurrentPosition() const { return shouldStartFromCurrentPosition_; }
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
	/// Set the detector incidence angle (tilt), in degrees, up from perfectly grazing (ie: 0 would line up the detector surface with the light direction, and you wouldn't see anything)
	void setDetectorTilt(double detectorTilt) { detectorTilt_ = detectorTilt; setModified(true); }
	/// Set a flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move things before starting the scan)
	void setShouldStartFromCurrentPosition(bool startInCurrentPosition) { shouldStartFromCurrentPosition_ = startInCurrentPosition; setModified(true); }


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
	/// The detector incidence angle (tilt), in degrees, up from perfectly grazing (ie: 0 would line up the detector surface with the light direction, and you wouldn't see anything)
	double detectorTilt_;
	/// The database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	int spectrometerCalibrationId_;
	/// A flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move it)
	bool shouldStartFromCurrentPosition_;

	/// Detector configuration information:
	REIXSXESMCPDetectorInfo mcpDetectorInfo_;



};

#endif // REIXSXESSCANCONFIGURATION_H
