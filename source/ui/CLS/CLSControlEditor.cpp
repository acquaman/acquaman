#include "CLSControlEditor.h"
#include "beamline/AMControl.h"
#include "ui/beamline/AMControlView.h"

CLSControlEditor::CLSControlEditor(AMControl *control, QWidget *parent) :
	CLSValueEditor(parent)
{
	// Setup class variables.

	control_ = 0;

	useControlNameAsTitle_ = true;
	useControlValueAsValue_ = true;
	useControlPrecisionAsPrecision_ = true;
	useControlMinimumAsMinimum_ = true;
	useControlMaximumAsMaximum_ = true;
	useControlValuesAsValues_ = true;
	useControlMoveValuesAsMoveValues_ = true;
	useControlUnitsAsUnits_ = true;
	useControlMovingAsProgress_ = true;

	stopAction_ = new QAction("Stop", this);
	connect( stopAction_, SIGNAL(triggered()), this, SLOT(onStopActionTriggered()) );

	calibrateAction_ = new QAction("Calibrate", this);
	connect( calibrateAction_, SIGNAL(triggered()), this, SLOT(onCalibrateActionTriggered()) );

	propertiesAction_ = new QAction("Properties", this);
	connect( propertiesAction_, SIGNAL(triggered()), this, SLOT(onPropertiesActionTriggered()) );

	// Current settings.

	setControl(control);

	refresh();
}

CLSControlEditor::~CLSControlEditor()
{

}

void CLSControlEditor::hideBorder()
{
	setStyleSheet("border:0;");
	setTitle("");
}

void CLSControlEditor::refresh()
{
	updateTitle();
	updateValueLabel();
	updateActions();
}

void CLSControlEditor::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );

			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateValue()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateProgressValue()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(updateValues()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(updateMoveValues()) );
			connect( control_, SIGNAL(unitsChanged(QString)), this, SLOT(updateUnits()) );
			connect( control_, SIGNAL(minimumValueChanged(double)), this, SLOT(updateMinimumValue()) );
			connect( control_, SIGNAL(maximumValueChanged(double)), this, SLOT(updateMaximumValue()) );

			connect( control_, SIGNAL(movingChanged(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(moveStarted()), this, SLOT(updateProgressValueMinimum()) );
			connect( control_, SIGNAL(moveStarted()), this, SLOT(updateProgressValueMaximum()) );
			connect( control_, SIGNAL(moveStarted()), this, SLOT(updateDisplayProgress()) );
			connect( control_, SIGNAL(moveReTargetted()), this, SLOT(updateProgressValueMinimum()) );
			connect( control_, SIGNAL(moveReTargetted()), this, SLOT(updateProgressValueMaximum()) );
			connect( control_, SIGNAL(moveFailed(int)), this, SLOT(updateProgressValue()) );
			connect( control_, SIGNAL(moveFailed(int)), this, SLOT(updateDisplayProgress()) );
			connect( control_, SIGNAL(moveFailed(int)), this, SLOT(updateInitiatedCurrentMove()) );
			connect( control_, SIGNAL(moveSucceeded()), this, SLOT(updateProgressValue()) );
			connect( control_, SIGNAL(moveSucceeded()), this, SLOT(updateDisplayProgress()) );
			connect( control_, SIGNAL(moveSucceeded()), this, SLOT(updateInitiatedCurrentMove()) );
		}

		updateTitleText();
		updateValue();
		updatePrecision();
		updateMinimumValue();
		updateMaximumValue();
		updateValues();
		updateMoveValues();
		updateUnits();

		updateProgressValueMinimum();
		updateProgressValueMaximum();
		updateProgressValue();
		updateDisplayProgress();

		refresh();

		emit controlChanged(control_);
	}
}

void CLSControlEditor::setTitle(const QString &title)
{
	setUseControlNameAsTitle(false);
	CLSValueEditor::setTitle(title);
}

void CLSControlEditor::setUseControlNameAsTitle(bool useName)
{
	if (useControlNameAsTitle_ != useName) {
		useControlNameAsTitle_ = useName;
		updateTitle();

		emit useControlNameAsTitleChanged(useName);
	}
}

void CLSControlEditor::setValue(const AMNumber &newValue)
{
	setUseControlValueAsValue(false);
	CLSValueEditor::setValue(newValue);
}

void CLSControlEditor::setUseControlValueAsValue(bool useValue)
{
	if (useControlValueAsValue_ != useValue) {
		useControlValueAsValue_ = useValue;
		updateValueLabel();

		emit useControlValueAsValueChanged(useControlValueAsValue_);
	}
}

void CLSControlEditor::setPrecision(int newPrecision)
{
	setUseControlPrecisionAsPrecision(false);
	CLSValueEditor::setPrecision(newPrecision);
}

void CLSControlEditor::setUseControlPrecisionAsPrecision(bool usePrecision)
{
	if (useControlPrecisionAsPrecision_ != usePrecision) {
		useControlPrecisionAsPrecision_ = usePrecision;
		updateValueLabel();

		emit useControlPrecisionAsPrecisionChanged(useControlPrecisionAsPrecision_);
	}
}

