#ifndef AMTIMEOUTACTION_H
#define AMTIMEOUTACTION_H

#include "actions3/actions/AMWaitAction.h"

class AMTimeoutAction : public AMWaitAction
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMTimeoutAction(AMWaitActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	AMTimeoutAction(const AMTimeoutAction &original);
	/// Destructor.
	virtual ~AMTimeoutAction();

	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const { return new AMTimeoutAction(*this); }

public slots:
	/// Notifies this action that it should succeed.
	void proceed();

protected slots:
	/// Handles setting this action to failed when the timed wait action times out.
	virtual void onWaitTimerTimeout();
};

#endif // AMTIMEOUTACTION_H
