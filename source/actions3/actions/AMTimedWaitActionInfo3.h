#ifndef AMTIMEDWAITACTIONINFO3_H
#define AMTIMEDWAITACTIONINFO3_H

#include "actions3/AMActionInfo3.h"

class AMTimedWaitActionInfo3 : public AMActionInfo3
{
Q_OBJECT

Q_PROPERTY(double waitTime READ waitTime WRITE setWaitTime)
public:
	/// Constructor. You should always specify a valid \c setpoint, but we provide the default argument because we need a default constructor for database loading.
 	virtual ~AMTimedWaitActionInfo3();
	Q_INVOKABLE AMTimedWaitActionInfo3(double waitTime = 1.000, QObject *parent = 0);

	/// Copy Constructor
	AMTimedWaitActionInfo3(const AMTimedWaitActionInfo3 &other);

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const { return new AMTimedWaitActionInfo3(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Timed Wait"; }

	// New public functions
	//////////////////////////

	/// Returns the wait time in seconds
	double waitTime() const { return waitTime_; }

	/// Sets the wait time in seconds
	void setWaitTime(double waitTime);

signals:

public slots:

protected:
	/// The wait time in seconds
	double waitTime_;

	/// A short helper function to update the action's description
	void updateDescriptionText();
};

#endif // AMTIMEDWAITACTIONINFO3_H
