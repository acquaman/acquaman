#ifndef AMCHANGERUNACTION_H
#define AMCHANGERUNACTION_H

#include "actions2/AMAction.h"
#include "actions2/actions/AMChangeRunActionInfo.h"

/// A workflow action to change the current run (to a new or existing run)
class AMChangeRunAction : public AMAction
{
    Q_OBJECT
public:
	/// Constructor: requires AMChangeRunActionInfo \c info.
	Q_INVOKABLE AMChangeRunAction(AMChangeRunActionInfo* info, QObject *parent = 0) : AMAction(info, parent) {}
	/// Copy constructor:
	AMChangeRunAction(const AMChangeRunAction& other) : AMAction(other) {}
	/// Virtual copy constructor:
	virtual AMAction* createCopy() const { return new AMChangeRunAction(*this); }

protected:
	// Re-implemented functions:
	/////////////////////////////

	/// Re-implemented to change the current run and report success.
	virtual void startImplementation();

};

#endif // AMCHANGERUNACTION_H
