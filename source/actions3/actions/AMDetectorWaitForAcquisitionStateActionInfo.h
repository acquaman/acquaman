#ifndef AMDETECTORWAITFORACQUISITIONSTATEACTIONINFO_H
#define AMDETECTORWAITFORACQUISITIONSTATEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

/// Holds the info for an action that waits for a specific acquisition state transition in the detector.
class AMDetectorWaitForAcquisitionStateActionInfo : public AMActionInfo3
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMDetectorWaitForAcquisitionStateActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), int acquisitionState = 0, QObject *parent = 0);
	/// Copy constructor.
	AMDetectorWaitForAcquisitionStateActionInfo(const AMDetectorWaitForAcquisitionStateActionInfo &other);
	/// Destructor.
	virtual ~AMDetectorWaitForAcquisitionStateActionInfo();

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3 *createCopy() const;

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Initialization"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }
	/// Returns the state as an int.
	int acquisitionState() const { return acquisitionState_; }

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

	/// Sets the acquisition state we're waiting on.
	void setAcquisitionState(int newState);

protected:
	/// The AMDetectorInfo that specifies which detector to initialize
	AMDetectorInfo detectorInfo_;
	/// The state enum as an integer.
	int acquisitionState_;
};

#endif // AMDETECTORWAITFORACQUISITIONSTATEACTIONINFO_H
