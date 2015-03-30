#ifndef AMDETECTORSETDARKCURRENTVALIDSTATEACTIONINFO_H
#define AMDETECTORSETDARKCURRENTVALIDSTATEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorSetDarkCurrentValidStateActionInfo : public AMActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMDetectorSetDarkCurrentValidStateActionInfo(bool newState, const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);
	/// Copy constructor.
	AMDetectorSetDarkCurrentValidStateActionInfo(const AMDetectorSetDarkCurrentValidStateActionInfo &other);
	/// Destructor.
	virtual ~AMDetectorSetDarkCurrentValidStateActionInfo();

	/// Virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action's type description.
	virtual QString typeDescription() const { return "Set Detector Dark Current Valid State"; }

	/// Returns the new state to be set by this action.
	bool darkCurrentState() const { return darkCurrentState_; }
	/// Returns the info for the detector to be changed.
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// Returns the detector info for database storing.
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only.
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The new dark current valid state to be set.
	bool darkCurrentState_;
	/// The info for the detector to be changed.
	AMDetectorInfo detectorInfo_;
};

#endif // AMDETECTORSETDARKCURRENTVALIDSTATEACTIONINFO_H
