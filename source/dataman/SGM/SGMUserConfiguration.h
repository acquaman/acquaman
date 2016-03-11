#ifndef SGMUSERCONFIGURATION_H
#define SGMUSERCONFIGURATION_H

#include "dataman/database/AMDbObject.h"
#include "dataman/AMRegionOfInterest.h"

/// A class that holds onto the latest experiment configuration and saves it to the database.  This allows it to be seamlessly reloaded from the database upon startup.
class SGMUserConfiguration : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)

	Q_CLASSINFO("AMDbObject_Attributes", "description=SGM User Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE SGMUserConfiguration(QObject *parent = 0);
	/// Destructor.
	virtual ~SGMUserConfiguration();

	/// Returns the list of regions the configuration has a hold of.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }

public slots:
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);

protected:
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

protected:
	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;
};

#endif // SGMUSERCONFIGURATION_H
