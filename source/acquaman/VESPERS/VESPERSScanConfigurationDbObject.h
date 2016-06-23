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


#ifndef VESPERSSCANCONFIGURATIONDBOBJECT_H
#define VESPERSSCANCONFIGURATIONDBOBJECT_H

//#include "dataman/database/AMDbObject.h"
#include "acquaman/CLS/CLSScanConfigurationDbObject.h"
#include "application/VESPERS/VESPERS.h"
//#include "dataman/AMRegionOfInterest.h"

/// This class is the common database object for all scan configurations for the VESPERS beamline.
/*!
	This class holds all of the persistent values that all scan configurations will need.  It is meant
	to consolidate all the duplications from all the scan configurations.  The only requirement for
	any class is that they'll need to set it in their Q_PROPERTY so that they properly load the
	configuration.
  */

#define ERR_VESPERS_SCAN_CONFIGURATION_DB_OBJECT_INVALID_COPY_CONSTRUCTOR 2050401

class VESPERSScanConfigurationDbObject : public CLSScanConfigurationDbObject
{
	Q_OBJECT

	Q_PROPERTY(int incomingChoice READ incomingChoice WRITE setIncomingChoice)
	Q_PROPERTY(int transmissionChoice READ transmissionChoice WRITE setTransmissionChoice)
	Q_PROPERTY(int motor READ motor WRITE setMotor)
	Q_PROPERTY(int fluorescenceDetector READ fluorescenceDetector WRITE setFluorescenceDetector)
	Q_PROPERTY(int ccdDetector READ ccdDetector WRITE setCCDDetector)
	Q_PROPERTY(QString ccdFileName READ ccdFileName WRITE setCCDFileName)
	Q_PROPERTY(double normalPosition READ normalPosition WRITE setNormalPosition)
//	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)

	Q_CLASSINFO("normalPosition", "upgradeDefault=888888.88")

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS Scan Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE VESPERSScanConfigurationDbObject(QObject *parent = 0);
	/// Copy Constructor.
	VESPERSScanConfigurationDbObject(const CLSScanConfigurationDbObject &original);
	/// Destructor.
	virtual ~VESPERSScanConfigurationDbObject();

	/// Returns the current I0 ion chamber choice.
	VESPERS::IonChamber incomingChoice() const { return I0_; }
	/// Returns the current It ion chamber choice.
	VESPERS::IonChamber transmissionChoice() const { return It_; }
	/// Returns the current fluorescence detector choice.
	VESPERS::FluorescenceDetectors fluorescenceDetector() const { return fluorescenceDetector_; }
	/// Returns the current motor choice.
	VESPERS::Motors motor() const { return motor_; }
	/// Returns what CCD the scan is using, if any.
	VESPERS::CCDDetectors ccdDetector() const { return ccdDetector_; }
	/// Returns the CCD file name.
	QString ccdFileName() const { return ccdFileName_; }
	/// Returns the normal position.
	double normalPosition() const { return normalPosition_; }
//	/// Returns the list of regions the configuration has a hold of.
//	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }

signals:
	/// Notifier that the incoming choice has changed.
	void incomingChoiceChanged(VESPERS::IonChamber);
	/// Same signal.  Just passing as an int.
	void incomingChoiceChanged(int);
	/// Notifier that the transmition choice has changed.
	void transmissionChoiceChanged(VESPERS::IonChamber);
	/// Same signal.  Just passing as an int.
	void transmissionChoiceChanged(int);
	/// Notifier that the fluorescence choice has changed.
	void fluorescenceDetectorChanged(VESPERS::FluorescenceDetectors);
	/// Same signal.  Just passing as an int.
	void fluorescenceDetectorChanged(int);
	/// Notifier that the motors choice has changed.
	void motorChanged(VESPERS::Motors);
	/// Same signal.  Just passing as an int.
	void motorChanged(int);
	/// Notifier that the flag for whether the CCD will be used has changed.
	void ccdDetectorChanged(VESPERS::CCDDetectors);
	/// Same signal.  Just passing as an int.
	void ccdDetectorChanged(int);
	/// Notifier that the name of the CCD file name has changed.
	void ccdFileNameChanged(QString);
	/// Notifier that the normal position has changed.
	void normalPositionChanged(double);

public slots:
	/// Sets the choice for I0 ion chamber.
	void setIncomingChoice(VESPERS::IonChamber I0);
	/// Overloaded.  Used for database loading.
	void setIncomingChoice(int I0) { setIncomingChoice((VESPERS::IonChamber)I0); }
	/// Sets the choice for It ion chamber.
	void setTransmissionChoice(VESPERS::IonChamber It);
	/// Overloaded.  Used for database loading.
	void setTransmissionChoice(int It) { setTransmissionChoice((VESPERS::IonChamber)It); }
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(VESPERS::FluorescenceDetectors detector);
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetector(int detector) { setFluorescenceDetector((VESPERS::FluorescenceDetectors)detector); }
	/// Sets the choice for the set of motors used for scanning.
	void setMotor(VESPERS::Motors choice);
	/// Overloaded.  Used for database loading.
	void setMotor(int choice) { setMotor((VESPERS::Motors)choice); }
	/// Sets whether the scan should be using the CCD or not.
	void setCCDDetector(VESPERS::CCDDetectors ccd);
	/// Overloaded.  Used for database loading.
	void setCCDDetector(int ccd) { setCCDDetector((VESPERS::CCDDetectors)ccd); }
	/// Sets the file name for the CCD files.
	void setCCDFileName(const QString &name);
	/// Sets the normal position.
	void setNormalPosition(double newPosition);
//	/// Adds a region of interest to the list.
//	void addRegionOfInterest(AMRegionOfInterest *region);
//	/// Removes a region of interest from the list.
//	void removeRegionOfInterest(AMRegionOfInterest *region);
//	/// Sets the bounding range for the given region of interest.
//	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region);

protected:
//	/// Returns the regions of interest list.
//	AMDbObjectList dbReadRegionsOfInterest();
//	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
//	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

	/// I0 ion chamber choice.
	VESPERS::IonChamber I0_;
	/// It ion chamber choice.
	VESPERS::IonChamber It_;
	/// Fluorescence detector choice.
	VESPERS::FluorescenceDetectors fluorescenceDetector_;
	/// Motor choice for which set of motors will be used.
	VESPERS::Motors motor_;
	/// CCD choice whether the scan should use a CCD detector or not.
	VESPERS::CCDDetectors ccdDetector_;
	/// The file name (minus number, path and extension of the file) for the CCD.
	QString ccdFileName_;
	/// The normal position of the scan.
	double normalPosition_;
//	/// The list of the regions of interest.
//	QList<AMRegionOfInterest *> regionsOfInterest_;
};

#endif // VESPERSSCANCONFIGURATIONDBOBJECT_H
