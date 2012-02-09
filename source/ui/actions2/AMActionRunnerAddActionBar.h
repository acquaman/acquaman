#ifndef AMACTIONRUNNERADDACTIONBAR_H
#define AMACTIONRUNNERADDACTIONBAR_H

#include <QWidget>

class AMAction;
class QPushButton;
class QLabel;
class QRadioButton;
class QVBoxLayout;

/// This widget provides a common, standard set of buttons to show below a typical "action configuration" panel. The buttons let the user run the action immediately (if AMActionRunner isn't already running an action), or add it to the action runner's queue. The buttons are automatically enabled/disabled based on the state of the action runner.
/*! To use this class, you need to re-implement the virtual protected function createAction() to provide the action that should be run/added to the queue.

  This widget leaves room in its layout for any kind action configuration widget to be added directly above it... Call addWidget() to insert that widget.*/

class AMActionRunnerAddActionBar : public QWidget
{
    Q_OBJECT
public:
	/// Constructor.  If the main "Start ____" should say something other than "Start Action", you can specify that name here.  (ex: "Scan", "Move", etc.)
	explicit AMActionRunnerAddActionBar(const QString& actionCategoryName = "Action", QWidget *parent = 0);

	/// Adds a widget to our layout, directly above the buttons.  We take ownership of that \c widget.
	void addWidget(QWidget* widget);

signals:
	/// Emitted to request the main application controller show the workflow pane.
	void showWorkflowRequested();

protected slots:
	/// Called when the "Start Scan" button is clicked
	void onStartActionRequested();
	/// Called when the "Add to Queue" button is clicked.
	void onAddToQueueRequested();

	/// This re-evaluates the state of the workflow and provides user feedback on the "Start [action]" button as required.
	void reviewStartActionButtonState();

protected:
	/// This function is called to get the action that should be added to the queue. The base class implementation returns 0 for no valid action, so it must be re-implemented.
	virtual AMAction* createAction() { return 0; }

	/// UI elements
	QLabel *whenDoneLabel_;
	QPushButton* startActionButton_, *addToQueueButton_;
	QRadioButton* goToWorkflowOption_, *setupAnotherActionOption_;
	QVBoxLayout* layout_;
	QString actionCategoryName_;

};

#endif // AMACTIONRUNNERADDACTIONBAR_H
