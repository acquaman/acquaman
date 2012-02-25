#ifndef REIXSACTIONBASEDCONTROLEDITOR_H
#define REIXSACTIONBASEDCONTROLEDITOR_H

#include "ui/beamline/AMControlEditor.h"

/// Re-implements AMBasicControlEditor to do its moves inside AMActionRunner with a REIXSControlMoveAction.
class REIXSActionBasedControlEditor : public AMBasicControlEditor
{
    Q_OBJECT
public:
	/// Construct an editor for \c control. If \c okToRunInBackground is true, the actions will be run regardless of what's going on in the AMActionRunner queue with AMActionRunner::runActionImmediately. Otherwise, they will only be run when no other actions are currently running. (This is best for things that could mess up scans.)
	REIXSActionBasedControlEditor(AMControl* control, bool okToRunInBackground = false, QWidget *parent = 0);


protected slots:
	/// Re-implemented from AMBasicControlEditor
	virtual void onNewSetpointChosen(double value);

protected:
	bool okToRunInBackground_;

};

#endif // REIXSACTIONBASEDCONTROLEDITOR_H
