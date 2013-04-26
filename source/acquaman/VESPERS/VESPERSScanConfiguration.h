#ifndef VESPERSSCANCONFIGURATION_H
#define VESPERSSCANCONFIGURATION_H

#include "acquaman/VESPERS/VESPERSScanConfigurationDbObject.h"
#include "application/VESPERS/VESPERS.h"

/*!
  This class is a non-QObject class that holds the database object for a generic scan configuration.  It has getters
  and setters to the database object.  This class exists purely because classes can not multiply inherit two QObject
  classes.
  */
class VESPERSScanConfiguration
{
public:
	/// Constructor.
	VESPERSScanConfiguration();
	/// Copy constructor.
	VESPERSScanConfiguration(const VESPERSScanConfiguration &original);

	// DbObject specific methods.
	////////////////////////////////////////////

	/// Returns the database object.  Intended for gaining access to its signals.
	VESPERSScanConfigurationDbObject *dbObject() const { return dbObject_; }
	/// The database reading member function.
	AMDbObject *dbReadScanConfigurationDbObject() { return dbObject_; }
	/// The database writing member function.
	void dbWriteScanConfigurationDbObject(AMDbObject *object);

	// Getters
	/////////////////////////////////////////

	/// Returns the current I0 ion chamber choice.
	VESPERS::IonChamber incomingChoice() const { return dbObject_->incomingChoice(); }
	/// Returns the current It ion chamber choice.
	VESPERS::IonChamber transmissionChoice() const { return dbObject_->transmissionChoice(); }
	/// Returns the current fluorescence detector choice.
	VESPERS::FluorescenceDetector fluorescenceDetector() const { return dbObject_->fluorescenceDetector(); }
	/// Returns the current motor choice.
	VESPERS::Motor motor() const { return dbObject_->motor(); }
	/// Returns the CCD detector choice.
	VESPERS::CCDDetector ccdDetector() const { return dbObject_->ccdDetector(); }
	/// Returns the CCD file name.
	QString ccdFileName() const { return dbObject_->ccdFileName(); }
	/// Returns the ROI list.  The list is empty if not using a fluorescence detector.
	AMROIInfoList roiList() const { return dbObject_->roiList(); }

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Overloaded.  Returns the current total estimated time but also specifies whether the time should be recomputed first.
	double totalTime(bool recompute) { if (recompute) computeTotalTimeImplementation(); return totalTime_; }
	/// Returns the time offset.
	double timeOffset() const { return timeOffset_; }

	// Setters
	////////////////////////////////////////

	/// Sets the choice for I0 ion chamber.
	void setIncomingChoice(VESPERS::IonChamber I0) { dbObject_->setIncomingChoice(I0); }
	/// Overloaded.  Used for database loading.
	void setIncomingChoice(int I0) { setIncomingChoice((VESPERS::IonChamber)I0); }
	/// Sets the choice for It ion chamber.
	void setTransmissionChoice(VESPERS::IonChamber It) { dbObject_->setTransmissionChoice(It); }
	/// Overloaded.  Used for database loading.
	void setTransmissionChoice(int It) { setTransmissionChoice((VESPERS::IonChamber)It); }
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(VESPERS::FluorescenceDetector detector) { dbObject_->setFluorescenceDetector(detector); }
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetector(int detector) { setFluorescenceDetector((VESPERS::FluorescenceDetector)detector); }
	/// Sets the choice for the set of motors used for scanning.
	void setMotor(VESPERS::Motor choice) { dbObject_->setMotor(choice); }
	/// Overloaded.  Used for database loading.
	void setMotor(int choice) { setMotor((VESPERS::Motor)choice); }
	/// Sets whether the scan should be using the CCD or not.
	void setCCDDetector(VESPERS::CCDDetector ccd) { dbObject_->setCCDDetector(ccd); }
	/// Overloaded.  Used for database loading.
	void setCCDDetector(int ccd) { setCCDDetector((VESPERS::CCDDetector)ccd); }
	/// Sets the file name for the CCD files.
	void setCCDFileName(const QString &name) { dbObject_->setCCDFileName(name); }
	/// Sets the ROI list.
	void setRoiInfoList(const AMROIInfoList &list) { dbObject_->setRoiInfoList(list); }

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
	QString fluorescenceHeaderString(VESPERS::FluorescenceDetector detector) const;
	/// Returns a string for the I0 ion chamber.
	QString incomingChoiceHeaderString(VESPERS::IonChamber detector) const;
	/// Returns a string for the It ion chamber.
	QString transmissionChoiceHeaderString(VESPERS::IonChamber detector) const;
	/// Returns a string that is nicely formatted for the regions of interest.
	QString regionOfInterestHeaderString(AMROIInfoList list) const;
	/// Returns a string that with what motors were used based on \param motor.
	QString motorHeaderString(VESPERS::Motor motor) const;
	/// Returns a string that gives the name of the CCD images.
	QString ccdDetectorHeaderString(VESPERS::CCDDetector detector) const;

	////////////////////////////////////////

	/// The database object we're encapsulating.
	VESPERSScanConfigurationDbObject *dbObject_;

	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
	/// Holds the offset per point of extra time when doing a scan.
	double timeOffset_;
};

#endif // VESPERSSCANCONFIGURATION_H