void CLSControlEditor::setMinimumValue(double minimumValue)
{
	setUseControlMinimumAsMinimum(false);
	CLSValueEditor::setMinimumValue(minimumValue);
}

void CLSControlEditor::setUseControlMinimumAsMinimum(bool useMinimum)
{
	if (useControlMinimumAsMinimum_ != useMinimum) {
		useControlMinimumAsMinimum_ = useMinimum;

		emit useControlMinimumAsMinimumChanged(useControlMinimumAsMinimum_);
	}
}

void CLSControlEditor::setMaximumValue(double maximumValue)
{
	setUseControlMaximumAsMaximum(false);
	CLSValueEditor::setMaximumValue(maximumValue);
}

void CLSControlEditor::setUseControlMaximumAsMaximum(bool useMaximum)
{
	if (useControlMaximumAsMaximum_ != useMaximum) {
		useControlMaximumAsMaximum_ = useMaximum;

		emit useControlMaximumAsMaximumChanged(useControlMaximumAsMaximum_);
	}
}

void CLSControlEditor::setValues(const QStringList &newValues)
{
	setUseControlValuesAsValues(false);
	CLSValueEditor::setValues(newValues);

}

void CLSControlEditor::setUseControlValuesAsValues(bool useValues)
{
	if (useControlValuesAsValues_ != useValues) {
		useControlValuesAsValues_ = useValues;

		emit useControlValuesAsValuesChanged(useControlValuesAsValues_);
	}
}

void CLSControlEditor::setMoveValues(const QStringList &newValues)
{
	setUseControlMoveValuesAsMoveValues(false);
	CLSValueEditor::setMoveValues(newValues);
}

void CLSControlEditor::setUseControlMoveValuesAsMoveValues(bool useValues)
{
	if (useControlMoveValuesAsMoveValues_ != useValues) {
		useControlMoveValuesAsMoveValues_ = useValues;

		emit useControlMoveValuesAsMoveValuesChanged(useControlMoveValuesAsMoveValues_);
	}
}

void CLSControlEditor::setUnits(const QString &newUnits)
{
	setUseControlUnitsAsUnits(false);
	CLSValueEditor::setUnits(newUnits);
}

void CLSControlEditor::setUseControlUnitsAsUnits(bool useUnits)
{
	if (useControlUnitsAsUnits_ != useUnits) {
		useControlUnitsAsUnits_ = useUnits;
		updateValueLabel();

		emit useControlUnitsAsUnitsChanged(useControlUnitsAsUnits_);
	}
}

void CLSControlEditor::setDisplayProgress(bool displayProgress)
{
	setUseControlMovingStateToDisplayProgress(false);
	CLSValueEditor::setDisplayProgress(displayProgress);
}

void CLSControlEditor::setUseControlMovingStateToDisplayProgress(bool useState)
{
	if (useControlMovingAsProgress_ != useState) {
		useControlMovingAsProgress_ = useState;
		updateValueLabel();

		emit useControlMovingStateToDisplayProgressChanged(useControlMovingAsProgress_);
	}
}

void CLSControlEditor::setInitiatedCurrentMove(bool initiatedMove)
{
	if (initiatedCurrentMove_ != initiatedMove) {
		initiatedCurrentMove_ = initiatedMove;
		emit initiatedCurrentMoveChanged(initiatedCurrentMove_);
	}
}

void CLSControlEditor::updateTitleText()
{
	if (control_ && useControlNameAsTitle_)
		CLSValueEditor::setTitle(control_->name());
}

void CLSControlEditor::updateValue()
{
	if (useControlValueAsValue_) {

		if (control_ && control_->canMeasure())
			CLSValueEditor::setValue(AMNumber(control_->value()));
		else
			CLSValueEditor::setValue(AMNumber(AMNumber::InvalidError));
	}
}

void CLSControlEditor::updatePrecision()
{
	if (control_ && useControlPrecisionAsPrecision_)
		CLSValueEditor::setPrecision(control_->displayPrecision());
}

void CLSControlEditor::updateMinimumValue()
{
	if (control_ && useControlMinimumAsMinimum_)
		CLSValueEditor::setMinimumValue(control_->minimumValue());
}

void CLSControlEditor::updateMaximumValue()
{
	if (control_ && useControlMaximumAsMaximum_)
		CLSValueEditor::setMaximumValue(control_->maximumValue());
}

void CLSControlEditor::updateValues()
{
	if (control_ && useControlValuesAsValues_)
		CLSValueEditor::setValues(control_->enumNames());
}

void CLSControlEditor::updateMoveValues()
{
	if (control_ && useControlMoveValuesAsMoveValues_)
		CLSValueEditor::setMoveValues(control_->moveEnumNames());
}

void CLSControlEditor::updateUnits()
{
	if (control_ && useControlUnitsAsUnits_)
		CLSValueEditor::setUnits(control_->units());
}

void CLSControlEditor::updateProgressValueMinimum()
{
	if (control_ && useControlMovingAsProgress_ && control_->moveInProgress())
		CLSValueEditor::setProgressValueMinimum(control_->value());
}

