#include "BioXASControlEditor.h"
#include "beamline/AMControl.h"

BioXASControlEditor::BioXASControlEditor(AMControl *control, QWidget *parent) :
	BioXASValueEditor(parent)
{
	// Initialize class variables.

	control_ = 0;

	useControlNameAsTitle_ = true;
	useControlValueAsValue_ = true;
	useControlPrecisionAsPrecision_ = true;
	useControlMinimumAsMinimum_ = true;
	useControlMaximumAsMaximum_ = true;
	useControlValuesAsValues_ = true;
	useControlUnitsAsUnits_ = true;

	stopAction_ = new QAction("Stop", this);

	calibrateAction_ = new QAction("Calibrate", this);

	// Make connections.

	connect( stopAction_, SIGNAL(triggered()), this, SLOT(onStopActionTriggered()) );
	connect( calibrateAction_, SIGNAL(triggered()), this, SLOT(onCalibrateActionTriggered()) );

	// Current settings.

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
	updateActions();
}

void BioXASControlEditor::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(movingChanged(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateValue()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(updateValues()) );
			connect( control_, SIGNAL(unitsChanged(QString)), this, SLOT(updateUnits()) );
		}

		updateTitleText();
		updateValue();
		updatePrecision();
		updateMinimumValue();
		updateMaximumValue();
		updateValues();
		updateUnits();

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASControlEditor::setTitle(const QString &title)
{
	if (title_ != title) {
		setUseControlNameAsTitle(false);
		BioXASValueEditor::setTitle(title);
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

void BioXASControlEditor::setValue(AMNumber newValue)
{
	if (value_ != AMNumber(newValue)) {
		setUseControlValueAsValue(false);
		BioXASValueEditor::setValue(newValue);
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

void BioXASControlEditor::setPrecision(int newPrecision)
{
	if (precision_ != newPrecision && validPrecision(newPrecision)) {
		setUseControlPrecisionAsPrecision(false);
		BioXASValueEditor::setPrecision(newPrecision);

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
		BioXASValueEditor::setMinimumValue(minimumValue);
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
		BioXASValueEditor::setMaximumValue(maximumValue);
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
		BioXASValueEditor::setValues(newValues);
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
		BioXASValueEditor::setUnits(newUnits);
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

void BioXASControlEditor::updateTitleText()
{
	if (control_ && useControlNameAsTitle_)
		BioXASValueEditor::setTitle(control_->name());
}

void BioXASControlEditor::updateValue()
{
	if (useControlValueAsValue_) {

		if (control_ && control_->canMeasure())
			BioXASValueEditor::setValue(AMNumber(control_->value()));
		else
			BioXASValueEditor::setValue(AMNumber(AMNumber::InvalidError));
	}
}

void BioXASControlEditor::updatePrecision()
{
	if (control_ && useControlPrecisionAsPrecision_)
		BioXASValueEditor::setPrecision(control_->displayPrecision());
}

void BioXASControlEditor::updateMinimumValue()
{
	if (control_ && useControlMinimumAsMinimum_)
		BioXASValueEditor::setMinimumValue(control_->minimumValue());
}

void BioXASControlEditor::updateMaximumValue()
{
	if (control_ && useControlMaximumAsMaximum_)
		BioXASValueEditor::setMaximumValue(control_->maximumValue());
}

void BioXASControlEditor::updateValues()
{
	if (control_ && useControlValuesAsValues_)
		BioXASValueEditor::setValues(control_->enumNames());
}

void BioXASControlEditor::updateUnits()
{
	if (control_ && useControlUnitsAsUnits_)
		BioXASValueEditor::setUnits(control_->units());
}

void BioXASControlEditor::updateActions()
{
	updateEditAction();
	updateStopAction();
	updateCalibrateAction();
}

void BioXASControlEditor::updateEditAction()
{
	bool enabled = false;

	if (!readOnly_) {
		if (useControlValueAsValue_) {
			if (control_ && control_->canMove())
				enabled = true;

		} else {
			enabled = true;
		}
	}

	editAction_->setEnabled(enabled);
}

void BioXASControlEditor::updateStopAction()
{
	bool enabled = false;

	if (!readOnly_) {
		if (control_ && control_->canStop())
			enabled = true;
	}

	stopAction_->setEnabled(enabled);
}

void BioXASControlEditor::updateCalibrateAction()
{
	bool enabled = false;

	if (!readOnly_) {
		if (control_ && control_->canCalibrate())
			enabled = true;
	}

	calibrateAction_->setEnabled(enabled);
}

void BioXASControlEditor::onEditActionTriggered()
{
	if (!readOnly_) {

		if (useControlValueAsValue_) {

			if (control_ && control_->canMove()) {
				AMNumber newValue = AMNumber(AMNumber::InvalidError);

				if (control_->isEnum())
					newValue = getEnumValue();
				else
					newValue = getDoubleValue();

				if (newValue.isValid())
					control_->move(double(newValue));

			} else {
				QApplication::beep();
			}

		} else {

			AMNumber newValue = AMNumber(AMNumber::InvalidError);

			if (values_.isEmpty())
				newValue = getDoubleValue();
			else
				newValue = getEnumValue();

			if (newValue.isValid())
				BioXASValueEditor::setValue(newValue);
		}

	} else {

		QApplication::beep();
	}
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
		AMNumber newValue = getCalibratedDoubleValue();

		if (newValue.isValid())
			control_->calibrate(value_, newValue);

	} else {
		QApplication::beep();
	}
}

AMNumber BioXASControlEditor::getCalibratedDoubleValue()
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	QString dialogTitle = (title_.isEmpty()) ? QString("Calibrate value") : QString("Calibrating %1").arg(title_);
	bool inputOK = false;

	double newValue = QInputDialog::getDouble(this, dialogTitle, QString("New value: "), value_, minimumValue_, maximumValue_, precision_, &inputOK);

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

	contextMenu.addAction(editAction_);
	contextMenu.addAction(stopAction_);
//	contextMenu.addAction(calibrateAction_);

	// Show menu.

	contextMenu.exec(mapToGlobal(clickPosition));
}

QString BioXASControlEditor::generateValueText() const
{
	QString text = "[Invalid control]";

	if (control_ && useControlValueAsValue_) {
		text = "[Not connected]";

		if (control_->isConnected()) {
			text = "[Not measurable]";

			if (control_->canMeasure()) {
				if (control_->isMoving() && control_->isEnum())
					text = "Moving...";
				else
					text = BioXASValueEditor::generateValueText();
			}
		}

	} else if (!useControlValueAsValue_) {

		text = BioXASValueEditor::generateValueText();
	}

	return text;
}

QString BioXASControlEditor::generateUnitsText() const
{
	QString text = "";

	if (!units_.isEmpty() && units_ != "[choice]")
		text = QString(" %1").arg(units_);

	return text;
}
