#include "BioXASControlEditor.h"
#include "beamline/AMControl.h"

BioXASControlEditor::BioXASControlEditor(AMControl *control, QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize class variables.

	control_ = 0;

	title_ = QString();
	useControlNameAsTitle_ = true;
	value_ = AMNumber(AMNumber::InvalidError);
	valueText_ = "[Invalid]";
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

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(valueLabel_);

	setLayout(layout);

	// Make connections.

	connect( moveAction_, SIGNAL(triggered()), this, SLOT(onMoveActionTriggered()) );
	connect( stopAction_, SIGNAL(triggered()), this, SLOT(onStopActionTriggered()) );
	connect( calibrateAction_, SIGNAL(triggered()), this, SLOT(onCalibrateActionTriggered()) );

	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)) );

	// Current settings.

	setContextMenuPolicy(Qt::CustomContextMenu);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
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

void BioXASControlEditor::setValue(double newValue)
{
	if (value_ != AMNumber(newValue)) {
		setUseControlValueAsValue(false);
		setValueDouble(newValue);
	}
}

void BioXASControlEditor::setUseControlValueAsValue(bool useValue)
{
	if (useControlValueAsValue_ != useValue) {
		useControlValueAsValue_ = useValue;
		updateValueLabel();

		emit useControlValueAsValueChanged(useControlValueAsValue_);
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
		setUseControlPrecisionAsPrecision(false);
		setPrecisionInt(newPrecision);

		emit precisionChanged(precision_);
	}
}

void BioXASControlEditor::setUseControlPrecisionAsPrecision(bool usePrecision)
{
	if (useControlPrecisionAsPrecision_ != usePrecision) {
		useControlPrecisionAsPrecision_ = usePrecision;
		updateValueLabel();

		emit useControlPrecisionAsPrecisionChanged(useControlPrecisionAsPrecision_);
	}
}

void BioXASControlEditor::setMinimumValue(double minimumValue)
{
	if (minimumValue_ != minimumValue) {
		setUseControlMinimumAsMinimum(false);
		setMinimumValueDouble(minimumValue);
	}
}

void BioXASControlEditor::setUseControlMinimumAsMinimum(bool useMinimum)
{
	if (useControlMinimumAsMinimum_ != useMinimum) {
		useControlMinimumAsMinimum_ = useMinimum;

		emit useControlMinimumAsMinimumChanged(useControlMinimumAsMinimum_);
	}
}

void BioXASControlEditor::setMaximumValue(double maximumValue)
{
	if (maximumValue_ != maximumValue) {
		setUseControlMaximumAsMaximum(false);
		setMaximumValueDouble(maximumValue);
	}
}

void BioXASControlEditor::setUseControlMaximumAsMaximum(bool useMaximum)
{
	if (useControlMaximumAsMaximum_ != useMaximum) {
		useControlMaximumAsMaximum_ = useMaximum;

		emit useControlMaximumAsMaximumChanged(useControlMaximumAsMaximum_);
	}
}

void BioXASControlEditor::setValues(const QStringList &newValues)
{
	if (values_ != newValues) {
		setUseControlValuesAsValues(false);
		setValuesList(newValues);
	}
}

