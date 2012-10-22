/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSSPATIALLINESCANCONFIGURATION_H
#define VESPERSSPATIALLINESCANCONFIGURATION_H

#include "acquaman/AMRegionScanConfiguration.h"
#include "dataman/info/AMROIInfo.h"
#include "application/VESPERS/VESPERS.h"

/// This class is the VESPERS specific spatial line scan configuration.
/*!
	This class extends the AMRegionsScanConfiguration to configure a scan
	using only one motor to move the sample translationally.  Currently,
	there are four choices of motor to use:

		- horizontally using the H pseudo motor
		- horizontally using the x real motor
		- vertically using the V pseudo motor
		- vertically using the z real motor.

	It then operates in exactly the same way as a 2D map, define a start
	point and end point with an appropriate delta.  It generates a pixmap
	with the number of pixels = 1.  All of the rest of the parameters set
	are the same as a 2D map.  It also has the ability to take CCD images
	at the same time (identical to the 2D map).
  */

class VESPERSSpatialLineScanConfiguration : public AMRegionScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(double start READ start WRITE setStart)
	Q_PROPERTY(double end READ end WRITE setEnd)
	Q_PROPERTY(double step READ step WRITE setStep)
	Q_PROPERTY(double time READ time WRITE setTime)
	Q_PROPERTY(int incomingChoice READ incomingChoice WRITE setIncomingChoice)
	Q_PROPERTY(int fluorescenceDetectorChoice READ fluorescenceDetectorChoice WRITE setFluorescenceDetectorChoice)
	Q_PROPERTY(int motorChoice READ motorChoice WRITE setMotorChoice)
	Q_PROPERTY(bool usingCCD READ usingCCD WRITE setUsingCCD)
	Q_PROPERTY(QString ccdFileName READ ccdFileName WRITE setCCDFileName)
	Q_PROPERTY(AMDbObject* roiInfoList READ dbGetROIInfoList WRITE dbLoadROIInfoList)
	Q_PROPERTY(QString rois READ readRoiList WRITE writeRoiList)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS Spatial Line Scan Configuration")

public:
	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector { None = 0, SingleElement = 1, FourElement = 2 };
	/// Enum for which two motors should be used for scanning the horizontal and vertical directions.  Currently there are only HandV and XandZ, but in the future there will be more (eg: nanoCube, big beam).
	enum MotorChoice { H = 0, X, V, Z };

	/// Constructor.
	Q_INVOKABLE VESPERSSpatialLineScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	VESPERSSpatialLineScanConfiguration(const VESPERSSpatialLineScanConfiguration &original);

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns whether we are going to export the spectra data sources or not.
	bool exportSpectraSources() const { return exportSpectraSources_; }

	/// Returns the current I0 ion chamber choice.
	VESPERS::IonChamber incomingChoice() const { return I0_; }
	/// Returns the current fluorescence detector choice.
	FluorescenceDetector fluorescenceDetectorChoice() const { return fluorescenceDetectorChoice_; }
	/// Returns the current motor choice.
	MotorChoice motorChoice() const { return motorChoice_; }
	/// Returns whether the scan is using the CCD or not.
	bool usingCCD() const { return usingCCD_; }
	/// Returns the CCD file name.
	QString ccdFileName() const { return ccdFileName_; }

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Overloaded.  Returns the current total estimated time but also specifies whether the time should be recomputed first.
	double totalTime(bool recompute) { if (recompute) computeTotalTime(); return totalTime_; }
	/// Returns the time offset.
	double timeOffset() const { return timeOffset_; }

	/// Returns the ROI list.  The list is empty if not using a fluorescence detector.
	AMROIInfoList roiList() const { return roiInfoList_; }

	/// Get a pretty looking string of the current regions of interest.  Used primarily for exporting the list into the header of the file.
	QString readRoiList() const;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void writeRoiList(QString) {}

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	QString headerText() const;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void setHeaderText(QString) {}

	/// Returns the AMControlInfo for the scanned region control.
	AMControlInfo regionControlInfo() const { return regions_->defaultControl()->toInfo(); }
	/// Returns the AMControlInfo for the time control.
	AMControlInfo timeControlInfo() const { return regions_->defaultTimeControl()->toInfo(); }

	// Database loading and storing
	///////////////////////

	/// The database reading member function.
	AMDbObject *dbGetROIInfoList() { return &roiInfoList_; }
	/// Don't need to do anything because dbGetROIList always returns a valid AMDbObject.
	void dbLoadROIInfoList(AMDbObject *) {}

	// Convience getters.
	/// Returns the start position for the region.
	double start() const { return regions_->start(0); }
	/// Returns the end position for the region.
	double end() const { return regions_->end(0); }
	/// Returns the step size for the region.
	double step() const { return regions_->delta(0); }
	/// Returns the time for the region.
	double time() const { return regions_->time(0); }
	/// Returns whether the region is valid.
	bool validAxis() const { return regions_->isValid(0); }

