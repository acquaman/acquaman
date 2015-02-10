#ifndef AMCHANGETOLERANCEACTIONINFO_H
#define AMCHANGETOLERANCEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"

/// This class changes the tolerance of the control associated with the given control info to a new value.
class AMChangeToleranceActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* controlInfo READ dbReadControlInfo WRITE dbLoadControlInfo)
	Q_PROPERTY(double tolerance READ tolerance WRITE setTolerance)

public:

	/// Constructor.
	Q_INVOKABLE AMChangeToleranceActionInfo(const AMControlInfo &control = AMControlInfo(), double newTolerance = 0.0, QObject *parent = 0);
	/// Copy constructor.
	AMChangeToleranceActionInfo(const AMChangeToleranceActionInfo &other);
	/// Destructor.
	~AMChangeToleranceActionInfo();

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const;

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Control Wait"; }

	// New public functions
	//////////////////////////

	/// Returns a pointer to our move destination setpoint
	const AMControlInfo& controlInfo() const { return controlInfo_; }

	/// Returns our timeout.
	double tolerance() const { return tolerance_; }

	/// Set the move destination setpoint, including the control name, value, and description.
	/*! \note We make a copy of \c controlInfo's values, and do not retain any reference to it afterward. */
	void setControlInfo(const AMControlInfo& controlInfo);
	/// Set new tolerance value (value only).
	void setTolerance(double newTolerance);

protected:
	// Database loading/storing
	////////////////////////////

	/// For database storing only.
	AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
	/// For database loading only. This function will never be called since dbReadControlInfo() always returns a valid setpoint, but it needs to be here.
	void dbLoadControlInfo(AMDbObject* newLoadedObject) { newLoadedObject->deleteLater(); }

	/// The AMControlInfo that specifies where to move to
	AMControlInfo controlInfo_;
	/// The max time to wait for the control to reach a setpoint. (seconds)
	double tolerance_;
};

#endif // AMCHANGETOLERANCEACTIONINFO_H
