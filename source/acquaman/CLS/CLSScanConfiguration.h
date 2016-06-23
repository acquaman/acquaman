#ifndef CLSSCANCONFIGURATION_H
#define CLSSCANCONFIGURATION_H

#include "acquaman/CLS/CLSScanConfigurationDbObject.h"

class AMRegionOfInterest;

class CLSScanConfiguration
{
public:
	CLSScanConfiguration(CLSScanConfigurationDbObject *dbObject);
	/// Destructor.
	virtual ~CLSScanConfiguration();

	// DbObject specific methods.
	////////////////////////////////////////////

	/// Returns the database object.  Intended for gaining access to its signals.
	CLSScanConfigurationDbObject *dbObject() const { return configurationDbObject_; }
	/// The database reading member function.
	AMDbObject *dbReadScanConfigurationDbObject() { return configurationDbObject_; }
	/// The database writing member function.
	void dbWriteScanConfigurationDbObject(AMDbObject *object);

	// Getters
	/////////////////////////////////////////

	/// Returns the list of regions of interest.
	virtual QList<AMRegionOfInterest *> regionsOfInterest() const { return configurationDbObject_->regionsOfInterest(); }

	// Setters
	/////////////////////////////////////////

	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region) { configurationDbObject_->addRegionOfInterest(region); }
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region) { configurationDbObject_->removeRegionOfInterest(region); }
	/// Sets the bounding range for the given region of interest.
	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region) { configurationDbObject_->setRegionOfInterestBoundingRange(region); }

protected:
	/// Returns a string that displays all the regions of interest.
	QString regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const;

protected:
	/// The database object we're encapsulating.
	CLSScanConfigurationDbObject *configurationDbObject_;

};

#endif // CLSSCANCONFIGURATION_H