signals:
	/// Notifier that the incoming choice has changed.
	void incomingChoiceChanged(VESPERS::IonChamber);
	/// Same signal.  Just passing as an int.
	void incomingChoiceChanged(int);
	/// Notifier that the fluorescence choice has changed.
	void fluorescenceDetectorChoiceChanged(FluorescenceDetector);
	/// Same signal.  Just passing as an int.
	void fluorescenceDetectorChoiceChanged(int);
	/// Notifier that the motors choice has changed.
	void motorChoiceChanged(MotorChoice);
	/// Same signal.  Just passing as an int.
	void motorChoiceChanged(int);
	/// Notifier that the flag for whether the CCD will be used has changed.
	void usingCCDChanged(bool);
	/// Notifier that the name of the CCD file name has changed.
	void ccdFileNameChanged(QString);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

	/// Notifier that the start position has changed.
	void startChanged(double);
	/// Notifier that the end position has changed.
	void endChanged(double);
	/// Notifier that the step size has changed.
	void stepChanged(double);
	/// Notifier that the time has changed.
	void timeChanged(double);

public slots:
	/// Sets the choice for I0 ion chamber.
	void setIncomingChoice(VESPERS::IonChamber I0);
	/// Overloaded.  Used for database loading.
	void setIncomingChoice(int I0) { setIncomingChoice((VESPERS::IonChamber)I0); }
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetectorChoice(FluorescenceDetector detector);
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetectorChoice(int detector) { setFluorescenceDetectorChoice((FluorescenceDetector)detector); }
	/// Sets the choice for the set of motors used for scanning.
	void setMotorChoice(MotorChoice choice);
	/// Overloaded.  Used for database loading.
	void setMotorChoice(int choice) { setMotorChoice((MotorChoice)choice); }
	/// Sets whether the scan should be using the CCD or not.
	void setUsingCCD(bool use) { usingCCD_ = use; emit usingCCDChanged(use); setModified(true); }
	/// Sets the file name for the CCD files.
	void setCCDFileName(const QString &name) { ccdFileName_ = name; emit ccdFileNameChanged(ccdFileName_); setModified(true); }
	/// Sets the time offset used for estimating the scan time.
	void setTimeOffset(double offset) { timeOffset_ = offset; computeTotalTime(); }

	/// Sets the ROI list.
	void setRoiInfoList(const AMROIInfoList &list) { roiInfoList_ = list; setModified(true); }

	// Convience setters.
	/// Sets the start position.
	void setStart(double position);
	/// Sets the end position.
	void setEnd(double position);
	/// Sets the step size.
	void setStep(double position);
	/// Sets the time.
	void setTime(double position);

	/// Sets whether we export the scan with the spectra included or not.
	void setExportSpectraSources(bool exportSpectra);

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime();

protected:
	/// I0 ion chamber choice.
	VESPERS::IonChamber I0_;
	/// Fluorescence detector choice.
	FluorescenceDetector fluorescenceDetectorChoice_;
	/// Motor choice for which set of motors will be used.
	MotorChoice motorChoice_;
	/// Flag holding whether the scan should use the CCD detector or not.
	bool usingCCD_;
	/// The file name (minus number, path and extension of the file) for the CCD.
	QString ccdFileName_;
	/// The list holding all the current ROIs for the detector.
	AMROIInfoList roiInfoList_;
	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
	/// Holds the offset per point of extra time when doing a scan.
	double timeOffset_;

	/// Flag holding whether we are exporting the spectra data sources or not.
	bool exportSpectraSources_;
};

#endif // VESPERSSPATIALLINESCANCONFIGURATION_H
