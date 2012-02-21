#ifndef VESPERS2DSCANCONFIGURATION_H
#define VESPERS2DSCANCONFIGURATION_H

#include "acquaman/AM2DScanConfiguration.h"
#include "dataman/info/AMROIInfo.h"

/// This class is the VESPERS specific 2D map scan configuration.
/*!
	This class extends AM2DScanConfiguration to configure 2D map scans for the
	VESPERS beamline.  These scans are spatial scans in both directions to perform
	an XRF map or an XRF + XRD map simulatenously.  The scan configuration for both
	types are the same because the XRD is handled inside of the configuration file
	that is loaded into AM2DDacqScanController and so the only thing that this
	configuration needs to do is know whether this scan needs to load in the configuration
	file with XRD or not.
  */

class VESPERS2DScanConfiguration : public AM2DScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int incomingChoice READ incomingChoice WRITE setIncomingChoice)
	Q_PROPERTY(int fluorescenceDetectorChoice READ fluorescenceDetectorChoice WRITE setFluorescenceDetectorChoice)
	Q_PROPERTY(bool usingCCD READ usingCCD WRITE setUsingCCD)
	Q_PROPERTY(AMDbObject* roiInfoList READ dbGetROIInfoList WRITE dbLoadROIInfoList)
	Q_PROPERTY(QString rois READ readRoiList WRITE writeRoiList)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS 2D Scan Configuration")

public:
	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector { None = 0, SingleElement, FourElement };
	/// Enum for the ion chambers.  Used when choosing It and I0.
	enum IonChamber { Isplit = 0, Iprekb, Imini, Ipost };

	/// Constructor.
	Q_INVOKABLE VESPERS2DScanConfiguration(QObject *parent = 0);
	/// Copy Constructor.
	VESPERS2DScanConfiguration(const VESPERS2DScanConfiguration &original);

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the x-axis name.
	virtual QString xAxisName() const { return "Horizontal"; }
	/// Returns the x-axis units.
	virtual QString xAxisUnits() const { return "mm"; }
	/// Returns the y-axis name.
	virtual QString yAxisName() const  { return "Vertical"; }
	/// Returns teh y-axis units.
	virtual QString yAxisUnits() const  { return "mm"; }

	/// Returns the current I0 ion chamber choice.
	IonChamber incomingChoice() const { return I0_; }
	/// Returns the current fluorescence detector choice.
	FluorescenceDetector fluorescenceDetectorChoice() const { return fluorescenceDetectorChoice_; }
	/// Returns whether the scan is using the CCD or not.
	bool usingCCD() const { return usingCCD_; }

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Overloaded.  Returns the current total estimated time but also specifies whether the time should be recomputed first.
	double totalTime(bool recompute) { if (recompute) computeTotalTime(); return totalTime_; }
	/// Returns the time offset.
	double timeOffset() const { return timeOffset_; }

	/// Returns the ROI list.  The list is empty if not using a fluorescence detector.
	AMROIInfoList roiList() const { return roiInfoList_; }

	/// Get a pretty looking string of the current regions of interest.  Used primarily for exporting the list into the header of the file.
	QString readRoiList() const;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void writeRoiList(QString) {}

	// Database loading and storing
	///////////////////////

	/// The database reading member function.
	AMDbObject *dbGetROIInfoList() { return &roiInfoList_; }
	/// Don't need to do anything because dbGetROIList always returns a valid AMDbObject.
	void dbLoadROIInfoList(AMDbObject *) {}

signals:
	/// Notifier that the incoming choice has changed.
	void incomingChoiceChanged(IonChamber);
	/// Notifier that the fluorescence choice has changed.
	void fluorescenceDetectorChoiceChanged(FluorescenceDetector);
	/// Notifier that the flag for whether the CCD will be used has changed.
	void usingCCDChanged(bool);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:
	/// Sets the choice for I0 ion chamber.
	void setIncomingChoice(IonChamber I0) { I0_ = I0; emit incomingChoiceChanged(I0_); setModified(true); }
	/// Overloaded.  Used for database loading.
	void setIncomingChoice(int I0) { setIncomingChoice((IonChamber)I0); }
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetectorChoice(FluorescenceDetector detector) { fluorescenceDetectorChoice_ = detector; emit fluorescenceDetectorChoiceChanged(fluorescenceDetectorChoice_); setModified(true); }
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetectorChoice(int detector) { setFluorescenceDetectorChoice((FluorescenceDetector)detector); }
	/// Sets whether the scan should be using the CCD or not.
	void setUsingCCD(bool use) { usingCCD_ = use; emit usingCCDChanged(use); setModified(true); }
	/// Sets the time offset used for estimating the scan time.
	void setTimeOffset(double offset) { timeOffset_ = offset; computeTotalTime(); }

	/// Sets the ROI list.
	void setRoiInfoList(const AMROIInfoList &list) { roiInfoList_ = list; setModified(true); }

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime();

protected:
	/// I0 ion chamber choice.
	IonChamber I0_;
	/// Fluorescence detector choice.
	FluorescenceDetector fluorescenceDetectorChoice_;
	/// Flag holding whether the scan should use the CCD detector or not.
	bool usingCCD_;
	/// The list holding all the current ROIs for the detector.
	AMROIInfoList roiInfoList_;
	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
	/// Holds the offset per point of extra time when doing a scan.
	double timeOffset_;
};

#endif // VESPERS2DSCANCONFIGURATION_H
