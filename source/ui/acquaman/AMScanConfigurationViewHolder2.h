#ifndef AMSCANCONFIGURATIONVIEWHOLDER2_H
#define AMSCANCONFIGURATIONVIEWHOLDER2_H

#include "ui/actions2/AMActionRunnerAddActionBar.h"

class AMScanConfiguration;
class AMScanConfigurationView;

/// This widget holds a scan configuration widget, and provides user controls to start the configured scan within the workflow.  It can be constructed to surround any AMScanConfigurationView. Below the widget, it adds buttons to start the configured scan or add it to the workflow queue (using AMActionRunner).
/*! This class is an updated version of AMScanConfigurationViewHolder that uses the workflow system in actions2/AMActionRunner.

This widget takes ownership of the view specifified in its constructor.

When the "Start Scan" button or "Add to Workflow" button is pressed, a copy of the view's current scan configuration is made, and an AMScanControllerAction (based on this config) is what gets added to the queue.  If you want to specify a different kind of action, you can overload the protected createAction() function.  (In those cases, this class is so trivial that you might just want to inherit AMActionRunnerAddActionBar directly... See REIXSScanConfigurationViewHolder for an example.)
*/
class AMScanConfigurationViewHolder2 : public AMActionRunnerAddActionBar
{
	Q_OBJECT
public:
	/// Construct a holder for an AMScanConfigurationView \c view (or 0 if no view is ready yet).
	/*! If provided, this widget takes ownership of the \c view */
	AMScanConfigurationViewHolder2(AMScanConfigurationView* view = 0, QWidget *parent = 0);

	/// Set the \c view contained within the holder.  If there is an existing view, the old view is deleted.
	/*! You can pass in 0 to remove the existing view from the holder.*/
	void setView(AMScanConfigurationView* view);

protected:

	/// This function is used to create the action that will be added to the queue. By default, it creates an AMScanControllerAction using a copy of the view's scan configuration. You can re-implement this to provide a specific kind of action (for ex: REIXSXESScanControllerAction, etc.) for use with the action registry system.
	virtual AMAction* createAction();

	/// This is the scan configuration widget we're wrapping
	AMScanConfigurationView* view_;
};

#endif // AMSCANCONFIGURATIONVIEWHOLDER2_H
