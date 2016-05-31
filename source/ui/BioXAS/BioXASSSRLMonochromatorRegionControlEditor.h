#ifndef BIOXASMAINMONOCHROMATORREGIONCONTROLVIEW_H
#define BIOXASMAINMONOCHROMATORREGIONCONTROLVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QProgressBar>

#include "ui/CLS/CLSControlEditor.h"

class BioXASSSRLMonochromatorRegionControl;

class BioXASSSRLMonochromatorRegionControlEditor : public CLSControlEditor
{
	Q_OBJECT
public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorRegionControlEditor(BioXASSSRLMonochromatorRegionControl *regionControl, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorRegionControlEditor();

public slots:
	/// Sets the control. Calls AMExtendedControlEditor::setControl(), and then makes an additional connect statement.
	virtual void setControl(AMControl *newControl);

protected slots:
	/// Shows the moving view, called when the region control indicates that a move has started.
	void onRegionControlMoveStarted();
};

class BioXASSSRLMonochromatorRegionControlMovingView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorRegionControlMovingView(BioXASSSRLMonochromatorRegionControl *regionControl, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorRegionControlMovingView();
	/// Returns the region control being viewed.
	BioXASSSRLMonochromatorRegionControl* regionControl() const { return regionControl_; }

signals:
	/// Notifier that the region control being viewed has changed.
	void regionControlChanged(AMControl *newControl);

public slots:
	/// Sets the region control being viewed.
	void setRegionControl(BioXASSSRLMonochromatorRegionControl *newControl);

protected slots:
	/// Handles updating the view when a region move has started.
	void onMoveStarted();
	/// Handles updating the view when a region move has failed.
	void onMoveFailed(int failureCode);
	/// Handles updating the view when a region move has succeeded.
	void onMoveSucceeded();
	/// Handles updating the progress bar when the region control reports move progress has been made.
	void onMoveProgressChanged(double numerator, double denominator);
	/// Handles updating the moving view description and instruction when the region control reports the current step in the move has changed.
	void onMoveStepChanged(const QString &newDescription, const QString &newInstruction, const QString &newNotes);

protected:
	/// The region control being viewed.
	BioXASSSRLMonochromatorRegionControl *regionControl_;
	/// The view displayed when the region control is moving as a result of this software.
	QWidget *movingView_;
	/// The progress bar in the moving view, displays the progress made through a region change.
	QProgressBar *movingProgress_;
	/// The label in the moving view that describes the current step being performed.
	QLabel *movingDescription_;
	/// The label in the moving view that informs the user of an action they should take.
	QLabel *movingInstruction_;
	/// The label in the moving view that presents general notes related to the current step.
	QLabel *movingNotes_;
	/// The view displayed when the region control reports a move has failed.
	QWidget *failedView_;
	/// The label in the failed view that provides the failure explaination.
	QLabel *failedDescription_;
	/// The view displayed when the region control reports a move has succeeded.
	QWidget *succeededView_;

private:
	/// Shows the moving view, hides the other views.
	void showMovingView();
	/// Shows the failed view, hides the other views.
	void showFailedView(const QString &message);
	/// Shows the succeeded view, hides the other views.
	void showSucceededView();
};

#endif // BIOXASMAINMONOCHROMATORREGIONCONTROLVIEW_H
