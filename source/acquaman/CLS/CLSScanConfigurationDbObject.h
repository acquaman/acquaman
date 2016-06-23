#ifndef CLSSCANCONFIGURATIONDBOBJECT_H
#define CLSSCANCONFIGURATIONDBOBJECT_H

#include "dataman/database/AMDbObject.h"
#include "dataman/AMRegionOfInterest.h"

class CLSScanConfigurationDbObject : public AMDbObject
{
    Q_OBJECT

	Q_PROPERTY(AMDbObjectList regionsOfInterest READ dbReadRegionsOfInterest WRITE dbLoadRegionsOfInterest)

	Q_CLASSINFO("AMDbObject_Attributes", "description=CLS Scan Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE CLSScanConfigurationDbObject(QObject *parent = 0);
	/// Copy constructor.
	CLSScanConfigurationDbObject(const CLSScanConfigurationDbObject &original);
	/// Destructor.
	virtual ~CLSScanConfigurationDbObject();

	/// Returns the list of regions the configuration has a hold of.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }

signals:

public slots:
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);
	/// Sets the bounding range for the given region of interest.
	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region);

protected:
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

protected:
	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;
};

#endif // CLSSCANCONFIGURATIONDBOBJECT_H
