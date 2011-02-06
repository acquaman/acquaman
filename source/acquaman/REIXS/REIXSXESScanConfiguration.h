#ifndef REIXSXESSCANCONFIGURATION_H
#define REIXSXESSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"

class REIXSXESScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int gratingNumber READ gratingNumber WRITE setGratingNumber)
	Q_PROPERTY(double centerEV READ centerEV WRITE setCenterEV)
	Q_PROPERTY(double defocusDistanceMm READ defocusDistanceMm WRITE setDefocusDistanceMm)
	Q_PROPERTY(int spectrometerCalibrationId READ spectrometerCalibrationId WRITE setSpectrometerCalibrationId)
	Q_PROPERTY(bool shouldStartFromCurrentPosition READ shouldStartFromCurrentPosition WRITE setShouldStartFromCurrentPosition)
	// Not here for now: Q_PROPERTY(AMDbObject* spectrometerPosition READ spectrometerPosition WRITE dbLoadSpectrometerPosition)

public:
	/// Default Constructor
	Q_INVOKABLE explicit REIXSXESScanConfiguration(QObject *parent = 0);


	/// The number of the grating to use for this scan
	int gratingNumber() const { return gratingNumber_; }
	/// Where we should place the detector (ie: the energy at the center of the detector)
	double centerEV() const { return centerEV_; }
	/// Any lateral offset we should introduce along the angle at this energy, to slide the detector into or out of the focus position (Useful for calibration and testing)
	double defocusDistanceMm() const { return defocusDistanceMm_; }
	/// The database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	int spectrometerCalibrationId() const { return spectrometerCalibrationId_; }
	/// A flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move it)
	bool shouldStartFromCurrentPosition() const { return shouldStartFromCurrentPosition_; }



	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this scan configuration.
	/*! \todo This could be const, but then AMScanConfiguration() constructor would need to accept a const AMScanConfiguration.*/
	virtual AMScanController* createController();


signals:

public slots:

	/// Set the number of the grating to use for this scan
	void setGratingNumber(int gratingNumber) { gratingNumber_ = gratingNumber; setModified(true); }
	/// Where we should place the detector (ie: the energy at the center of the detector)
	void setCenterEV(double centerEV) { centerEV_ = centerEV; setModified(true); }
	/// Any lateral offset we should introduce along the angle at this energy, to slide the detector into or out of the focus position (Useful for calibration and testing)
	void setDefocusDistanceMm(double defocusDistanceMm) { defocusDistanceMm_ = defocusDistanceMm; setModified(true); }
	/// The database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	void setSpectrometerCalibrationId(int id) { spectrometerCalibrationId_ = id; setModified(true); }
	/// A flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move it)
	void setShouldStartFromCurrentPosition(bool startInCurrentPosition) { shouldStartFromCurrentPosition_ = startInCurrentPosition; setModified(true); }


protected:

	/// The number of the grating to use for this scan
	int gratingNumber_;
	/// Where we should place the detector (ie: the energy at the center of the detector)
	double centerEV_;
	/// Any lateral offset we should introduce along the angle at this energy, to slide the detector into or out of the focus position (Useful for calibration and testing)
	double defocusDistanceMm_;
	/// The database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	int spectrometerCalibrationId_;
	/// A flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move it)
	bool shouldStartFromCurrentPosition_;



};

#endif // REIXSXESSCANCONFIGURATION_H
