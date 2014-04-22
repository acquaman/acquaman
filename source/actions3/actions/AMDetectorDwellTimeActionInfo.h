#ifndef AMDETECTORDWELLTIMEACTIONINFO_H
#define AMDETECTORDWELLTIMEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorDwellTimeActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMDetectorDwellTimeActionInfo();
	Q_INVOKABLE AMDetectorDwellTimeActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), double dwellSeconds = 1,  QObject *parent = 0);

	/// Copy Constructor
	AMDetectorDwellTimeActionInfo(const AMDetectorDwellTimeActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMDetectorDwellTimeActionInfo* createCopy() const { return new AMDetectorDwellTimeActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Dwell Time"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// Returns the requested dwell time
	double dwellSeconds() const { return dwellSeconds_; }

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { delete newLoadedObject; }

protected:
	/// The AMDetectorInfo that specifies which detector to acquire
	AMDetectorInfo detectorInfo_;

	/// The requested dwell time
	double dwellSeconds_;
};

#endif // AMDETECTORDWELLTIMEACTIONINFO_H
