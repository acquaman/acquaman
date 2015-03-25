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

	Q_PROPERTY(int endstation READ endstation WRITE setEndstation)
	Q_PROPERTY(int fluorescenceDetectors READ fluorescenceDetectors WRITE setFluorescenceDetectors)
	Q_PROPERTY(double x READ x WRITE setX)
	Q_PROPERTY(double y READ y WRITE setY)
	Q_PROPERTY(double z READ z WRITE setZ)
	Q_PROPERTY(double rotation READ rotation WRITE setRotation)
	Q_PROPERTY(double energy READ energy WRITE setEnergy)
	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)

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

	/// Returns the endstation configuration of this configuration.
	SXRMB::Endsation endstation() const { return endstation_; }
	/// Returns the fluorescence detector configuration.
	SXRMB::FluorescenceDetectors fluorescenceDetectors() const { return fluorescenceDetectors_; }
	/// Returns the x position.
	double x() const { return x_; }
	/// Returns the y position.
	double y() const { return y_; }
	/// Returns the z position.
	double z() const { return z_; }
	/// Returns the rotation position.
	double rotation() const { return rotation_; }
	/// Returns the energy.
	double energy() const { return energy_; }
	/// Returns the list of regions the configuration has a hold of.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }

	/// Returns whether we are enabling Bruker Detector
	bool enableBrukerDetector() const { return enableBrukerDetector_; }

signals:
	/// Notifier that the endstation has changed.
	void endstationChanged(SXRMB::Endsation);
	/// Notifier that the fluorescence detectors have changed.
	void fluorescenceDetectorsChanged(SXRMB::FluorescenceDetectors);
	/// Notifier that the x position has changed.
	void xChanged(double);
	/// Notifier that the y position has changed.
	void yChanged(double);
	/// Notifier that the z position has changed.
	void zChanged(double);
	/// Notifier that the rotation position has changed.
	void rotationChanged(double);
	/// Notifier that the energy has changed.
	void energyChanged(double);

public slots:
	/// Sets the endstation value.
	void setEndstation(SXRMB::Endsation endstation);
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetectors(SXRMB::FluorescenceDetectors detector);
	/// Sets the x position.
	void setX(double newX);
	/// Sets the y position.
	void setY(double newY);
	/// Sets the z position.
	void setZ(double newZ);
	/// Sets the rotation position.
	void setRotation(double newRotation);
	/// Sets the energy position.
	void setEnergy(double newEnergy);
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);

	/// Sets the Bruker detector setting
	void setEnableBrukerDetector(bool enable);

protected:
	/// Overloaded.  Used for database loading.
	void setEndstation(int endstation);
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetectors(int detector);
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

	/// The endstation this scan was used on.
	SXRMB::Endsation endstation_;
	/// The fluorescence detector enum.
	SXRMB::FluorescenceDetectors fluorescenceDetectors_;
	/// The x position of the scan.
	double x_;
	/// The y position of the scan.
	double y_;
	/// The z position of the scan.
	double z_;
	/// The rotation position of the scan.
	double rotation_;
	/// The energy of the scan.
	double energy_;
	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;

	/// Flag to enable Bruker Detector, default: enable
	bool enableBrukerDetector_;
};

#endif // SXRMBSCANCONFIGURATIONDBOBJECT_H
