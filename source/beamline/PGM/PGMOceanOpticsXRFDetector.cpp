#include "PGMOceanOpticsXRFDetector.h"

PGMOceanOpticsXRFDetector::PGMOceanOpticsXRFDetector(const QString &name, const QString &description, QObject *parent) :
	AMXRFDetector(name, description, parent)
{
	acquireControl_ = 0;
	acquisitionStatusControl_ = new AMReadOnlyPVControl("AcquisitionStatus", "SA1611-01:Acquiring", this);
	acquireTimeControl_ = new AMSinglePVControl("AcquireTime", "BL1611-ID-2:dwell:setTime", this);
	// no elapsed time?

	spectraControls_.append(new AMReadOnlyPVControl("Raw spectrum", "SA1611-01:Spectra", this));

	allControlsCreated();
}

PGMOceanOpticsXRFDetector::~PGMOceanOpticsXRFDetector()
{

}

QString PGMOceanOpticsXRFDetector::details() const
{
	return QString("%1\nAcquisition Time: %2 seconds\n\n")
			.arg(description())
			.arg(acquisitionTime());
}

bool PGMOceanOpticsXRFDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}
