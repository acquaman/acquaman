#ifndef AMDETECTORREADACTIONINFO_H
#define AMDETECTORREADACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorReadActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMDetectorReadActionInfo();
	Q_INVOKABLE AMDetectorReadActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);

	/// Copy Constructor
	AMDetectorReadActionInfo(const AMDetectorReadActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMDetectorReadActionInfo* createCopy() const { return new AMDetectorReadActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Read"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { delete newLoadedObject; }

protected:
	/// The AMDetectorInfo that specifies which detector to acquire
	AMDetectorInfo detectorInfo_;
};

#endif // AMDETECTORREADACTIONINFO_H
