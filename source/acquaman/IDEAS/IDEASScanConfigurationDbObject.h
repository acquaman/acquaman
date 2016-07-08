#ifndef IDEASSCANCONFIGURATIONDBOBJECT_H
#define IDEASSCANCONFIGURATIONDBOBJECT_H

//#include "dataman/database/AMDbObject.h"
//#include "dataman/AMRegionOfInterest.h"
#include "acquaman/CLS/CLSScanConfigurationDbObject.h"
#include "application/IDEAS/IDEAS.h"

/// 3 - CLS  007 - ideas  06 - scan configuration db
#define ERR_IDEAS_SCAN_CONFIGURATION_DB_OBJECT_INVALID_COPY_CONSTRUCTOR 30070601

/// This class is the common database object for all scan configurations for the IDEAS beamline.
class IDEASScanConfigurationDbObject : public CLSScanConfigurationDbObject
{
	Q_OBJECT

	Q_PROPERTY(double energy READ energy WRITE setEnergy)
	Q_PROPERTY(int fluorescenceDetector READ fluorescenceDetector WRITE setFluorescenceDetector)
//	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)

	Q_CLASSINFO("AMDbObject_Attributes", "description=IDEAS Scan Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE IDEASScanConfigurationDbObject(QObject *parent = 0);
	/// Copy constructor.
	IDEASScanConfigurationDbObject(const CLSScanConfigurationDbObject &original);
	/// Destructor.
	virtual ~IDEASScanConfigurationDbObject();

	/// Returns the edge energy for the scan.
	double energy() const { return energy_; }
	/// Returns the current fluorescence detector choice.
	IDEAS::FluorescenceDetectors fluorescenceDetector() const { return fluorescenceDetector_; }
//	/// Returns the list of regions the configuration has a hold of.
//	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }

signals:
	/// Notifier that the edge energy has changed.
	void energyChanged(double);
	/// Notifier that the fluorescence choice has changed.
	void fluorescenceDetectorChanged(IDEAS::FluorescenceDetectors);
	/// Same signal.  Just passing as an int.
	void fluorescenceDetectorChanged(int);

public slots:
	/// Sets the edge energy.
	void setEnergy(double edgeEnergy);
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(IDEAS::FluorescenceDetectors detector);
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetector(int detector) { setFluorescenceDetector((IDEAS::FluorescenceDetectors)detector); }
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

	/// The edge energy for the scan.
	double energy_;
	/// Fluorescence detector choice.
	IDEAS::FluorescenceDetectors fluorescenceDetector_;
//	/// The list of the regions of interest.
//	QList<AMRegionOfInterest *> regionsOfInterest_;
};

#endif // IDEASSCANCONFIGURATIONDBOBJECT_H
