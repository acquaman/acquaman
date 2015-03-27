#ifndef AMDETECTORSETASDARKCURRENTTIMEACTIONINFO_H
#define AMDETECTORSETASDARKCURRENTTIMEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorSetAsDarkCurrentTimeActionInfo : public AMActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMDetectorSetAsDarkCurrentTimeActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), double secondsDwell = 0, QObject *parent = 0);
	/// Copy constructor.
	AMDetectorSetAsDarkCurrentTimeActionInfo(const AMDetectorSetAsDarkCurrentTimeActionInfo &other);
	/// Destructor.
	virtual ~AMDetectorSetAsDarkCurrentTimeActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action's type description.
	virtual QString typeDescription() const { return "Detector Set Dark Current Time"; }

	/// Returns the detector info.
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }
	/// Returns the dark current time to be set, in seconds.
	double darkCurrentTime() const { return darkCurrentTime_; }

	/// Returns the detector info for database storing.
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only.
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The AMDetectorInfo.
	AMDetectorInfo detectorInfo_;
	/// The desired dark current time.
	double darkCurrentTime_;
};

#endif // AMDETECTORSETASDARKCURRENTTIMEACTIONINFO_H
