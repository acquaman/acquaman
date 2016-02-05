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


#ifndef REIXSXESSCANCONFIGURATION_H
#define REIXSXESSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "dataman/REIXS/REIXSXESMCPDetectorInfo.h"

/// This class specifies the parameters that describe how to run an XES scan on the REIXS beamline/spectrometer
class REIXSXESScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int gratingNumber READ gratingNumber WRITE setGratingNumber)
	Q_PROPERTY(double maximumTotalCounts READ maximumTotalCounts WRITE setMaximumTotalCounts)
	Q_PROPERTY(double maximumDurationSeconds READ maximumDurationSeconds WRITE setMaximumDurationSeconds)
	Q_PROPERTY(double defocusDistanceMm READ defocusDistanceMm WRITE setDefocusDistanceMm)
	Q_PROPERTY(int spectrometerCalibrationId READ spectrometerCalibrationId WRITE setSpectrometerCalibrationId)
	Q_PROPERTY(double detectorTiltOffset READ detectorTiltOffset WRITE setDetectorTiltOffset)
	Q_PROPERTY(bool shouldStartFromCurrentPosition READ shouldStartFromCurrentPosition WRITE setShouldStartFromCurrentPosition)
	Q_PROPERTY(bool doNotClearExistingCounts READ doNotClearExistingCounts WRITE setDoNotClearExistingCounts)
	Q_PROPERTY(AMDbObject* mcpDetectorInfo READ dbGetMcpDetectorInfo WRITE dbLoadMcpDetectorInfo)

	Q_PROPERTY(double slitWidth READ slitWidth WRITE setSlitWidth)
	Q_PROPERTY(bool applySlitWidth READ applySlitWidth WRITE setApplySlitWidth)
	Q_PROPERTY(double energy READ energy WRITE setEnergy)
	Q_PROPERTY(bool applyEnergy READ applyEnergy WRITE setApplyEnergy)

	Q_PROPERTY(int polarization READ polarization WRITE setPolarization)
	Q_PROPERTY(double polarizationAngle READ polarizationAngle WRITE setPolarizationAngle)
	Q_PROPERTY(bool applyPolarization READ applyPolarization WRITE setApplyPolarization)


	// The following properties should be moved upwards if we can figure out how to work on this together, and determine what should be stored at the AMScanConfiguration level.
	Q_PROPERTY(QString userScanName READ userScanName WRITE setUserScanName)
	Q_PROPERTY(int scanNumber READ scanNumber WRITE setScanNumber)
	Q_PROPERTY(int sampleId READ sampleId WRITE setSampleId)
	Q_PROPERTY(bool namedAutomatically READ namedAutomatically WRITE setNamedAutomatically)

