#ifndef VESPERSXRFSCANCONFIGURATION_H
#define VESPERSXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

class VESPERSXRFScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int detectorChoice READ detectorChoice WRITE setDetectorChoice)
	Q_PROPERTY(AMDbObject* xrfDetectorInfo READ dbReadXRFDetectorInfo WRITE dbLoadXRFDetectorInfo)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS XRF Scan Configuration")

public:
	/// Default constructor.
	Q_INVOKABLE explicit VESPERSXRFScanConfiguration(QObject *parent = 0);
	/// Convenience constructor.
	VESPERSXRFScanConfiguration(VESPERSBeamline::XRFDetectorChoice choice, XRFDetectorInfo info, QObject *parent = 0);

	/// Returns the detector info for the current detector.
	XRFDetectorInfo detectorInfo() const { return xrfDetectorInfo_; }

	/// Returns the XRF beamline enum for the detector choice.
	VESPERSBeamline::XRFDetectorChoice detectorChoice() const { return choice_; }

	/// Returns a new instance of the scan configuration.
	virtual AMScanConfiguration *createCopy() const;

	/// Returns a new instance of the scan controller.
	virtual AMScanController *createController();

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details.
	virtual QString description() const {
		return QString("XRF Free Run Scan");
	}

public slots:
	/// Sets the detector based on the beamline enum XRFDetectorChoice.
	void setDetectorChoice(VESPERSBeamline::XRFDetectorChoice choice);
	/// Sets the detector info to the given detector info.
	void setDetectorInfo(XRFDetectorInfo info) { xrfDetectorInfo_ = info; }

protected:
	/// Returns an AMDbObject pointer to the detector info.
	AMDbObject *dbReadXRFDetectorInfo() { return &xrfDetectorInfo_; }
	/// Empty function since it will never be called.
	void dbLoadXRFDetectorInfo(AMDbObject *) {}

	/// Overloaded.
	void setDetectorChoice(int choice) { setDetectorChoice((VESPERSBeamline::XRFDetectorChoice)choice); }

	// Member variables.
	/// Detector info member variable.
	XRFDetectorInfo xrfDetectorInfo_;

	/// The detector choice.  Which detector is being pointed to is determined by this enum.
	VESPERSBeamline::XRFDetectorChoice choice_;
};

#endif // VESPERSXRFSCANCONFIGURATION_H
