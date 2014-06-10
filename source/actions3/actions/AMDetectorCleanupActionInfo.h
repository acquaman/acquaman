#ifndef AMDETECTORCLEANUPACTIONINFO_H
#define AMDETECTORCLEANUPACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorCleanupActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMDetectorCleanupActionInfo();
	Q_INVOKABLE AMDetectorCleanupActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);

	/// Copy Constructor
	AMDetectorCleanupActionInfo(const AMDetectorCleanupActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMDetectorCleanupActionInfo* createCopy() const { return new AMDetectorCleanupActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Cleanup"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { delete newLoadedObject; }

protected:
	/// The AMDetectorInfo that specifies which detector to initialize
	AMDetectorInfo detectorInfo_;
};

#endif // AMDETECTORCLEANUPACTIONINFO_H