public:
	/// Default Constructor
	Q_INVOKABLE explicit REIXSXESScanConfiguration(QObject *parent = 0);
	REIXSXESScanConfiguration(const REIXSXESScanConfiguration &original);
	virtual ~REIXSXESScanConfiguration();

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const { return "XES Scan"; }

	/// A human-readable description of this scan configuration. Used by scan action to set the title for the action view.
	virtual QString description() const;

	/// A human-readable synopsis of this scan configuration.  Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const { return "XRay Emission Scan"; }


	/// The number of the grating to use for this scan
	int gratingNumber() const { return gratingNumber_; }

	/// Energy to set beamline to before scan
	double energy() const { return energy_; }
	/// A flag indicating that the energy should be changed before starting the scan
	bool applyEnergy() const { return applyEnergy_; }

	/// Slit Width to set before scan
	double slitWidth() const { return slitWidth_; }
	/// A flag indicating that the slit width should be changed before starting the scan
	bool applySlitWidth() const { return applySlitWidth_; }

	int polarization() const { return polarization_; }
	double polarizationAngle() const { return polarizationAngle_; }
	bool applyPolarization() const { return applyPolarization_; }

	/// How long to count for? We should stop this scan when we get this many counts. (Using double so that we can go higher than maximum int value)
	double maximumTotalCounts() const { return maximumTotalCounts_; }
	/// How long to count for? We should stop this scan after this many seconds have elapsed
	double maximumDurationSeconds() const { return double(maximumDurationSeconds_); }
	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Overloaded.  Returns the current total estimated time but also specifies whether the time should be recomputed first.
	double totalTime(bool recompute) { if (recompute) computeTotalTimeImplementation(); return totalTime_; }

	/// Any lateral offset we should introduce along the angle at this energy, to slide the detector into or out of the focus position (Useful for calibration and testing)
	double defocusDistanceMm() const { return defocusDistanceMm_; }
	/// The database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	int spectrometerCalibrationId() const { return spectrometerCalibrationId_; }
	/// Normally, the detector should be tangent to the rowland circle for best focussing.  This is an offset tilt, in degrees, where positive means more normal, negative means more grazing.
	double detectorTiltOffset() const { return detectorTiltOffset_; }
	/// A flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move it)
	bool shouldStartFromCurrentPosition() const { return shouldStartFromCurrentPosition_; }
	/// A flag indicating that we should start the scan without clearing the existing counts on the detector.
	bool doNotClearExistingCounts() const { return doNotClearExistingCounts_; }

	/// Configuration information for the MCP detector itself
	const REIXSXESMCPDetectorInfo* mcpDetectorInfo() const { return &mcpDetectorInfo_; }
	REIXSXESMCPDetectorInfo* mcpDetectorInfo() { return &mcpDetectorInfo_; }

	/// Computes the total time as count rate changes.
	void computeTotalTime() { computeTotalTimeImplementation(); }
	/// Meta-data information to pre-set in the scan.
	int scanNumber() const { return scanNumber_; }
	/// Meta-data information to pre-set in the scan.
	int sampleId() const { return sampleId_; }
	/// True if we should generate the scan name, number, and sampleId automatically (as best we can, we'll do this based on the last sample move)
	bool namedAutomatically() const { return namedAutomatically_; }

	/// Returns whether or not this scan configuration can convert a property from an enum into a useful string. For use with the export systems. SGMXASScanConfiguration can convert grating and harmonic.
	virtual bool canEnumConvert(const QString &enumName) const;

	/// Returns the useful string version for enum convertable properites. Will return [??] if the property cannot be converted by this scan configuration. SGMXASScanConfiguration can convert grating and harmonic.
	virtual QString enumConvert(const QString &enumName, int enumValue) const;

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this scan configuration.
	/*! \todo This could be const, but then AMScanConfiguration() constructor would need to accept a const AMScanConfiguration.*/
	virtual AMScanController* createController();
	/// Creates a new view (AMScanConfigurationView) showing this configuration. Ownership of the view is the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	// Support for database loading and storing operations
	////////////////
	/// used to access the MCP detector information for storing to the database
	AMDbObject* dbGetMcpDetectorInfo() { return &mcpDetectorInfo_; }
	/// used to load the MCP detector information from the database
	void dbLoadMcpDetectorInfo(AMDbObject* newObject) { REIXSXESMCPDetectorInfo* d; if((d = qobject_cast<REIXSXESMCPDetectorInfo*>(newObject))) mcpDetectorInfo_ = *d; setModified(true); }





signals:
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);


