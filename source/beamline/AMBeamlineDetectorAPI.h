#ifndef AMBEAMLINEDETECTORAPI_H
#define AMBEAMLINEDETECTORAPI_H

#include <QString>

#include "dataman/info/AMDetectorInfo.h"

class AMDetector;

class AMBeamlineDetectorAPI {
public:
	AMBeamlineDetectorAPI();

	virtual ~AMBeamlineDetectorAPI();

	/// Returns a detector based on the name of the detector. Returns 0 if no detector is found.
	virtual AMDetector* exposedDetectorByName(const QString &detectorName) { Q_UNUSED(detectorName); return 0; }
	/// Returns a detector based on the detector info. Returns 0 if no control is found.
	virtual AMDetector* exposedDetectorByInfo(const AMDetectorInfo &detectorInfo) { Q_UNUSED(detectorInfo); return 0; }

	/// Returns true if the detector referred to by this detector info is available (connected)
	virtual bool detectorAvailable(const AMDetectorInfo &detectorInfo) { Q_UNUSED(detectorInfo); return false; }

	/// Adds a detector to the exposed set. Returns whether or not the detector was successfully added.
	virtual bool addExposedDetector(AMDetector *detector) { Q_UNUSED(detector); return false; }
};

#endif // AMBEAMLINEDETECTORAPI_H
