#ifndef VESPERSUSERCONFIGURATION_H
#define VESPERSUSERCONFIGURATION_H

#include "dataman/database/AMDbObject.h"
#include "application/VESPERS/VESPERS.h"
#include "dataman/AMRegionOfInterest.h"

/// A class that holds onto the latest experiment configuration and saves it to the database.  This allows it to be seamlessly reloaded from the database upon startup.
class VESPERSUserConfiguration : public AMDbObject
{
	Q_PROPERTY(int incomingChoice READ incomingChoice WRITE setIncomingChoice)
	Q_PROPERTY(int transmissionChoice READ transmissionChoice WRITE setTransmissionChoice)
	Q_PROPERTY(int motor READ motor WRITE setMotor)
	Q_PROPERTY(int fluorescenceDetector READ fluorescenceDetector WRITE setFluorescenceDetector)
	Q_PROPERTY(int ccdDetector READ ccdDetector WRITE setCCDDetector)
	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS User Configuration Database Object")

public:
	/// Constructor.  Builds a user configuration.
	Q_INVOKABLE VESPERSUserConfiguration(QObject *parent = 0);

	/// Returns the current I0 ion chamber choice.
	VESPERS::IonChamber incomingChoice() const { return I0_; }
	/// Returns the current It ion chamber choice.
	VESPERS::IonChamber transmissionChoice() const { return It_; }
	/// Returns the current fluorescence detector choice.
	VESPERS::FluorescenceDetector fluorescenceDetector() const { return fluorescenceDetector_; }
	/// Returns the current motor choice.
	VESPERS::Motor motor() const { return motor_; }
	/// Returns what CCD the scan is using, if any.
	VESPERS::CCDDetector ccdDetector() const { return ccdDetector_; }
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

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
	void setFluorescenceDetector(VESPERS::FluorescenceDetector detector);
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetector(int detector) { setFluorescenceDetector((VESPERS::FluorescenceDetector)detector); }
	/// Sets the choice for the set of motors used for scanning.
	void setMotor(VESPERS::Motor choice);
	/// Overloaded.  Used for database loading.
	void setMotor(int choice) { setMotor((VESPERS::Motor)choice); }
	/// Sets whether the scan should be using the CCD or not.
	void setCCDDetector(VESPERS::CCDDetector ccd);
	/// Overloaded.  Used for database loading.
	void setCCDDetector(int ccd) { setCCDDetector((VESPERS::CCDDetector)ccd); }
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);

protected:
	/// I0 ion chamber choice.
	VESPERS::IonChamber I0_;
	/// It ion chamber choice.
	VESPERS::IonChamber It_;
	/// Fluorescence detector choice.
	VESPERS::FluorescenceDetector fluorescenceDetector_;
	/// Motor choice for which set of motors will be used.
	VESPERS::Motor motor_;
	/// CCD choice whether the scan should use a CCD detector or not.
	VESPERS::CCDDetector ccdDetector_;
	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;
};

#endif // VESPERSUSERCONFIGURATION_H