public slots:

	/// Set the number of the grating to use for this scan
	void setGratingNumber(int gratingNumber) { if(gratingNumber_ == gratingNumber) return; gratingNumber_ = gratingNumber;  setModified(true); emit configurationChanged(); }

	/// Set the desired beamline energy value
	void setEnergy(double energy) {if(energy_ == energy) return; energy_ = energy; setModified(true); emit configurationChanged(); }
	void setApplyEnergy(bool ApplyEnergy) { if(applyEnergy_ == ApplyEnergy) return; applyEnergy_ = ApplyEnergy; setModified(true); emit configurationChanged(); }

	/// Set the desired slit width value
	void setSlitWidth(double slitWidth) {if(slitWidth_ == slitWidth) return; slitWidth_ = slitWidth; setModified(true); emit configurationChanged(); }
		void setApplySlitWidth(bool applySlitWidth) { if(applySlitWidth_ == applySlitWidth) return; applySlitWidth_ = applySlitWidth; setModified(true); emit configurationChanged(); }

		void setPolarization(int polarization) { if(polarization == polarization_) return; polarization_ = polarization; setModified(true); emit configurationChanged(); }
		void setPolarizationAngle(double polarizationAngle) { if(polarizationAngle == polarizationAngle_) return; polarizationAngle_ = polarizationAngle; setModified(true); emit configurationChanged(); }
		void setApplyPolarization(bool applyPolarization) { if(applyPolarization == applyPolarization_) return; applyPolarization_ = applyPolarization; setModified(true); emit configurationChanged(); }

	/// Set how long to count for. We should stop this scan when we get this many counts. (Using double so that we can go higher than maximum int value)
	void setMaximumTotalCounts(double counts) { if(maximumTotalCounts_ == counts) return; maximumTotalCounts_ = counts; setModified(true); emit configurationChanged(); }
	/// Set how long to count for. We should stop this scan after this many seconds have elapsed
	void setMaximumDurationSeconds(double seconds) { if(maximumDurationSeconds_ == int(seconds)) return; maximumDurationSeconds_ = int(seconds); setModified(true); emit configurationChanged(); }
	/// Set any lateral offset we should introduce along the angle at this energy, to slide the detector into or out of the focus position (Useful for calibration and testing)
	void setDefocusDistanceMm(double defocusDistanceMm) { if(defocusDistanceMm_ == defocusDistanceMm) return; defocusDistanceMm_ = defocusDistanceMm; setModified(true); emit configurationChanged(); }
	/// Set the database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	void setSpectrometerCalibrationId(int id) { if(spectrometerCalibrationId_ == id) return; spectrometerCalibrationId_ = id; setModified(true); emit configurationChanged(); }
	/// Set the detector incidence angle offset (tilt), in degrees, up from tangent to the rowland circle. Normally, the detector should be tangent to the rowland circle for best focussing.  This is an offset tilt, in degrees, where positive means more normal, negative means more grazing.
	void setDetectorTiltOffset(double detectorTilt) { if(detectorTiltOffset_ == detectorTilt) return; detectorTiltOffset_ = detectorTilt; setModified(true); emit configurationChanged(); }
	/// Set a flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move things before starting the scan)
	void setShouldStartFromCurrentPosition(bool startInCurrentPosition) { if(shouldStartFromCurrentPosition_ == startInCurrentPosition) return; shouldStartFromCurrentPosition_ = startInCurrentPosition; setModified(true); emit configurationChanged(); }
	/// Set a flag indicating that we should start the scan without clearing the existing counts on the detector.
	void setDoNotClearExistingCounts(bool doNotClear) { if(doNotClearExistingCounts_ == doNotClear) return; doNotClearExistingCounts_ = doNotClear; setModified(true); emit configurationChanged(); }


	void setScanNumber(int number) { scanNumber_ = number; setModified(true); }
	void setSampleId(int sampleId) { sampleId_ = sampleId; setModified(true); }
	void setNamedAutomatically(bool autoOn) { namedAutomatically_ = autoOn; setModified(true); }


protected:

	/// Computes the total estimated time for the scan.
	virtual void computeTotalTimeImplementation();


	/// The number of the grating to use for this scan
	int gratingNumber_;

	/// The beamline energy to use for this scan
	double energy_;
	/// A flag indicating that the energy should be changed before starting the scan
	bool applyEnergy_;

	/// The slit width for this scan
	double slitWidth_;
	/// A flag indicating that the slit width should be changed before starting the scan
	bool applySlitWidth_;

	int polarization_;
	double polarizationAngle_;
	bool applyPolarization_;


	/// We should stop this scan when we get this many counts
	double maximumTotalCounts_;
	/// We should stop this scan after this many seconds have elapsed
	double maximumDurationSeconds_;
	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;

	/// Any lateral offset we should introduce along the angle at this energy, to slide the detector into or out of the focus position (Useful for calibration and testing)
	double defocusDistanceMm_;
	/// Normally, the detector should be tangent to the rowland circle for best focussing.  This is an offset tilt, in degrees, where positive means more normal, negative means more grazing, and 0 will place the detector tangent to the rowland circle.
	double detectorTiltOffset_;
	/// The database id of the stored spectrometer calibration we should use. (This spectromter calibration is found in the user database, for now)
	int spectrometerCalibrationId_;
	/// A flag indicating that we should start the scan in whatever position the spectrometer is now. (ie: don't compute the desired position and move it)
	bool shouldStartFromCurrentPosition_;
	/// A flag indicating that we should start the scan without clearing the existing counts on the detector.
	bool doNotClearExistingCounts_;

	int scanNumber_;
	int sampleId_;
	bool namedAutomatically_;

	/// Detector configuration information:
	REIXSXESMCPDetectorInfo mcpDetectorInfo_;





};

#endif // REIXSXESSCANCONFIGURATION_H
