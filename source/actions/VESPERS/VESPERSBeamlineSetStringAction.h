#ifndef VESPERSBEAMLINESETSTRINGACTION_H
#define VESPERSBEAMLINESETSTRINGACTION_H

#include "actions/AMBeamlineActionItem.h"
#include "beamline/AMProcessVariable.h"

#define VESPERSBEAMLINESETSTRINGACTION_SUCCEEDED 878000
#define VESPERSBEAMLINESETSTRINGACTION_FAILED 878001

/// This class sets a string to an AMProcessVariable.  It requires a very specific setString method that appears to only exist for a couple string based PVs on VESPERS.
class VESPERSBeamlineSetStringAction : public AMBeamlineActionItem
{
	Q_OBJECT

public:
	/// Constructor.  Builds the action - requires an AMProcessVariable and a QString.
	VESPERSBeamlineSetStringAction(AMProcessVariable *pv, const QString &string, QObject *parent = 0);

	/// Returns 0.  This action should not be used with the old workflow.
	virtual AMBeamlineActionItemView* createView(int index = 0) { Q_UNUSED(index); return 0; }

public slots:
	/// The start method.  Uses the specific method to set the string to the process variable.
	virtual void start();
	/// The clean up method.
	virtual void cleanup();

protected slots:
	/// Slot that handles the value changed signal after changing the value.
	void onValueChanged();

protected:
	/// Pointer to the process variable.
	AMProcessVariable *pv_;
	/// Holds the string we want to set.
	QString string_;
};

#endif // VESPERSBEAMLINESETSTRINGACTION_H
