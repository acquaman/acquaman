#ifndef AMBEAMLINEDETECTORSETAPI_H
#define AMBEAMLINEDETECTORSETAPI_H

#include <QString>

class AMDetectorSet;

class AMBeamlineDetectorSetAPI {
public:
	AMBeamlineDetectorSetAPI();

	/// Returns the detector set that contains all of the public detectors. These are used with scan actions and configurations for automatic lookup.
	virtual AMDetectorSet* exposedDetectors() const { return 0; }

	/// Returns a new detector set which includes a subset of detectors for a particular keyed purpose (passing the type of a scan should return the detectors that can be used for that type of scan). The detector set is a new object, so the caller is responsible for the memory. The default implementation returns a NULL pointer.
	virtual AMDetectorSet* detectorsFor(const QString &key) { Q_UNUSED(key); return 0; }
};

#endif // AMBEAMLINEDETECTORSETAPI_H