void CLSControlEditor::updateProgressValueMaximum()
{
	if (control_ && useControlMovingAsProgress_ && control_->moveInProgress())
		CLSValueEditor::setProgressValueMaximum(control_->setpoint());
}

void CLSControlEditor::updateProgressValue()
{
	if (control_ && useControlMovingAsProgress_ && control_->moveInProgress())
		CLSValueEditor::setProgressValue(control_->value());
}

void CLSControlEditor::updateDisplayProgress()
{
	if (control_ && useControlMovingAsProgress_ && control_->moveInProgress())
		CLSValueEditor::setDisplayProgress(true);
	else
		CLSValueEditor::setDisplayProgress(false);
}

void CLSControlEditor::updateInitiatedCurrentMove()
{
	if (control_ && initiatedCurrentMove_ && !control_->isMoving())
		setInitiatedCurrentMove(false);
	else if (!control_)
		setInitiatedCurrentMove(false);
}

void CLSControlEditor::updateActions()
{
	updateEditAction();
	updateStopAction();
	updateCalibrateAction();
}

void CLSControlEditor::updateEditAction()
{
	bool enabled = false;

	if (!readOnly_) {
		if (useControlValueAsValue_ && control_ && control_->canMove())
			enabled = true;
		else if (!useControlValueAsValue_)
			enabled = true;
	}

	editAction_->setEnabled(enabled);
}

void CLSControlEditor::updateStopAction()
{
	bool enabled = false;

	if (!readOnly_) {
		if (control_ && control_->canStop())
			enabled = true;
	}

	stopAction_->setEnabled(enabled);
}

void CLSControlEditor::updateCalibrateAction()
{
	bool enabled = false;

	if (!readOnly_) {
		if (control_ && control_->canCalibrate())
			enabled = true;
	}

	calibrateAction_->setEnabled(enabled);
}

void CLSControlEditor::updatePropertiesAction()
{
	bool enabled = false;

	if (control_)
		enabled = true;

	propertiesAction_->setEnabled(enabled);
}

void CLSControlEditor::editImplementation()
{
	if (useControlValueAsValue_) {

		// If the control is valid and can move, then identify the
		// new value setpoint and move the control. If any of those
		// conditions aren't met, we do nothing.

		if (control_ && control_->canMove()) {

			AMNumber newValue = getValue();

			if (newValue.isValid()) {
				setInitiatedCurrentMove(true);
				control_->move(double(newValue));
			}
		}

	} else {

		CLSValueEditor::editImplementation();
	}
}

AMNumber CLSControlEditor::getValue()
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	if (control_) {
		if (control_->isEnum())
			result = getEnumValue();
		else
			result = getDoubleValue();
	}

	return result;
}

void CLSControlEditor::onStopActionTriggered()
{
	if (control_ && control_->canStop())
		control_->stop();
}

void CLSControlEditor::onCalibrateActionTriggered()
{
	if (control_ && control_->canCalibrate()) {
		AMNumber newValue = getCalibratedDoubleValue();

		if (newValue.isValid())
			control_->calibrate(value_, newValue);
	}
}

void CLSControlEditor::onPropertiesActionTriggered()
{
	if (control_) {

		AMControlViewDialog *view = new AMControlViewDialog(control_);
		view->setWindowTitle(QString("%1 properties").arg(title_));
		view->show();
		view->raise();

		connect( view, SIGNAL(accepted()), view, SLOT(deleteLater()) );
		connect( view, SIGNAL(rejected()), view, SLOT(deleteLater()) );
	}
}

AMNumber CLSControlEditor::getCalibratedDoubleValue()
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	QString dialogTitle = (title_.isEmpty()) ? QString("Calibrate value") : QString("Calibrating %1").arg(title_.toLower());
	bool inputOK = false;

	double newValue = QInputDialog::getDouble(this, dialogTitle, QString("New value: "), value_, minimumValue_, maximumValue_, precision_, &inputOK);

	if (inputOK)
		result = AMNumber(newValue);

	return result;
}

void CLSControlEditor::onContextMenuRequested(const QPoint &clickPosition)
{
	// Update the actions to reflect current control settings.

	updateActions();

	// Create context menu.

	QMenu contextMenu;

	contextMenu.addAction(editAction_);
	contextMenu.addAction(stopAction_);
//	contextMenu.addAction(calibrateAction_);
	contextMenu.addSeparator();
	contextMenu.addAction(propertiesAction_);

	// Show menu.

	contextMenu.exec(mapToGlobal(clickPosition));
}

QString CLSControlEditor::generateValueText() const
{
	QString text = "[Invalid control]";

	if (control_ && useControlValueAsValue_) {
		text = "[Not measurable]";

		if (control_->canMeasure())
			text = CLSValueEditor::generateValueText();

	} else if (!useControlValueAsValue_) {

		text = CLSValueEditor::generateValueText();
	}

	return text;
}

QString CLSControlEditor::generateUnitsText() const
{
	QString text = "";

	if (!units_.isEmpty() && units_ != "[choice]")
		text = QString(" %1").arg(units_);

	return text;
}
