#ifndef AMDETECTORINITIALIZEACTIONINFO_H
#define AMDETECTORINITIALIZEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorInitializeActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMDetectorInitializeActionInfo();
	Q_INVOKABLE AMDetectorInitializeActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);

	/// Copy Constructor
	AMDetectorInitializeActionInfo(const AMDetectorInitializeActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMDetectorInitializeActionInfo* createCopy() const { return new AMDetectorInitializeActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Initialization"; }

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

#endif // AMDETECTORINITIALIZEACTIONINFO_H
