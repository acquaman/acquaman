#ifndef REIXSSAMPLEMOVEACTION_H
#define REIXSSAMPLEMOVEACTION_H

#include "actions2/AMListAction.h"
#include "actions2/actions/REIXS/REIXSSampleMoveActionInfo.h"

/// This action moves the REIXS sample manipulator to a target position (predefined in a REIXSSampleMoveActionInfo.)
/*! It does the x, y, z, and theta moves in a specific sequence, so that if the sample arm is in a not-crashed position prior to the move, and the target position is also a not-crashed position, no crashes between the slit and sample will happen during the move.  However, there is no internal checking that the target position is safe -- that's up to you.*/
class REIXSSampleMoveAction : public AMListAction
{
    Q_OBJECT
public:
	// Constructors
	///////////////////

	/// Constructor: must take ownership of a REIXSSampleMoveActionInfo
	Q_INVOKABLE REIXSSampleMoveAction(REIXSSampleMoveActionInfo* info, QObject *parent = 0);
	/// This convenience constructor is equivalent to REIXSSampleMoveAction(new REIXSSampleMoveActionInfo(targetPositions).
	REIXSSampleMoveAction(const AMControlInfoList& targetPositions, QObject* parent = 0);
	/// Copy constructor. The base classes take care of making a copy of the info for us.
	REIXSSampleMoveAction(const REIXSSampleMoveAction& other) : AMListAction(other) {}
	/// Virtual copy constructor
	AMAction* createCopy() const { return new REIXSSampleMoveAction(*this); }



signals:

public slots:

protected:
	// Re-implemented virtual functions
	//////////////////

	/// Re-implemented from AMListAction to look at the current manipulator position and figure out the correct order to add AMControlMove sub-actions, to avoid crashing.
	virtual void startImplementation();


	// Helper functions
	//////////////////////

	/// Our info() function returns a generic AMActionInfo, so here we cast it as a specific REIXSSampleMoveActionInfo for convenience.
	REIXSSampleMoveActionInfo* sampleMoveInfo() { return qobject_cast<REIXSSampleMoveActionInfo*>(info()); }
	/// Our info() function returns a generic AMActionInfo, so here we cast it as a specific REIXSSampleMoveActionInfo for convenience.
	const REIXSSampleMoveActionInfo* sampleMoveInfo() const { return qobject_cast<const REIXSSampleMoveActionInfo*>(info()); }


};

#endif // REIXSSAMPLEMOVEACTION_H
