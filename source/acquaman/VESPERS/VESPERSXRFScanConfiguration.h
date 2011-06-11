#ifndef VESPERSXRFSCANCONFIGURATION_H
#define VESPERSXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "util/VESPERS/XRFPeriodicTable.h"

class VESPERSXRFScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* xrfDetectorInfo READ dbReadXRFDetectorInfo WRITE dbLoadXRFDetectorInfo)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS XRF Scan Configuration")

public:
	/// Default constructor.
	Q_INVOKABLE explicit VESPERSXRFScanConfiguration(QObject *parent = 0);
	/// Convenience constructor.
	VESPERSXRFScanConfiguration(XRFDetector *detector, QObject *parent = 0);
	/// Destructor.
	~VESPERSXRFScanConfiguration();

	/// Returns the detector info for the current detector.
	XRFDetectorInfo detectorInfo() const { return xrfDetectorInfo_; }

	/// Returns a new instance of the scan configuration.
	virtual AMScanConfiguration *createCopy() const;

	/// Returns a new instance of the scan controller.
	virtual AMScanController *createController();

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details.
	virtual QString description() const {
		return QString("XRF Free Run Scan");
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const;

	// Non-database functions associated with the configuration.

	/// Returns the detector used in this configuration.
	XRFDetector *detector() const { return detector_; }
	/// Returns the XRF periodic table used by this configuration.
	XRFPeriodicTable *table() const { return xrfTable_; }

public slots:
	/// Sets the detector info to the given detector info.
	void setDetectorInfo(XRFDetectorInfo info) { xrfDetectorInfo_ = info; }
	/// Sets the detector.
	void setDetector(XRFDetector *detector) { detector_ = detector; }

protected slots:
	/// Handles what happens when the detector becomes connected.
	void onRoisHaveValues(bool hasValues);
	/// Handles when the regions of interest change from an external source.
	void onExternalRegionsOfInterestChanged();

protected:
	/// Returns an AMDbObject pointer to the detector info.
	AMDbObject *dbReadXRFDetectorInfo() { return &xrfDetectorInfo_; }
	/// Empty function since it will never be called.
	void dbLoadXRFDetectorInfo(AMDbObject *) {}

	/// Helper function that takes in a region of interest name and adds it to the XRFPeriodicTable.  Takes in the ROI name, finds the element and line it is associated with and adds it to the XRFPeriodicTable.
	void addRegionOfInterestToTable(QString name);

	// Member variables.
	/// Detector info member variable.
	XRFDetectorInfo xrfDetectorInfo_;

	/// The detector itself.  This has live beamline communications.
	XRFDetector *detector_;

	/// The periodic table that holds information about the regions of interest.
	XRFPeriodicTable *xrfTable_;
};

#endif // VESPERSXRFSCANCONFIGURATION_H
