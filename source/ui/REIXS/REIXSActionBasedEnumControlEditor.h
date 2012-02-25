#ifndef REIXSACTIONBASEDENUMCONTROLEDITOR_H
#define REIXSACTIONBASEDENUMCONTROLEDITOR_H

#include "ui/beamline/AMBasicEnumControlEditor.h"

/// This widget re-implements AMBasicEnumControlEditor to do its moves inside AMActionRunner with a REIXSControlMoveAction.
class REIXSActionBasedEnumControlEditor : public AMBasicEnumControlEditor
{
    Q_OBJECT
public:
	/// Construct an editor for \c control. If \c okToRunInBackground is true, the actions will be run regardless of what's going on in the AMActionRunner queue with AMActionRunner::runActionImmediately. Otherwise, they will only be run when no other actions are currently running. (This is best for things that could mess up scans.)
	REIXSActionBasedEnumControlEditor(AMControl* control, bool okToRunInBackground = false, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Re-implemented to let us do the move using an action.
	void onNewEnumValueChosen(int value);


protected:
	bool okToRunInBackground_;
};

#endif // REIXSACTIONBASEDENUMCONTROLEDITOR_H
