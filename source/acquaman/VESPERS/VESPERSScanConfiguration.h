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


#ifndef VESPERSSCANCONFIGURATION_H
#define VESPERSSCANCONFIGURATION_H

#include "acquaman/CLS/CLSScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSScanConfigurationDbObject.h"
#include "application/VESPERS/VESPERS.h"

/*!
  This class is a non-QObject class that holds the database object for a generic scan configuration.  It has getters
  and setters to the database object.  This class exists purely because classes can not multiply inherit two QObject
  classes.
  */
class VESPERSScanConfiguration: public CLSScanConfiguration
{
public:
	/// Constructor.
	VESPERSScanConfiguration();
	/// Copy constructor.
	VESPERSScanConfiguration(const VESPERSScanConfiguration &original);
	/// Destructor.
	virtual ~VESPERSScanConfiguration();

//	// DbObject specific methods.
//	////////////////////////////////////////////

//	/// Returns the database object.  Intended for gaining access to its signals.
//	VESPERSScanConfigurationDbObject *dbObject() const { return dbObject_; }
//	/// The database reading member function.
//	AMDbObject *dbReadScanConfigurationDbObject() { return dbObject_; }
//	/// The database writing member function.
//	void dbWriteScanConfigurationDbObject(AMDbObject *object);

	// Getters
	/////////////////////////////////////////

	/// Returns the current I0 ion chamber choice.
	VESPERS::IonChamber incomingChoice() const { return dbObject_->incomingChoice(); }
	/// Returns the current It ion chamber choice.
	VESPERS::IonChamber transmissionChoice() const { return dbObject_->transmissionChoice(); }
	/// Returns the current fluorescence detector choice.
	VESPERS::FluorescenceDetectors fluorescenceDetector() const { return dbObject_->fluorescenceDetector(); }
	/// Returns the current motor choice.
	VESPERS::Motors motor() const { return dbObject_->motor(); }
	/// Returns the CCD detector choice.
	VESPERS::CCDDetectors ccdDetector() const { return dbObject_->ccdDetector(); }
	/// Returns the CCD file name.
	QString ccdFileName() const { return dbObject_->ccdFileName(); }
	/// Returns the normal position.
	double normalPosition() const { return dbObject_->normalPosition(); }
//	/// Returns the list of regions of interest.
//	QList<AMRegionOfInterest *> regionsOfInterest() const { return dbObject_->regionsOfInterest(); }

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Overloaded.  Returns the current total estimated time but also specifies whether the time should be recomputed first.
	double totalTime(bool recompute) { if (recompute) computeTotalTimeImplementation(); return totalTime_; }
	/// Returns the time offset.
	double timeOffset() const { return timeOffset_; }
	/// Returns the close fast shutter flag.
	bool closeFastShutter() const { return closeFastShutter_; }
	/// Returns whether the motor should go back to where it started after a scan.
	bool returnToOriginalPosition() const { return returnToOriginalPosition_; }
	/// Returns whether the scaler will be cleaned up after the end of a scan.
	bool cleanupScaler() const { return cleanupScaler_; }

	// Setters
	////////////////////////////////////////

	/// Sets the choice for I0 ion chamber.
	void setIncomingChoice(VESPERS::IonChamber I0) { dbObject_->setIncomingChoice(I0); }
	/// Overloaded.  Used for database loading.
	void setIncomingChoice(int I0) { setIncomingChoice((VESPERS::IonChamber)I0); }
	/// Sets the choice for It ion chamber.
	void setTransmissionChoice(VESPERS::IonChamber It) { dbObject_->setTransmissionChoice(It); }
	/// Overloaded.  Used for database loading.
	void setTransmissionChoice(int It) { setTransmissionChoice((VESPERS::IonChamber)It); }
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(VESPERS::FluorescenceDetectors detector) { dbObject_->setFluorescenceDetector(detector); }
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetector(int detector) { setFluorescenceDetector((VESPERS::FluorescenceDetectors)detector); }
	/// Sets the choice for the set of motors used for scanning.
	void setMotor(VESPERS::Motors choice) { dbObject_->setMotor(choice); }
	/// Overloaded.  Used for database loading.
	void setMotor(int choice) { setMotor((VESPERS::Motors)choice); }
	/// Sets whether the scan should be using the CCD or not.
	void setCCDDetector(VESPERS::CCDDetectors ccd) { dbObject_->setCCDDetector(ccd); }
	/// Overloaded.  Used for database loading.
	void setCCDDetector(int ccd) { setCCDDetector((VESPERS::CCDDetectors)ccd); }
	/// Sets the file name for the CCD files.
	void setCCDFileName(const QString &name) { dbObject_->setCCDFileName(name); }
	/// Sets the normal position.
	void setNormalPosition(double newPosition) { dbObject_->setNormalPosition(newPosition); }
//	/// Adds a region of interest to the list.
//	void addRegionOfInterest(AMRegionOfInterest *region) { dbObject_->addRegionOfInterest(region); }
//	/// Removes a region of interest from the list.
//	void removeRegionOfInterest(AMRegionOfInterest *region) { dbObject_->removeRegionOfInterest(region); }
//	/// Sets the bounding range for the given region of interest.
//	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region) { dbObject_->setRegionOfInterestBoundingRange(region); }

	/// Sets the time offset used for estimating the scan time.
	void setTimeOffset(double offset) { timeOffset_ = offset; computeTotalTimeImplementation(); }
	/// Sets the flag for whether the fast shutter should be put in after a scan.
	void setCloseFastShutter(bool close) { closeFastShutter_ = close; }
	/// Sets the flag for whether the sample should return to its original position.
	void setReturnToOriginalPosition(bool returnToOriginalPosition) { returnToOriginalPosition_ = returnToOriginalPosition; }
	/// Sets the cleanup scaler flag.
	void setCleanupScaler(bool cleanup) { cleanupScaler_ = cleanup; }

protected:
	/// Computes the total time any time the regions list changes.  It is expected that subclasses will have a signal totalTimeChanged(double) that is emitted by this method.
	virtual void computeTotalTimeImplementation() = 0;

	// Header method and helper methods.
	////////////////////////////////////////

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	virtual QString headerText() const = 0;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void setHeaderText(QString) {}
	/// Returns a string for the header string for the given \param fluorescence detector.
	QString fluorescenceHeaderString(VESPERS::FluorescenceDetectors detector) const;
	/// Returns a string for the I0 ion chamber.
	QString incomingChoiceHeaderString(VESPERS::IonChamber detector) const;
	/// Returns a string for the It ion chamber.
	QString transmissionChoiceHeaderString(VESPERS::IonChamber detector) const;
	/// Returns a string that with what motors were used based on \param motor.
	QString motorHeaderString(VESPERS::Motors motor) const;
	/// Returns a string that gives the name of the CCD images.
	QString ccdDetectorHeaderString(VESPERS::CCDDetectors detector) const;
//	/// Returns a string that displays all the regions of interest.
//	QString regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const;

	////////////////////////////////////////

	/// The database object we're encapsulating.
	VESPERSScanConfigurationDbObject *dbObject_;

	/// Holds whether the fast shutter needs to be put in at the end of a scan.
	bool closeFastShutter_;
	/// Holds whether a scan should move to the position before the scan after the scan finishes.
	bool returnToOriginalPosition_;
	/// Holds whether the cleanup actions should put the scaler back in continuous mode with 1 second dwell time.
	bool cleanupScaler_;
	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
	/// Holds the offset per point of extra time when doing a scan.
	double timeOffset_;
};

#endif // VESPERSSCANCONFIGURATION_H
