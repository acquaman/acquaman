#ifndef REIXSMOVETOSAMPLETRANSFERPOSITIONACTION_H
#define REIXSMOVETOSAMPLETRANSFERPOSITIONACTION_H

#include "actions2/AMListAction.h"
#include "actions2/actions/REIXS/REIXSMoveToSampleTransferPositionActionInfo.h"

/// This action tries to safely move the REIXS sample manipulator to the sample transfer position.
class REIXSMoveToSampleTransferPositionAction : public AMListAction
{
    Q_OBJECT
public:
	/// Constructor, requires a REIXSMoveToSampleTransferPositionActionInfo
	Q_INVOKABLE REIXSMoveToSampleTransferPositionAction(REIXSMoveToSampleTransferPositionActionInfo* info = new REIXSMoveToSampleTransferPositionActionInfo(), QObject *parent = 0);
	/// Copy constructor
	REIXSMoveToSampleTransferPositionAction(const REIXSMoveToSampleTransferPositionAction& other) : AMListAction(other) {}
	/// Virtual copy constructor
	virtual AMAction* createCopy() const { return new REIXSMoveToSampleTransferPositionAction(*this); }



protected:
	/// Re-implemented from AMListAction to add a set of move actions
	virtual void startImplementation();

};

#endif // REIXSMOVETOSAMPLETRANSFERPOSITIONACTION_H
