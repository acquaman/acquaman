#ifndef AMCONTROLSTOPACTIONINFO_H
#define AMCONTROLSTOPACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"

/// This action info specifies the very limited amount of information needed to stop a control.
class AMControlStopActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* controlInfo READ dbReadControlInfo WRITE dbLoadControlInfo)

public:
	/// Constructor.  Builds the action info required to stop a control.
 	virtual ~AMControlStopActionInfo();
	Q_INVOKABLE AMControlStopActionInfo(const AMControlInfo &setpoint = AMControlInfo(), QObject *parent = 0);
	/// Copy contructor.
	AMControlStopActionInfo(const AMControlStopActionInfo &other);

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const { return new AMControlStopActionInfo(*this); }
	/// Short description of the type of action this is.
	virtual QString typeDescription() const { return "Control Stop"; }

	/// Returns a pointer to our move destination setpoint
	const AMControlInfo* controlInfo() const { return &controlInfo_; }
	/// Set the move destination setpoint, including the control name, value, and description.
	/*! \note We make a copy of \c controlInfo's values, and do not retain any reference to it afterward. */
	void setControlInfo(const AMControlInfo& controlInfo);

	// Database loading/storing
	////////////////////////////

	/// For database storing only.
	AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
	/// For database loading only. This function will never be called since dbReadControlInfo() always returns a valid setpoint, but it needs to be here.
	void dbLoadControlInfo(AMDbObject* newLoadedObject) { delete newLoadedObject; }

protected:
	/// The AMControlInfo that specifies where to move to
	AMControlInfo controlInfo_;
};

#endif // AMCONTROLSTOPACTIONINFO_H
