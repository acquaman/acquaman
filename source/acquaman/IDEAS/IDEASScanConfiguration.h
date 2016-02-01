#ifndef IDEASSCANCONFIGURATION_H
#define IDEASSCANCONFIGURATION_H

#include "acquaman/IDEAS/IDEASScanConfigurationDbObject.h"
#include "application/IDEAS/IDEAS.h"

/*!
	This is a non-QObject class that holds the database object for a general
	scan configuration.  It has getters and setters to the database object.
	*/
class IDEASScanConfiguration
{
public:
	/// Constructor.
	IDEASScanConfiguration();
	/// Copy constructor.
	IDEASScanConfiguration(const IDEASScanConfiguration &original);
	/// Destructor.
	virtual ~IDEASScanConfiguration();

	// DbObject specific methods.
	////////////////////////////////////////////

	/// Returns the database object.  Intended for gaining access to its signals.
	IDEASScanConfigurationDbObject *dbObject() const { return dbObject_; }
	/// The database reading member function.
	AMDbObject *dbReadScanConfigurationDbObject() { return dbObject_; }
	/// The database writing member function.
	void dbWriteScanConfigurationDbObject(AMDbObject *object);

	// Getters
	/////////////////////////////////////////

	/// Returns the energy.
	double energy() const { return dbObject_->energy(); }
	/// Returns the current fluorescence detector choice.
	IDEAS::FluorescenceDetectors fluorescenceDetector() const { return dbObject_->fluorescenceDetector(); }
	/// Returns the list of regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return dbObject_->regionsOfInterest(); }

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Overloaded.  Returns the current total estimated time but also specifies whether the time should be recomputed first.
	double totalTime(bool recompute) { if (recompute) computeTotalTimeImplementation(); return totalTime_; }
	/// Returns the time offset.
	double timeOffset() const { return timeOffset_; }

	// Setters
	////////////////////////////////////////

	/// Sets the energy.
	void setEnergy(double newEnergy) { dbObject_->setEnergy(newEnergy); }
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(IDEAS::FluorescenceDetectors detector) { dbObject_->setFluorescenceDetector(detector); }
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetector(int detector) { setFluorescenceDetector((IDEAS::FluorescenceDetectors)detector); }
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region) { dbObject_->addRegionOfInterest(region); }
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region) { dbObject_->removeRegionOfInterest(region); }
	/// Sets the bounding range for the given region of interest.
	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region) { dbObject_->setRegionOfInterestBoundingRange(region); }

	/// Sets the time offset used for estimating the scan time.
	void setTimeOffset(double offset) { timeOffset_ = offset; computeTotalTimeImplementation(); }

protected:
	/// Computes the total time any time the regions list changes.  It is expected that subclasses will have a signal totalTimeChanged(double) that is emitted by this method.
	virtual void computeTotalTimeImplementation() = 0;

	// Header method and helper methods.
	////////////////////////////////////////

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	virtual QString headerText() const = 0;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void setHeaderText(QString) {}
	/// Returns a string for the header string for the given \param fluorescence detector.
	QString fluorescenceHeaderString(IDEAS::FluorescenceDetectors detector) const;
	/// Returns a string that displays all the regions of interest.
	QString regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const;

	//////////////////////////////////////////////////

	/// The databse object we're encapsulating.
	IDEASScanConfigurationDbObject *dbObject_;

	/// Holds the offset per point of extra time when doing a scan.
	double timeOffset_;
	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
};

#endif // IDEASSCANCONFIGURATION_H
