#ifndef AMTIMEOUTLOOPACTIONINFO_H
#define AMTIMEOUTLOOPACTIONINFO_H

#include "actions3/AMLoopActionInfo3.h"

/// This class extends the standard loop action to have a timeout.  This means that even if the actions complete before the timeout, then the loop will still wait until the timeout before starting the next iteration.
class AMTimeoutLoopActionInfo : public AMLoopActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(double timeoutTime READ timeoutTime WRITE setTimeoutTime NOTIFY timeoutTimeChanged)

public:
	/// Constructor.  Specify the \code loopCount and \code timeout.
	AMTimeoutLoopActionInfo(int iterations = 3, double timeout = 10.0, const QString &shortDescription = "Timed Loop", const QString &longDescription = "Timed loop", const QString& iconFileName = ":/32x32/media-playlist-repeat.png", QObject *parent = 0);
	/// Destructor.
	virtual ~AMTimeoutLoopActionInfo();

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Loop (Repeat)"; }

	/// Returns the timeout time.
	double timeoutTime() const { return timeoutTime_; }
	/// Sets the timeout time.
	void setTimeoutTime(double newTime);

signals:
	/// Notifier that the timeout time has changed.
	void timeoutTimeChanged(double);

protected:
	/// The timeout time.
	double timeoutTime_;
};

#endif // AMTIMEOUTLOOPACTIONINFO_H
