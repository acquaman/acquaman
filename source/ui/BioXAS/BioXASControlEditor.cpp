#include "BioXASControlEditor.h"
#include "dataman/AMNumber.h"
#include "beamline/AMControl.h"

BioXASControlEditor::BioXASControlEditor(AMControl *control, QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize class variables.

	control_ = 0;

	title_ = QString();
	useControlNameAsTitle_ = true;
	value_ = 0;
	useControlValueAsValue_ = true;
	minimumValue_ = DBL_MIN;
	useControlMinimumAsMinimum_ = true;
	maximumValue_ = DBL_MAX;
	useControlMaximumAsMaximum_ = true;
	format_ = 'g';
	precision_ = 3;
	useControlPrecisionAsPrecision_ = true;
	values_ = QStringList();
	useControlValuesAsValues_ = true;
	units_ = QString();
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

	spinBox_ = new QDoubleSpinBox();
	spinBox_->setAlignment(Qt::AlignCenter);

	comboBox_ = new QComboBox();

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(valueLabel_);
	layout->addWidget(spinBox_);
	layout->addWidget(comboBox_);

	setLayout(layout);

	// Make connections.

	connect( moveAction_, SIGNAL(triggered()), this, SLOT(onMoveActionTriggered()) );
	connect( stopAction_, SIGNAL(triggered()), this, SLOT(onStopActionTriggered()) );
	connect( calibrateAction_, SIGNAL(triggered()), this, SLOT(onCalibrateActionTriggered()) );

	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)) );

	// Current settings.

	setContextMenuPolicy(Qt::CustomContextMenu);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setControl(control);

	refresh();
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

		if (control_ && useControlNameAsTitle_)
			setTitleText(control_->name());

		if (control_ && useControlUnitsAsUnits_)
			setUnitsText(control_->units());

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASControlEditor::setTitle(const QString &title)
{
	if (title_ != title) {
		setUseControlNameAsTitle(false);
		setTitleText(title);
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
		setUseControlUnitsAsUnits(false);
		setUnitsText(newUnits);
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
	QGroupBox::setTitle(title_);
}

void BioXASControlEditor::updateValueLabel()
{
	// Update text.

	valueLabel_->setText( generateValueLabelText() );
}

void BioXASControlEditor::setTitleText(const QString &newText)
{
	if (title_ != newText) {
		title_ = newText;
		updateTitle();

		emit titleChanged(newText);
	}
}

void BioXASControlEditor::setUnitsText(const QString &newText)
{
	if (units_ != newText) {
		units_ = newText;
		updateValueLabel();

		emit unitsChanged(units_);
	}
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
	setViewMode(Edit);
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
		double newValue = QInputDialog::getDouble(this, QString("Calibrate %1").arg(control_->name()), "Enter calibrated value:", oldValue, DBL_MIN, DBL_MAX, precision_, &inputOK);

		if (inputOK)
			control_->calibrate(oldValue, newValue);

	} else {
		QApplication::beep();
	}
}

AMNumber BioXASControlEditor::getDoubleValue() const
{
	AMNumber result = new AMNumber(AMNumber::InvalidError);

	bool inputOK = false;
	double newValue = QInputDialog::getDouble(this, title_, QString("New value: "), value_, minimumValue_, maximumValue_, precision_, &inputOK);

	if (inputOK)
		result = AMNumber(newValue);

	return result;
}

AMNumber BioXASControlEditor::getEnumValue(QStringList values) const
{
	AMNumber result = new AMNumber(AMNumber::InvalidError);

	bool inputOK = false;
	QString newValueName = QInputDialog::getItem(this, title_, QString("New value: "), values, value_, true, &inputOK);

	if (inputOK)
		result = AMNumber(newValue);

	return result;
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

bool BioXASControlEditor::canSetViewModeToDisplay() const
{
	return true;
}

bool BioXASControlEditor::canSetViewModeToEdit() const
{
	bool result = false;

	if (control_ && control_->canMove() && !readOnly_)
		result = true;

	return result;
}

void BioXASControlEditor::setupSpinBox()
{
	if (control_) {
		spinBox_->setValue(control_->value());
		spinBox_->setMinimum(control_->minimumValue());
		spinBox_->setMaximum(control_->maximumValue());
	}
}

void BioXASControlEditor::setupComboBox()
{
	if (control_ && control_->isEnum()) {
		comboBox_->addItems(control_->moveEnumNames());
		comboBox_->setCurrentIndex(int(control_->value()));

		// Center displayed entry.

		comboBox_->setEditable(true);
		comboBox_->lineEdit()->setReadOnly(true);
		comboBox_->lineEdit()->setAlignment(Qt::AlignCenter);

		for (int i = 0; i < comboBox_->count(); i++)
			comboBox_->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}
}

QString BioXASControlEditor::generateValueLabelText() const
{
	QString text = "[Invalid]";

	if (control_) {
		text = "[Not measurable]";

		if (control_ && control_->canMeasure()) {
			text = QString::number(control_->value(), format_.toAscii(), precision_);

			if (!units_.isEmpty())
				text.append(QString(" %1").arg(units_));
		}
	}

	return text;
}

bool BioXASControlEditor::generateValueLabelVisibleStatus() const
{
	bool result = false;

	if (viewMode_ == Display && !underMouse())
		result = true;

	return result;
}

void BioXASControlEditor::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
	setViewMode(Edit);
}
