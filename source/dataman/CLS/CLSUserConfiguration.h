#ifndef CLSUSERCONFIGURATION_H
#define CLSUSERCONFIGURATION_H


#include "dataman/database/AMDbObject.h"
#include "dataman/AMRegionOfInterest.h"


class CLSUserConfiguration: public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)

	Q_CLASSINFO("AMDbObject_Attributes", "description=CLS User Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE CLSUserConfiguration(QObject *parent = 0);
	/// Destructor.
	virtual ~CLSUserConfiguration();

	/// Returns the list of regions the configuration has a hold of.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }

public slots:
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);
	/// Updates a region of interest with a new bounding range.
	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region);

protected:
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

	/// Returns true if the given ROI is in the list already
	bool containsRegionOfInterest(AMRegionOfInterest *toFind) const;

	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;

};

#endif // CLSUSERCONFIGURATION_H
