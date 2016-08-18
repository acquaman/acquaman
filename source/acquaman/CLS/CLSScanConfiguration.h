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
	CLSScanConfigurationDbObject *dbObject() const { return scanConfigurationDbObject_; }
	/// The database reading member function.
	AMDbObject *dbReadScanConfigurationDbObject() { return scanConfigurationDbObject_; }
	/// The database writing member function.
	void dbWriteScanConfigurationDbObject(AMDbObject *dbObject);

	// Getters
	/////////////////////////////////////////

	/// Returns the list of regions of interest.
	virtual QList<AMRegionOfInterest *> regionsOfInterest() const { return scanConfigurationDbObject_->regionsOfInterest(); }

	// Setters
	/////////////////////////////////////////

	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region) { scanConfigurationDbObject_->addRegionOfInterest(region); }
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region) { scanConfigurationDbObject_->removeRegionOfInterest(region); }
	/// Sets the bounding range for the given region of interest.
	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region) { scanConfigurationDbObject_->setRegionOfInterestBoundingRange(region); }

protected:
	/// Returns a string that displays all the regions of interest.
	QString regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const;

protected:
	/// The database object we're encapsulating.
	CLSScanConfigurationDbObject *scanConfigurationDbObject_;

};

#endif // CLSSCANCONFIGURATION_H