void BioXASControlEditor::setUseControlValuesAsValues(bool useValues)
{
	if (useControlValuesAsValues_ != useValues) {
		useControlValuesAsValues_ = useValues;

		emit useControlValuesAsValuesChanged(useControlValuesAsValues_);
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

void BioXASControlEditor::setTitleText(const QString &newText)
{
	if (title_ != newText) {
		title_ = newText;
		updateTitle();

		emit titleChanged(newText);
	}
}

void BioXASControlEditor::setValueDouble(double newValue)
{
	if (value_ != AMNumber(newValue)) {
		value_ = AMNumber(newValue);
		updateValueText();
		updateValueLabel();

		emit valueChanged(double(value_));
	}
}

void BioXASControlEditor::setValueText(const QString &text)
{
	if (valueText_ != text) {
		valueText_ = text;
		updateValueLabel();
	}
}

void BioXASControlEditor::setPrecisionInt(int newPrecision)
{
	if (precision_ != newPrecision) {
		precision_ = newPrecision;
		updateValueLabel();

		emit precisionChanged(precision_);
	}
}

void BioXASControlEditor::setMinimumValueDouble(double newValue)
{
	if (minimumValue_ != newValue) {
		minimumValue_ = newValue;

		emit minimumValueChanged(minimumValue_);
	}
}

void BioXASControlEditor::setMaximumValueDouble(double newValue)
{
	if (maximumValue_ != newValue) {
		maximumValue_ = newValue;

		emit maximumValueChanged(maximumValue_);
	}
}

void BioXASControlEditor::setValuesList(const QStringList &newValues)
{
	if (values_ != newValues) {
		values_ = newValues;

		emit valuesChanged(values_);
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

void BioXASControlEditor::updateTitleText()
{
	if (control_ && useControlNameAsTitle_)
		setTitleText(control_->name());
}

void BioXASControlEditor::updateValue()
{
	if (control_ && control_->canMeasure() && useControlValueAsValue_)
		setValueDouble(control_->value());
}

void BioXASControlEditor::updateValueText()
{
	QString text = "[Invalid]";

	if (control_ && useControlValueAsValue_) {
		text = "[Not measurable]";

		if (control_->canMeasure() && value_.isValid()) {
			text = QString::number(value_, format_.toAscii(), precision_);

			if (!units_.isEmpty())
				text.append(QString(" %1").arg(units_));
		}

	} else if (!useControlValueAsValue_ && value_.isValid()) {

		text = QString::number(value_, format_.toAscii(), precision_);

		if (!units_.isEmpty())
			text.append(QString(" %1").arg(units_));
	}

	setValueText(text);
}

void BioXASControlEditor::updatePrecision()
{
	if (control_ && useControlPrecisionAsPrecision_)
		setPrecisionInt(control_->displayPrecision());
}

void BioXASControlEditor::updateMinimumValue()
{
	if (control_ && useControlMinimumAsMinimum_)
		setMinimumValueDouble(control_->minimumValue());
}

void BioXASControlEditor::updateMaximumValue()
{
	if (control_ && useControlMaximumAsMaximum_)
		setMaximumValueDouble(control_->maximumValue());
}

void BioXASControlEditor::updateValues()
{
	if (control_ && useControlValuesAsValues_)
		setValuesList(control_->moveEnumNames());
}

void BioXASControlEditor::updateUnits()
{
	if (control_ && useControlUnitsAsUnits_)
		setUnitsText(control_->units());
}

void BioXASControlEditor::updateTitle()
{
	QGroupBox::setTitle(title_);
}

void BioXASControlEditor::updateValueLabel()
{
	valueLabel_->setText(valueText_);
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
	AMNumber result = AMNumber(AMNumber::InvalidError);

//	bool inputOK = false;
//	double newValue = QInputDialog::getDouble(this, QString("Editing %1").arg(title_), QString("New value: "), value_, minimumValue_, maximumValue_, precision_, &inputOK);

//	if (inputOK)
//		result = AMNumber(newValue);

	return result;
}

AMNumber BioXASControlEditor::getEnumValue() const
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

//	bool inputOK = false;
//	QString newValueName = QInputDialog::getItem(this, QString("Editing %1").arg(title_), QString("New value: "), values_, value_, true, &inputOK, 0, 0);

//	if (inputOK) {
//		// convert value name to index.
//	}

	return result;
}

AMNumber BioXASControlEditor::getCalibratedDoubleValue() const
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

//	bool inputOK = false;
//	double newValue = QInputDialog::getDouble(this, QString("Calibrating %1").arg(title_), QString("New value: "), value_, minimumValue_, maximumValue_, precision_, &inputOK);

//	if (inputOK)
//		result = AMNumber(newValue);

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

void BioXASControlEditor::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
}
