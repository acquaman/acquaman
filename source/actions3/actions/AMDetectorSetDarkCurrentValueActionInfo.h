#ifndef AMDETECTORSETDARKCURRENTVALUEACTIONINFO_H
#define AMDETECTORSETDARKCURRENTVALUEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorSetDarkCurrentValueActionInfo : public AMActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMDetectorSetDarkCurrentValueActionInfo(double newValue = 0, const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);
	/// Copy constructor.
	AMDetectorSetDarkCurrentValueActionInfo(const AMDetectorSetDarkCurrentValueActionInfo &other);
	/// Destructor.
	virtual ~AMDetectorSetDarkCurrentValueActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action's type description.
	virtual QString typeDescription() const { return "Set Detector Dark Current Value"; }

	/// Returns the detector info.
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }
	/// Returns the dark current value to be set.
	double darkCurrentValue() const { return darkCurrentValue_; }

	/// Returns the detector info for database storing.
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only.
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The AMDetectorInfo.
	AMDetectorInfo detectorInfo_;
	/// The desired dark current value.
	double darkCurrentValue_;

};

#endif // AMDETECTORSETDARKCURRENTVALUEACTIONINFO_H
