#ifndef VESPERSXRFSCANCONFIGURATION_H
#define VESPERSXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "beamline/VESPERS/XRFDetector.h"

class VESPERSXRFScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

public:
	/// Default constructor.
	VESPERSXRFScanConfiguration(XRFDetector *detector, QObject *parent = 0);

	/// Returns the detector used in this configuration.
	XRFDetector *detector() const { return detector_; }

	/// Returns a new instance of the scan configuration.
	virtual AMScanConfiguration *createCopy() const;

	/// Returns a new instance of the scan controller.
	virtual AMScanController *createController();

protected:
	/// The detector used in the configuration.  Currently, can only be the single element or the four element.
	XRFDetector *detector_;
};

#endif // VESPERSXRFSCANCONFIGURATION_H
