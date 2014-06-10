#ifndef AMDETECTORACQUISITIONACTIONINFO_H
#define AMDETECTORACQUISITIONACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorAcquisitionActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMDetectorAcquisitionActionInfo();
	Q_INVOKABLE AMDetectorAcquisitionActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::SingleRead,  QObject *parent = 0);

	/// Copy Constructor
	AMDetectorAcquisitionActionInfo(const AMDetectorAcquisitionActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMDetectorAcquisitionActionInfo* createCopy() const { return new AMDetectorAcquisitionActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Acquisition"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// Returns the read mode for this acquisition
	AMDetectorDefinitions::ReadMode readMode() const { return readMode_; }

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { delete newLoadedObject; }

protected:
	/// The AMDetectorInfo that specifies which detector to acquire
	AMDetectorInfo detectorInfo_;

	/// The read mode we want to acquire with
	AMDetectorDefinitions::ReadMode readMode_;
};

#endif // AMDETECTORACQUISITIONACTIONINFO_H
