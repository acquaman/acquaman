#ifndef AMWAITACTIONINFO_H
#define AMWAITACTIONINFO_H

#include "actions3/AMActionInfo3.h"

/// This class is the info for a simple timed wait action.  The only input is a time and it will run for that amount of time.
class AMWaitActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(double waitTime READ waitTime WRITE setWaitTime)

public:
	/// Constructor.
	Q_INVOKABLE AMWaitActionInfo(double waitTime = 0.0, QObject *parent = 0);
	/// Copy Constructor.
	AMWaitActionInfo(const AMWaitActionInfo &other);
	/// Destructor.
	virtual ~AMWaitActionInfo(){}

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const;

	/// This should describe the type of the action.
	virtual QString typeDescription() const { return QString("Timed Wait of %1 seconds").arg(waitTime_); }
	/// Returns the time this action will wait for.
	double waitTime() const { return waitTime_; }

signals:
	/// Notifier that the wait time has changed.
	void waitTimeChanged(double);

public slots:
	/// Sets the wait time.
	void setWaitTime(double newTime);

protected:
	/// The member that holds the wait time.
	double waitTime_;
};

#endif // AMWAITACTIONINFO_H
