#ifndef AMWAITACTIONINFO_H
#define AMWAITACTIONINFO_H

#include "actions2/AMActionInfo.h"

/// This class specifies the information for an AMWaitAction. (In this case, the information is only the number of seconds to wait.)  It provides a simple example of a how to subclass AMActionInfo.
class AMWaitActionInfo : public AMActionInfo
{
    Q_OBJECT
	Q_PROPERTY(double secondsToWait READ secondsToWait WRITE setSecondsToWait)

public:
	/// Constructor. Specify the number of \c seconds you want to wait for
	Q_INVOKABLE AMWaitActionInfo(double seconds = 60, QObject *parent = 0);

	/// Copy Constructor
	AMWaitActionInfo(const AMWaitActionInfo& other) : AMActionInfo(other), seconds_(other.seconds_) {}

	/// This function is used as a virtual copy constructor
	virtual AMWaitActionInfo* createCopy() const { return new AMWaitActionInfo(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Wait (Delay)"; }

	// New public functions
	//////////////////////////

	/// The total number of seconds that we're supposed to wait for
	double secondsToWait() const { return seconds_; }

	/// Set the total number of seconds to wait
	void setSecondsToWait(double seconds) { seconds_ = seconds; setModified(true); }

signals:

public slots:

protected:
	/// The number of seconds to wait for
	double seconds_;

};

#endif // AMWAITACTIONINFO_H
