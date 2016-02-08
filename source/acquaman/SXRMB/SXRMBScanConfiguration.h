#ifndef SXRMBSCANCONFIGURATION_H
#define SXRMBSCANCONFIGURATION_H

#include "acquaman/SXRMB/SXRMBScanConfigurationDbObject.h"

/*!
  This class is a non-QObject class that holds the database object for a generic scan configuration.  It has getters
  and setters to the database object.  This class exists purely because classes can not multiply inherit two QObject
  classes.
*/
class SXRMBScanConfiguration
{
public:
	/// Constructor.
	SXRMBScanConfiguration();
	/// Copy constructor.
	SXRMBScanConfiguration(const SXRMBScanConfiguration &original);
	/// Destructor.
	virtual ~SXRMBScanConfiguration();

	// DbObject specific methods.
	////////////////////////////////////////////

	/// Returns the database object.  Intended for gaining access to its signals.
	SXRMBScanConfigurationDbObject *dbObject() const { return dbObject_; }
	/// The database reading member function.
	AMDbObject *dbReadScanConfigurationDbObject() { return dbObject_; }
	/// The database writing member function.
	void dbWriteScanConfigurationDbObject(AMDbObject *object);

	// Getters
	/////////////////////////////////////////

	/// Returns the endstation.
	SXRMB::Endstation endstation() const { return dbObject_->endstation(); }
	/// Returns the fluorescence detectors.
	SXRMB::FluorescenceDetectors fluorescenceDetector() const { return dbObject_->fluorescenceDetector(); }
	/// Returns the flag to power on HV control
	bool powerOnHVControl() const;
	/// Returns the x position.
	double x() const { return dbObject_->x(); }
	/// Returns the y position.
	double y() const { return dbObject_->y(); }
	/// Returns the z position.
	double z() const { return dbObject_->z(); }
	/// Returns the rotation.
	double rotation() const { return dbObject_->rotation(); }
	/// Returns the energy.
	double energy() const { return dbObject_->energy(); }
	/// Returns the list of regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return dbObject_->regionsOfInterest(); }

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Overloaded.  Returns the current total estimated time but also specifies whether the time should be recomputed first.
	double totalTime(bool recompute);
	/// Returns the time offset.
	double timeOffset() const { return timeOffset_; }

	// Setters
	////////////////////////////////////////

	/// Sets the endstation.
	void setEndstation(SXRMB::Endstation newEndstation) { dbObject_->setEndstation(newEndstation); }
	/// Sets the fluorescence detectors.
	void setFluorescenceDetector(SXRMB::FluorescenceDetectors newFluorescenceDetectors) { dbObject_->setFluorescenceDetector(newFluorescenceDetectors); }
	/// Sets the x position.
	void setX(double newX) { dbObject_->setX(newX); }
	/// Sets the y position.
	void setY(double newY) { dbObject_->setY(newY); }
	/// Sets the z position.
	void setZ(double newZ) { dbObject_->setZ(newZ); }
	/// Sets the rotation position.
	void setRotation(double newRotation) { dbObject_->setRotation(newRotation); }
	/// Sets the energy.
	void setEnergy(double newEnergy) { dbObject_->setEnergy(newEnergy); }
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region) { dbObject_->addRegionOfInterest(region); }
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region) { dbObject_->removeRegionOfInterest(region); }
	/// Sets the bounding range for the given region of interest.
	void setRegionOfInterestBoundingRange(AMRegionOfInterest *region) { dbObject_->setRegionOfInterestBoundingRange(region); }

	/// Sets the time offset used for estimating the scan time.
	void setTimeOffset(double offset) { timeOffset_ = offset; computeTotalTimeImplementation(); }

	/// Sets power on HV Control flag
	void setPowerOnHVControlEnabled(bool value) { powerOnHVControl_ = value; }

protected:
	/// Computes the total time any time the regions list changes.  It is expected that subclasses will have a signal totalTimeChanged(double) that is emitted by this method.
	virtual void computeTotalTimeImplementation() = 0;

	// Header method and helper methods.
	////////////////////////////////////////

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	virtual QString headerText() const = 0;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void setHeaderText(QString) {}
	/// Returns a string that displays all the regions of interest.
	QString regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const;


protected:
	/// The database object we're encapsulating.
	SXRMBScanConfigurationDbObject *dbObject_;

	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
	/// Holds the offset per point of extra time when doing a scan.
	double timeOffset_;

	/// the flag to auto turn the TEY HV control on
	bool powerOnHVControl_;
};

#endif // SXRMBSCANCONFIGURATION_H
