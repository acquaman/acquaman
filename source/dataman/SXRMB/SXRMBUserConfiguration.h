#ifndef SXRMBUSERCONFIGURATION_H
#define SXRMBUSERCONFIGURATION_H

#include "dataman/database/AMDbObject.h"
#include "dataman/AMRegionOfInterest.h"

#include "application/SXRMB/SXRMB.h"

/// A class that holds onto the latest experiment configuration and saves it to the database.  This allows it to be seamlessly reloaded from the database upon startup.
class SXRMBUserConfiguration : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)
	Q_PROPERTY(int fluorescenceDetector READ fluorescenceDetector WRITE dbLoadFluorescenceDetector)

	Q_CLASSINFO("AMDbObject_Attributes", "description=SXRMB User Configuration Database Object")

public:
	/// Constructor.  Builds a user configuration.
	Q_INVOKABLE SXRMBUserConfiguration(QObject *parent = 0);
	/// Destructor.
	virtual ~SXRMBUserConfiguration();

	/// Returns the list of regions the configuration has a hold of.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }
	/// Returns the current fluorescence detector choice.
	SXRMB::FluorescenceDetectors fluorescenceDetector() const { return fluorescenceDetector_; }

public slots:
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);
	/// Updates a region of interest with a new bounding range.
	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region);
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(SXRMB::FluorescenceDetectors detector);

protected:
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);
	/// Overloaded.  Used for database loading.
	void dbLoadFluorescenceDetector(int detector);

protected:
	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;

	/// Fluorescence detector choice.
	SXRMB::FluorescenceDetectors fluorescenceDetector_;
};

#endif // SXRMBUSERCONFIGURATION_H
