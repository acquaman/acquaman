#ifndef AMDETECTORSETLASTMEASUREMENTASDARKCURRENTACTIONINFO_H
#define AMDETECTORSETLASTMEASUREMENTASDARKCURRENTACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorSetLastMeasurementAsDarkCurrentActionInfo : public AMActionInfo3
{
    Q_OBJECT
	Q_PROPERTY(AMDbObject* detectorInfo READ dbReadDetectorInfo WRITE dbLoadDetectorInfo)

public:
	/// Constructor.
	Q_INVOKABLE AMDetectorSetLastMeasurementAsDarkCurrentActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);
	/// Copy constructor.
	AMDetectorSetLastMeasurementAsDarkCurrentActionInfo(const AMDetectorSetLastMeasurementAsDarkCurrentActionInfo &other);
	/// Destructor.
	virtual ~AMDetectorSetLastMeasurementAsDarkCurrentActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;
	/// Returns the action's type description.
	virtual QString typeDescription() const { return "Set Detector's Last Measurement As Dark Current"; }
	/// Returns the detector info.
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

protected:

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

	/// The detector info.
	AMDetectorInfo detectorInfo_;
};

#endif // AMDETECTORSETLASTMEASUREMENTASDARKCURRENTACTIONINFO_H
