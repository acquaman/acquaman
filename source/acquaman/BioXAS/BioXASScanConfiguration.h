#ifndef BIOXASSCANCONFIGURATION_H
#define BIOXASSCANCONFIGURATION_H

#include "acquaman/BioXAS/BioXASScanConfigurationDbObject.h"

/*!
  This class is a non-QObject class that holds the database object for generic
  BioXAS scan details.  This class exists to bypass the limitation that you
  can't multiply inherit QObjects.
  */
class BioXASScanConfiguration
{

public:
	/// Constructor.
	BioXASScanConfiguration();
	/// Copy constructor.
	BioXASScanConfiguration(const BioXASScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASScanConfiguration();

	// DbObject specific methods.
	////////////////////////////////////////////

	/// Returns the database object.  Intended for gaining access to its signals.
	BioXASScanConfigurationDbObject *dbObject() const { return dbObject_; }

	// Getters
	/////////////////////////////////////////////

	/// Returns the energy.
	double energy() const { return dbObject_->energy(); }
	/// Returns the edge.
	QString edge() const { return dbObject_->edge(); }
	/// Returns whether the scan is using an XRF detector or not.
	bool usingXRFDetector() const { return dbObject_->usingXRFDetector(); }
	/// Returns the list of regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return dbObject_->regionsOfInterest(); }

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Overloaded.  Returns the current total estimated time but also specifies whether the time should be recomputed first.
	double totalTime(bool recompute);
	/// Returns the time offset.
	double timeOffset() const { return timeOffset_; }

	// Setters
	///////////////////////////////////////////

	/// Sets the energy.
	void setEnergy(double newEnergy) { dbObject_->setEnergy(newEnergy); }
	/// Sets the edge.
	void setEdge(const QString &newEdge) { dbObject_->setEdge(newEdge); }
	/// Sets whether the configuration is using an XRF detector.
	void setUsingXRFDetector(bool hasXRF) { dbObject_->setUsingXRFDetector(hasXRF); }
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region) { dbObject_->addRegionOfInterest(region); }
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region) { dbObject_->removeRegionOfInterest(region); }

	/// Sets the time offset used for estimating the scan time.
	void setTimeOffset(double offset) { timeOffset_ = offset; computeTotalTimeImplementation(); }

protected:
	/// Computes the total time any time the regions list changes.  It is expected that subclasses will have a signal totalTimeChanged(double) that is emitted by this method.
	virtual void computeTotalTimeImplementation() = 0;

	/// The database reading member function.
	AMDbObject *dbReadScanConfigurationDbObject() { return dbObject_; }
	/// The database writing member function.
	void dbWriteScanConfigurationDbObject(AMDbObject *object);

	// Header method and helper methods.
	////////////////////////////////////////

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	virtual QString headerText() const = 0;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void setHeaderText(QString) {}
	/// Returns a string that displays all the regions of interest.
	QString regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const;

	/////////////////////////////////////////////

	/// The database object we're encapsulating.
	BioXASScanConfigurationDbObject *dbObject_;

	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
	/// Holds the offset per point of extra time when doing a scan.
	double timeOffset_;
};

#endif // BIOXASSCANCONFIGURATION_H
