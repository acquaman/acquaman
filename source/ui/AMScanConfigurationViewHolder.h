#ifndef AMSCANCONFIGURATIONVIEWHOLDER_H
#define AMSCANCONFIGURATIONVIEWHOLDER_H

#include <QWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class AMScanConfiguration;
class AMScanConfigurationView;
class AMWorkflowManagerView;

/// This widget holds a scan configuration widget, and provides user controls to start the configured scan within the workflow.  It can be constructed to surround any AMScanConfigurationView. Below the widget, it adds buttons to start the configured scan or add it to the queue.
/*! This widget takes ownership of the view specifified in its constructor.

When the "Start Scan" button or "Add to Workflow" button is pressed, a copy of the scan configuration view's current scan configuration is made, and this is what gets added to the queue.
*/
class AMScanConfigurationViewHolder : public QWidget
{
	Q_OBJECT
public:
	/// Construct a holder for an AMScanConfigurationView \c view (or 0 if no view is ready yet).  A pointer to the workflow manager \c workflow must be provided, which we use to add scan actions when required.
	/*! If provided, this widget takes ownership of the \c view */
	AMScanConfigurationViewHolder(AMWorkflowManagerView* workflow, AMScanConfigurationView* view = 0, QWidget *parent = 0);
	/// Destructor.
	~AMScanConfigurationViewHolder();

	/// Set the \c view contained within the holder.  If there is an existing view, the old view is deleted.
	/*! You can pass in 0 to remove the existing view from the holder.*/
	void setView(AMScanConfigurationView* view);

public slots:

signals:

	/// Emitted to request the main application controller show the workflow pane.
	void showWorkflowRequested();

protected slots:

	/// Called when the "Start Scan" button is clicked
	void onStartScanRequested();
	/// Called when the "Add to Queue" button is clicked.
	void onAddToQueueRequested();

	/// This re-evaluates the state of the workflow and provides user feedback on the "Start Scan" button as required.
	/*! If the scan configuration view or scan configuration are invalid, sets the button disabled with a message. If the workflow cannot be started because the beamline is busy (ie: one of your scans, or someone else's scan, is running), sets the button disabled with a message. Otherwise enables the button. */
	void reviewStartScanButtonState();

protected:

	/// UI elements
	QLabel *whenDoneLabel_;
	QPushButton* startScanButton_, *addToQueueButton_;
	QRadioButton* goToWorkflowOption_, *setupAnotherScanOption_;
	QVBoxLayout* layout_;

	/// This is the scan configuration widget we're wrapping
	AMScanConfigurationView* view_;
	/// This is a pointer to the workflow manager (view, for now). We use it to add scan actions to the workflow
	AMWorkflowManagerView* workflow_;

};

#endif // AMSCANCONFIGURATIONVIEWHOLDER_H
