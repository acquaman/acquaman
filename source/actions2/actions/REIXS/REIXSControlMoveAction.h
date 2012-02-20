#ifndef REIXSCONTROLMOVEACTION_H
#define REIXSCONTROLMOVEACTION_H

#include "actions2/actions/AMControlMoveAction.h"
#include "actions2/actions/REIXS/REIXSControlMoveActionInfo.h"

/// This subclass of AMControlMoveAction provides the required lookup of controls on the REIXS beamline at the CLS.
class REIXSControlMoveAction : public AMControlMoveAction
{
    Q_OBJECT
public:
	// Constructors and copying
	//////////////////////////////

	/// Constructor. Requires and takes ownership of an existing AMControlMoveActionInfo \c info.
	Q_INVOKABLE REIXSControlMoveAction(REIXSControlMoveActionInfo* info, QObject *parent = 0) : AMControlMoveAction(info, parent) {}
	/// Copy constructor: must re-implement, but can simply use the base class copy constructor.
	REIXSControlMoveAction(const REIXSControlMoveAction& other) : AMControlMoveAction(other) {}
	/// Virtual copy constructor
	virtual REIXSControlMoveAction* createCopy() const { return new REIXSControlMoveAction(*this); }

	// Re-implemented virtual functions
	/////////////////////////

	/// Re-implemented to return a control from the REIXS beamline.
	AMControl* getControlForAction(const AMControlInfo &controlInfo) const;

};

#endif // REIXSCONTROLMOVEACTION_H
