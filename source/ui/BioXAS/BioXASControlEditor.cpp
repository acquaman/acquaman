#include "BioXASControlEditor.h"
#include "beamline/AMControl.h"

BioXASControlEditor::BioXASControlEditor(AMControl *control, QWidget *parent) :
	AMExtendedControlEditor(control, 0, false, false, parent)
{
	// Initialize class variables.

	moveAction_ = new QAction("Move", this);
	moveAction_->setEnabled(false);

	connect( moveAction_, SIGNAL(triggered()), this, SLOT(onMoveActionTriggered()) );

	stopAction_ = new QAction("Stop", this);
	stopAction_->setEnabled(false);

	connect( stopAction_, SIGNAL(triggered()), this, SLOT(onStopActionTriggered()) );

	calibrateAction_ = new QAction("Calibrate", this);
	calibrateAction_->setEnabled(false);

	connect( calibrateAction_, SIGNAL(triggered()), this, SLOT(onCalibrateActionTriggered()) );

	// Current settings.

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)) );
}

BioXASControlEditor::~BioXASControlEditor()
{

}

void BioXASControlEditor::updateActions()
{
	updateMoveAction();
	updateStopAction();
	updateCalibrateAction();
}

void BioXASControlEditor::updateMoveAction()
{
	bool enabled = false;

	if (control_ && control_->canMove())
		enabled = true;

	moveAction_->setEnabled(enabled);
}

void BioXASControlEditor::updateStopAction()
{
	bool enabled = false;

	if (control_ && control_->canStop())
		enabled = true;

	stopAction_->setEnabled(enabled);
}

void BioXASControlEditor::updateCalibrateAction()
{
	bool enabled = false;

	if (control_ && control_->canCalibrate())
		enabled = true;

	calibrateAction_->setEnabled(enabled);
}

void BioXASControlEditor::onMoveActionTriggered()
{
	onEditStart();
}

void BioXASControlEditor::onStopActionTriggered()
{
	if (control_ && control_->canStop())
		control_->stop();
	else
		QApplication::beep();
}

void BioXASControlEditor::onCalibrateActionTriggered()
{
	if (control_ && control_->canCalibrate()) {
		bool inputOK = false;
		double oldValue = control_->value();
		double newValue = QInputDialog::getDouble(this, QString("Calibrate %1").arg(control_->name()), "Enter calibrated value:", oldValue, DBL_MIN, DBL_MAX, 3, &inputOK);

		if (inputOK)
			control_->calibrate(oldValue, newValue);

	} else {
		QApplication::beep();
	}
}

void BioXASControlEditor::onContextMenuRequested(const QPoint &clickPosition)
{
	// Update the actions to reflect current control settings.

	updateActions();

	// Create context menu.

	QMenu contextMenu;

	contextMenu.addAction(moveAction_);
	contextMenu.addAction(stopAction_);
	contextMenu.addAction(calibrateAction_);

	// Show menu.

	contextMenu.exec(mapToGlobal(clickPosition));
}


