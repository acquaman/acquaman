#include "BioXASControlEditor.h"
#include "beamline/AMControl.h"

BioXASControlEditor::BioXASControlEditor(AMControl *control, QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize class variables.

	control_ = 0;

	useControlNameAsTitle_ = true;
	format_ = 'g';
	precision_ = 3;
	useControlUnitsAsUnits_ = true;
	readOnly_ = false;

	moveAction_ = new QAction("Move", this);
	moveAction_->setEnabled(false);

	stopAction_ = new QAction("Stop", this);
	stopAction_->setEnabled(false);

	calibrateAction_ = new QAction("Calibrate", this);
	calibrateAction_->setEnabled(false);

	// Create UI elements.

	valueLabel_ = new QLabel();
	valueLabel_->setAlignment(Qt::AlignCenter);
	valueLabel_->setStyleSheet("color: rgb(0, 0, 0); background-color: rgb(255, 255, 255);");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(valueLabel_);

	setLayout(layout);

	// Make connections.

	connect( moveAction_, SIGNAL(triggered()), this, SLOT(onMoveActionTriggered()) );
	connect( stopAction_, SIGNAL(triggered()), this, SLOT(onStopActionTriggered()) );
	connect( calibrateAction_, SIGNAL(triggered()), this, SLOT(onCalibrateActionTriggered()) );

	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)) );

	// Current settings.

	setControl(control);
	setContextMenuPolicy(Qt::CustomContextMenu);
}

BioXASControlEditor::~BioXASControlEditor()
{

}

void BioXASControlEditor::refresh()
{
	updateTitle();
	updateValueLabel();
}

void BioXASControlEditor::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateValueLabel()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(updateValueLabel()) );
			connect( control_, SIGNAL(unitsChanged(QString)), this, SLOT(updateValueLabel()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASControlEditor::setTitle(const QString &title)
{
	if (title_ != title) {
		title_ = title;
		setUseControlNameAsTitle(false);
		updateTitle();
	}
}

void BioXASControlEditor::setUseControlNameAsTitle(bool useName)
{
	if (useControlNameAsTitle_ != useName) {
		useControlNameAsTitle_ = useName;
		updateTitle();

		emit useControlNameAsTitleChanged(useName);
	}
}

void BioXASControlEditor::setFormat(const QChar &newFormat)
{
	if (format_ != newFormat) {
		format_ = newFormat;
		updateValueLabel();

		emit formatChanged(format_);
	}
}

void BioXASControlEditor::setPrecision(int newPrecision)
{
	if (precision_ != newPrecision) {
		precision_ = newPrecision;
		updateValueLabel();

		emit precisionChanged(precision_);
	}
}

void BioXASControlEditor::setUnits(const QString &newUnits)
{
	if (units_ != newUnits) {
		units_ = newUnits;

		setUseControlUnitsAsUnits(false);
		updateValueLabel();

		emit unitsChanged(units_);
	}
}

void BioXASControlEditor::setUseControlUnitsAsUnits(bool useUnits)
{
	if (useControlUnitsAsUnits_ != useUnits) {
		useControlUnitsAsUnits_ = useUnits;
		updateValueLabel();

		emit useControlUnitsAsUnitsChanged(useControlUnitsAsUnits_);
	}
}

void BioXASControlEditor::setReadOnly(bool readOnly)
{
	if (readOnly_ != readOnly) {
		readOnly_ = readOnly;

		emit readOnlyChanged(readOnly_);
	}
}

void BioXASControlEditor::updateTitle()
{
	QString title;

	if (control_ && useControlNameAsTitle_)
		title = control_->name();
	else
		title = title_;

	setTitleText(title);
}

void BioXASControlEditor::updateValueLabel()
{
	QString text = "[Invalid]";

	if (control_) {
		text = "[Not measurable]";

		if (control_ && control_->canMeasure()) {
			text = QString::number(control_->value(), format_.toAscii(), precision_);

			QString units;

			if (useControlUnitsAsUnits_)
				units = control_->units();
			else
				units = units_;

			if (!units.isEmpty())
				text.append(QString(" %1").arg(units));
		}
	}

	valueLabel_->setText(text);
}

void BioXASControlEditor::setTitleText(const QString &newText)
{
	QGroupBox::setTitle(newText);
	emit titleChanged(newText);
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
//	onEditStart();
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

void BioXASControlEditor::mouseReleaseEvent(QMouseEvent *event)
{

}
