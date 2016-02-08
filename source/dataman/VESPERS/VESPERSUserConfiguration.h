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


#ifndef VESPERSUSERCONFIGURATION_H
#define VESPERSUSERCONFIGURATION_H

#include "dataman/database/AMDbObject.h"
#include "application/VESPERS/VESPERS.h"
#include "dataman/AMRegionOfInterest.h"

/// A class that holds onto the latest experiment configuration and saves it to the database.  This allows it to be seamlessly reloaded from the database upon startup.
class VESPERSUserConfiguration : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(int incomingChoice READ incomingChoice WRITE setIncomingChoice)
	Q_PROPERTY(int transmissionChoice READ transmissionChoice WRITE setTransmissionChoice)
	Q_PROPERTY(int motor READ motor WRITE setMotor)
	Q_PROPERTY(int fluorescenceDetector READ fluorescenceDetector WRITE setFluorescenceDetector)
	Q_PROPERTY(int ccdDetector READ ccdDetector WRITE setCCDDetector)
	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS User Configuration Database Object")

	Q_CLASSINFO("incomingChoice", "upgradeDefault=2")
	Q_CLASSINFO("transmissionChoice", "upgradeDefault=3")
	Q_CLASSINFO("motor", "upgradeDefault=3")
	Q_CLASSINFO("fluorescenceDetector", "upgradeDefault=1")
	Q_CLASSINFO("ccdDetector", "upgradeDefault=0")

public:
	/// Constructor.  Builds a user configuration.
	Q_INVOKABLE VESPERSUserConfiguration(QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSUserConfiguration();

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
	/// Returns the list of regions the configuration has a hold of.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }

public slots:
	/// Sets the choice for I0 ion chamber.
	void setIncomingChoice(VESPERS::IonChamber I0);
	/// Sets the choice for It ion chamber.
	void setTransmissionChoice(VESPERS::IonChamber It);
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(VESPERS::FluorescenceDetectors detector);
	/// Sets the choice for the set of motors used for scanning.
	void setMotor(VESPERS::Motors choice);
	/// Sets whether the scan should be using the CCD or not.
	void setCCDDetector(VESPERS::CCDDetectors ccd);
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);
	/// Updates a region of interest with a new bounding range.
	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region);

protected slots:
	/// Overloaded.  Used for database loading.
	void setIncomingChoice(int I0) { setIncomingChoice((VESPERS::IonChamber)I0); }
	/// Overloaded.  Used for database loading.
	void setTransmissionChoice(int It) { setTransmissionChoice((VESPERS::IonChamber)It); }
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetector(int detector) { setFluorescenceDetector((VESPERS::FluorescenceDetectors)detector); }
	/// Overloaded.  Used for database loading.
	void setMotor(int choice) { setMotor((VESPERS::Motors)choice); }
	/// Overloaded.  Used for database loading.
	void setCCDDetector(int ccd) { setCCDDetector((VESPERS::CCDDetectors)ccd); }

protected:
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

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
	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;
};

#endif // VESPERSUSERCONFIGURATION_H
