#ifndef AMCONTROLMOVEACTIONINFO_H
#define AMCONTROLMOVEACTIONINFO_H

#include "actions2/AMActionInfo.h"
#include "dataman/info/AMControlInfoList.h"

/// This AMActionInfo-subclass specifies the information for AMControlMoveAction -- an action that moves a control to a setpoint.  This info specifies the setpoint (the name of the control and where to move it to), in the form of an AMControlInfo.
class AMControlMoveActionInfo : public AMActionInfo
{
    Q_OBJECT
	Q_PROPERTY(AMDbObject* controlInfo READ dbReadControlInfo WRITE dbLoadControlInfo)

public:
	/// Constructor. You should always specify a valid \c setpoint, but we provide the default argument because we need a default constructor for database loading.
	Q_INVOKABLE AMControlMoveActionInfo(const AMControlInfo& setpoint = AMControlInfo(), QObject *parent = 0);

	/// Copy Constructor
	AMControlMoveActionInfo(const AMControlMoveActionInfo& other);

	/// This function is used as a virtual copy constructor
	virtual AMControlMoveActionInfo* createCopy() const { return new AMControlMoveActionInfo(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Control Move"; }

	// New public functions
	//////////////////////////

	/// Returns a pointer to our move destination setpoint
	const AMControlInfo* controlInfo() const { return &controlInfo_; }

	/// Set the move destination setpoint. (We make a copy of \c controlInfo's values, but don't take ownership of it.)
	void setControlInfo(const AMControlInfo& controlInfo) { controlInfo_.setValuesFrom(controlInfo); }

	// Database loading/storing
	////////////////////////////

	/// For database storing only.
	AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
	/// For database loading only. This function will never be called since dbReadControlInfo() always returns a valid setpoint, but it needs to be here.
	void dbLoadControlInfo(AMDbObject* newLoadedObject) { delete newLoadedObject; }

signals:

public slots:

protected:
	/// The AMControlInfo that specifies where to move to
	AMControlInfo controlInfo_;

};

#endif // AMCONTROLMOVEACTIONINFO_H
