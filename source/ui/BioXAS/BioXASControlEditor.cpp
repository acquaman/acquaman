#include "BioXASControlEditor.h"
#include "beamline/AMControl.h"

BioXASControlEditor::BioXASControlEditor(AMControl *control, QWidget *parent) :
    QGroupBox(parent)
{
	// Initialize class variables.

	control_ = 0;

	moveAction_ = new QAction("Move", this);
	connect( moveAction_, SIGNAL(triggered()), this, SLOT(onMoveActionTriggered()) );

	stopAction_ = new QAction("Stop", this);
	connect( stopAction_, SIGNAL(triggered()), this, SLOT(onStopActionTriggered()) );

	calibrateAction_ = new QAction("Calibrate", this);
	connect( calibrateAction_, SIGNAL(triggered()), this, SLOT(onCalibrateActionTriggered()) );

	// Create UI elements.

	valueLabel_ = new QLabel();
	valueLabel_->setAlignment(Qt::AlignCenter);
	valueLabel_->setAttribute(Qt::WA_TranslucentBackground);

	moveProgressBar_ = new QProgressBar();
//	moveProgressBar_->setStyleSheet("border-radius: 5px; background-color: rgb(255, 255, 255);");
	moveProgressBar_->setTextVisible(false);

	QHBoxLayout *overlay = new QHBoxLayout(valueLabel_);
	overlay->addWidget(moveProgressBar_);

	moveProgressBar_->stackUnder(valueLabel_);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(valueLabel_);

	setLayout(layout);

	// Setup context menu.

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)) );

	// Current settings.

	setControl(control);

	refresh();
}

BioXASControlEditor::~BioXASControlEditor()
{

}

void BioXASControlEditor::refresh()
{
	setTitle("");

	if (control_)
		setTitle(control_->name());

	updateValueLabel();

	updateMoveAction();
	updateStopAction();
	updateCalibrateAction();
}

void BioXASControlEditor::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateValueLabel()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASControlEditor::updateValueLabel()
{
	QString newValue = "[Invalid]";

	if (control_) {

		newValue = "[Not measurable]";

		if (control_->canMeasure()) {
			newValue = QString("%1").arg(control_->value());

			QString units = control_->units();
			if (!units.isEmpty())
				newValue.append(QString(" %1").arg(units));
		}
	}

	valueLabel_->setText(newValue);
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

void BioXASControlEditor::onContextMenuRequested(const QPoint &clickPosition)
{
	if (control_) {

		QMenu contextMenu;

		// Add action options to the menu.

		contextMenu.addAction(moveAction_);
		contextMenu.addAction(stopAction_);
		contextMenu.addAction(calibrateAction_);

		// Show the menu.

		contextMenu.exec(mapToGlobal(clickPosition));
	}
}
