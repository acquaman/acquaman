#ifndef AMBEAMLINE2DSCANACTION_H
#define AMBEAMLINE2DSCANACTION_H

#include "actions/AMBeamlineScanAction.h"
#include <QPushButton>
/*!
	This is a relatively simple extension of AMBeamlineScanAction that allows the scan to "Stop at the end of the line".
	Since this requires adding a new option to the possible during scan actions, it has its own special view with an extra button.
*/
class AMBeamline2DScanAction : public AMBeamlineScanAction
{
	Q_OBJECT

public:
	/// Constructor.
	AMBeamline2DScanAction(AMScanConfiguration *cfg, QObject *parent = 0);

	/// Returns a new instance of the view that represents this action.
	virtual AMBeamlineActionItemView *createView(int index = 0);

public slots:
	/// Tells the scan controller to stop at the end of the line it is currently scanning.
	virtual void stopScanAtEndOfLine();

protected slots:
	/// Reimplemented.  If stop at the end of the line has been chosen, it sets the description to match the real state rather than going back to [Running].
	virtual void onScanResumed();
};

/// This class adds a button that enables the scan to be stopped at the end of the line.
class AMBeamline2DScanActionView : public AMBeamlineScanActionView
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for the 2D scan action and adds another button.
	AMBeamline2DScanActionView(AMBeamline2DScanAction *scanAction, int index = 0, QWidget *parent = 0);

protected slots:
	/// Handles changing the view and passing on the signal that the scan should stop at the end of the line.
	virtual void onStopScanAtEndOfLineButtonClicked();
	/// Reimplemented.  Makes the look and feel of the stop at end of line button the same as the the other buttons.  Calls AMBeamlineScanAction::onScanStarted() at the end.
	virtual void onScanStarted();

protected:
	/// Button that stops the scan at the end of the line.
	QPushButton *stopAtEndOfLineButton_;
};

#endif // AMBEAMLINE2DSCANACTION_H
