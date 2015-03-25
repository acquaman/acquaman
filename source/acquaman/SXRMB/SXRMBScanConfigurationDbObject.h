#ifndef SXRMBSCANCONFIGURATIONDBOBJECT_H
#define SXRMBSCANCONFIGURATIONDBOBJECT_H

#include "dataman/database/AMDbObject.h"
#include "dataman/AMRegionOfInterest.h"
#include "application/SXRMB/SXRMB.h"

/// This class is the common database object for all scan configurations for the SXRMB beamline.
/*!
	This class holds all of the persistent values that all scan configurations will need.  It is meant
	to consolidate all the duplications from all the scan configurations.  The only requirement for
	any class is that they'll need to set it in their Q_PROPERTY so that they properly load the
	configuration.
  */
class SXRMBScanConfigurationDbObject : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(double normalPosition READ normalPosition WRITE setNormalPosition)
	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)
	Q_PROPERTY(int endstation READ endstation WRITE setEndstation)
	Q_PROPERTY(int fluorescenceDetectors READ fluorescenceDetectors WRITE setFluorescenceDetectors)
	Q_PROPERTY(int ionChambers READ ionChambers WRITE setIonChambers)
	Q_PROPERTY(bool enableBrukerDetector READ enableBrukerDetector WRITE setEnableBrukerDetector)

	Q_CLASSINFO("enableBrukerDetector", "upgradeDefault=false")

	Q_CLASSINFO("AMDbObject_Attributes", "description=SXRMB Scan Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE SXRMBScanConfigurationDbObject(QObject *parent = 0);
	/// Copy constructor.
	SXRMBScanConfigurationDbObject(const SXRMBScanConfigurationDbObject &original);
	/// Destructor.
	virtual ~SXRMBScanConfigurationDbObject();

	/// Returns the normal position.
	double normalPosition() const { return normalPosition_; }
	/// Returns the list of regions the configuration has a hold of.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }
	/// Returns the endstation configuration of this configuration.
	SXRMB::Endsation endstation() const { return endstation_; }
	/// Returns the fluorescence detector configuration.
	SXRMB::FluorescenceDetectors fluorescenceDetectors() const { return fluorescenceDetectors_; }
	/// Returns the ion chamber configuration.
	SXRMB::IonChambers ionChambers() const { return ionChambers_; }

	/// Returns whether we are enabling Bruker Detector
	bool enableBrukerDetector() const { return enableBrukerDetector_; }

signals:
	/// Notifier that the normal position has changed.
	void normalPositionChanged(double);

public slots:
	/// Sets the normal position.
	void setNormalPosition(double newPosition);
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);
	/// Sets the endstation value.
	void setEndstation(SXRMB::Endsation endstation);
	/// Overloaded.  Used for database loading.
	void setEndstation(int endstation) { setEndstation(SXRMB::Endsation(endstation)); }
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetectors(SXRMB::FluorescenceDetectors detector);
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetectors(int detector) { setFluorescenceDetectors((SXRMB::FluorescenceDetectors)detector); }
	/// Sets the ion chamber configuration.
	void setIonChambers(SXRMB::IonChambers ionChamber);
	/// Overloaded.  Used for database loading.
	void setIonChambers(int ionChamber) { setIonChambers(SXRMB::IonChambers(ionChamber)); }

	/// Sets the Bruker detector setting
	void setEnableBrukerDetector(bool enable);

protected:
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

	/// The normal position of the scan.
	double normalPosition_;
	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;
	/// The endstation this scan was used on.
	SXRMB::Endsation endstation_;
	/// The fluorescence detector enum.
	SXRMB::FluorescenceDetectors fluorescenceDetectors_;
	/// The ion chamber configuration.
	SXRMB::IonChambers ionChambers_;

	/// Flag to enable Bruker Detector, default: enable
	bool enableBrukerDetector_;
};

#endif // SXRMBSCANCONFIGURATIONDBOBJECT_H
