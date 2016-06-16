#include "CLSControlEditor.h"
#include "beamline/AMControl.h"
#include "ui/beamline/AMControlView.h"

CLSControlEditor::CLSControlEditor(AMControl *control, QWidget *parent) :
	CLSValueEditor(parent)
{
	// Initialize class variables.

	control_ = 0;

	useControlNameAsTitle_ = true;
	useControlValueAsValue_ = true;
	useControlPrecisionAsPrecision_ = true;
	useControlMinimumAsMinimum_ = true;
	useControlMaximumAsMaximum_ = true;
	useControlValuesAsValues_ = true;
	useControlMoveValuesAsMoveValues_ = true;
	useControlUnitsAsUnits_ = true;

	useControlMovingAsDisplayProgress_ = true;
	useControlMoveProgressAsProgress_ = true;

	stopAction_ = new QAction("Stop", this);

	calibrateAction_ = new QAction("Calibrate", this);

	propertiesAction_ = new QAction("Properties", this);

	// Make connections.

	connect( stopAction_, SIGNAL(triggered()), this, SLOT(onStopActionTriggered()) );
	connect( calibrateAction_, SIGNAL(triggered()), this, SLOT(onCalibrateActionTriggered()) );
	connect( propertiesAction_, SIGNAL(triggered()), this, SLOT(onPropertiesActionTriggered()) );

	// Current settings.

	setControl(control);

	refresh();
}

CLSControlEditor::~CLSControlEditor()
{

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
			connect( control_, SIGNAL(movingChanged(bool)), this, SLOT(refresh()) );

			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateValue()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(updateValues()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(updateMoveValues()) );
			connect( control_, SIGNAL(unitsChanged(QString)), this, SLOT(updateUnits()) );
			connect( control_, SIGNAL(minimumValueChanged(double)), this, SLOT(updateMinimumValue()) );
			connect( control_, SIGNAL(maximumValueChanged(double)), this, SLOT(updateMaximumValue()) );

//			connect( control_, SIGNAL(moveStarted()), this, SLOT(updateProgressValueMinimum()) );
//			connect( control_, SIGNAL(moveStarted()), this, SLOT(updateProgressValueMaximum()) );
//			connect( control_, SIGNAL(moveStarted()), this, SLOT(updateDisplayProgress()) );
//			connect( control_, SIGNAL(moveReTargetted()), this, SLOT(updateProgressValueMinimum()) );
//			connect( control_, SIGNAL(moveReTargetted()), this, SLOT(updateProgressValueMaximum()) );
//			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateProgressValue()) );
//			connect( control_, SIGNAL(moveFailed(int)), this, SLOT(updateProgressValue()) );
//			connect( control_, SIGNAL(moveFailed(int)), this, SLOT(updateDisplayProgress()) );
//			connect( control_, SIGNAL(moveSucceeded()), this, SLOT(updateProgressValue()) );
//			connect( control_, SIGNAL(moveSucceeded()), this, SLOT(updateDisplayProgress()) );

			connect( control_, SIGNAL(moveProgressChanged(double)), this, SLOT(updateProgressValue()) );
			connect( control_, SIGNAL(movingChanged(bool)), this, SLOT(updateDisplayProgress()) );
		}

		updateTitleText();
		updateValue();
		updatePrecision();
		updateMinimumValue();
		updateMaximumValue();
		updateValues();
		updateMoveValues();
		updateUnits();

//		updateProgressValueMinimum();
//		updateProgressValueMaximum();
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

void CLSControlEditor::setProgressValue(double newValue)
{
	setUseControlMoveProgressAsProgress(false);
	CLSValueEditor::setProgressValue(newValue);
}

void CLSControlEditor::setUseControlMoveProgressAsProgress(bool useProgress)
{
	if (useControlMoveProgressAsProgress_ != useProgress) {
		useControlMoveProgressAsProgress_ = useProgress;
		updateValueLabel();

		emit useControlMoveProgressAsProgressChanged(useControlMoveProgressAsProgress_);
	}
}

void CLSControlEditor::setDisplayProgress(bool showProgress)
{
	setUseControlMovingAsDisplayProgress(false);
	CLSValueEditor::setDisplayProgress(showProgress);
}

void CLSControlEditor::setUseControlMovingAsDisplayProgress(bool useState)
{
	if (useControlMovingAsDisplayProgress_ != useState) {
		useControlMovingAsDisplayProgress_ = useState;
		updateValueLabel();

		emit useControlMovingAsDisplayProgressChanged(useControlMovingAsDisplayProgress_);
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
	if (control_ && useControlMovingAsDisplayProgress_ && control_->isMoving())
		CLSValueEditor::setProgressValueMinimum(control_->value());
}

void CLSControlEditor::updateProgressValueMaximum()
{
	if (control_ && useControlMovingAsDisplayProgress_ && control_->isMoving())
		CLSValueEditor::setProgressValueMaximum(control_->setpoint());
}

void CLSControlEditor::updateProgressValue()
{
	if (control_ && useControlMoveProgressAsProgress_)
		CLSValueEditor::setProgressValue(control_->moveProgress());
}

void CLSControlEditor::updateDisplayProgress()
{
	if (control_ && useControlMovingAsDisplayProgress_ && control_->isMoving())
		CLSValueEditor::setDisplayProgress(true);
	else
		CLSValueEditor::setDisplayProgress(false);
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
		if (useControlValueAsValue_) {
			if (control_ && control_->canMove())
				enabled = true;

		} else {
			enabled = true;
		}
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

void CLSControlEditor::onEditActionTriggered()
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
				CLSValueEditor::setValue(newValue);
		}

	} else {

		QApplication::beep();
	}
}

void CLSControlEditor::onStopActionTriggered()
{
	if (control_ && control_->canStop())
		control_->stop();
	else
		QApplication::beep();
}

void CLSControlEditor::onCalibrateActionTriggered()
{
	if (control_ && control_->canCalibrate()) {
		AMNumber newValue = getCalibratedDoubleValue();

		if (newValue.isValid())
			control_->calibrate(value_, newValue);

	} else {
		QApplication::beep();
	}
}

void CLSControlEditor::onPropertiesActionTriggered()
{
	if (control_) {

		AMControlViewDialog *view = new AMControlViewDialog(control_);
		view->setWindowTitle(QString("%1 properties").arg(title()));
		view->show();
		view->raise();

		connect( view, SIGNAL(accepted()), view, SLOT(deleteLater()) );
		connect( view, SIGNAL(rejected()), view, SLOT(deleteLater()) );
	}
}

AMNumber CLSControlEditor::getCalibratedDoubleValue()
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	QString dialogTitle = (title_.isEmpty()) ? QString("Calibrate value") : QString("Calibrating %1").arg(title_);
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
		text = "[Not connected]";

		if (control_->isConnected()) {
			text = "[Not measurable]";

			if (control_->canMeasure()) {
				if (control_->isMoving() && control_->isEnum())
					text = "Moving...";
				else
					text = CLSValueEditor::generateValueText();
			}
		}

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
