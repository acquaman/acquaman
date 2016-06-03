#include "BioXASControlSetpointEditor.h"
#include "beamline/AMControl.h"

BioXASControlSetpointEditor::BioXASControlSetpointEditor(AMControl *control, bool showFeedback, QWidget *parent) :
	BioXASValueSetpointEditor(BioXASValueSetpointEditor::TypeDouble, showFeedback, parent)
{
	// Initialize class variables.

	control_ = 0;

	useControlValueSetpointAsValue_ = true;
	useControlValueAsValueFeedback_ = true;
	useControlPrecisionAsPrecision_ = true;
	useControlMinimumAsMinimum_ = true;
	useControlMaximumAsMaximum_ = true;
	useControlMoveValuesAsValues_ = true;
	useControlUnitsAsUnits_ = true;
	useControlEnumStatusAsEditorInputType_ = true;

	// Current settings.

	setControl(control);
}

BioXASControlSetpointEditor::~BioXASControlSetpointEditor()
{

}

void BioXASControlSetpointEditor::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(setpointChanged(double)), this, SLOT(updateValue()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateValueFeedback()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(updateValues()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(updateInputType()) );
			connect( control_, SIGNAL(minimumValueChanged(double)), this, SLOT(updateMinimumValue()) );
			connect( control_, SIGNAL(maximumValueChanged(double)), this, SLOT(updateMaximumValue()) );
			connect( control_, SIGNAL(unitsChanged(QString)), this, SLOT(updateUnits()) );
		}

		emit controlChanged(control_);
	}

	updateValueFeedback();
	updateValues();
	updatePrecision();
	updateMinimumValue();
	updateMaximumValue();
	updateUnits();
	updateInputType();
}

void BioXASControlSetpointEditor::setValue(double newValue)
{
	setUseControlValueSetpointAsValue(false);
	BioXASValueSetpointEditor::setValue(newValue);
}

void BioXASControlSetpointEditor::setUseControlValueSetpointAsValue(bool useValue)
{
	if (useControlValueSetpointAsValue_ != useValue) {
		useControlValueSetpointAsValue_ = useValue;
		emit useControlValueSetpointAsValueChanged(useControlValueSetpointAsValue_);
	}

	updateValue();
}

void BioXASControlSetpointEditor::setValueFeedback(double newValue)
{
	setUseControlValueAsValueFeedback(false);
	BioXASValueSetpointEditor::setValueFeedback(newValue);
}

void BioXASControlSetpointEditor::setUseControlValueAsValueFeedback(bool useValue)
{
	if (useControlValueAsValueFeedback_ != useValue) {
		useControlValueAsValueFeedback_ = useValue;
		emit useControlValueAsValueFeedbackChanged(useControlValueAsValueFeedback_);
	}

	updateValueFeedback();
}

void BioXASControlSetpointEditor::setPrecision(int newPrecision)
{
	setUseControlPrecisionAsPrecision(false);
	BioXASValueSetpointEditor::setPrecision(newPrecision);
}

void BioXASControlSetpointEditor::setUseControlPrecisionAsPrecision(bool usePrecision)
{
	if (useControlPrecisionAsPrecision_ != usePrecision) {
		useControlPrecisionAsPrecision_ = usePrecision;
		emit useControlPrecisionAsPrecisionChanged(useControlPrecisionAsPrecision_);
	}

	updatePrecision();
}

void BioXASControlSetpointEditor::setMinimumValue(double minimumValue)
{
	setUseControlMinimumAsMinimum(false);
	BioXASValueSetpointEditor::setMinimumValue(minimumValue);
}

void BioXASControlSetpointEditor::setUseControlMinimumAsMinimum(bool useMinimum)
{
	if (useControlMinimumAsMinimum_ != useMinimum) {
		useControlMinimumAsMinimum_ = useMinimum;
		emit useControlMinimumAsMinimumChanged(useControlMinimumAsMinimum_);
	}

	updateMinimumValue();
}

void BioXASControlSetpointEditor::setMaximumValue(double maximumValue)
{
	setUseControlMaximumAsMaximum(false);
	BioXASValueSetpointEditor::setMaximumValue(maximumValue);
}

void BioXASControlSetpointEditor::setUseControlMaximumAsMaximum(bool useMaximum)
{
	if (useControlMaximumAsMaximum_ != useMaximum) {
		useControlMaximumAsMaximum_ = useMaximum;
		emit useControlMaximumAsMaximumChanged(useControlMaximumAsMaximum_);
	}

	updateMaximumValue();
}

void BioXASControlSetpointEditor::setValues(const QStringList &newValues)
{
	setUseControlMoveValuesAsValues(false);
	BioXASValueSetpointEditor::setValues(newValues);
}

void BioXASControlSetpointEditor::setUseControlMoveValuesAsValues(bool useValues)
{
	if (useControlMoveValuesAsValues_ != useValues) {
		useControlMoveValuesAsValues_ = useValues;
		emit useControlMoveValuesAsValuesChanged(useControlMoveValuesAsValues_);
	}

	updateValues();
}

void BioXASControlSetpointEditor::setUnits(const QString &newUnits)
{
	setUseControlUnitsAsUnits(false);
	BioXASValueSetpointEditor::setUnits(newUnits);
}

void BioXASControlSetpointEditor::setUseControlUnitsAsUnits(bool useUnits)
{
	if (useControlUnitsAsUnits_ != useUnits) {
		useControlUnitsAsUnits_ = useUnits;
		emit useControlUnitsAsUnitsChanged(useControlUnitsAsUnits_);
	}

	updateUnits();
}

void BioXASControlSetpointEditor::setInputType(BioXASValueSetpointEditor::InputType newType)
{
	setUseControlEnumStatusAsInputType(false);
	BioXASValueSetpointEditor::setInputType(newType);
}

void BioXASControlSetpointEditor::setUseControlEnumStatusAsInputType(bool useStatus)
{
	if (useControlEnumStatusAsEditorInputType_ != useStatus) {
		useControlEnumStatusAsEditorInputType_ = useStatus;
		emit useControlEnumStatusAsEditorInputTypeChanged(useControlEnumStatusAsEditorInputType_);
	}

	updateInputType();
}
#include <QDebug>
void BioXASControlSetpointEditor::initializeValue()
{
	if (control_ && control_->canMeasure() && useControlValueSetpointAsValue_) {
		qDebug() << "\n\nInitializing editor value to" << control_->value();
		BioXASValueSetpointEditor::setValue(control_->value());
	}
}

void BioXASControlSetpointEditor::updateValue()
{
	if (control_ && useControlValueSetpointAsValue_)
		BioXASValueSetpointEditor::setValue(control_->setpoint());
}

void BioXASControlSetpointEditor::updateValueFeedback()
{
	if (control_ && control_->canMeasure() && useControlValueAsValueFeedback_)
		BioXASValueSetpointEditor::setValueFeedback(control_->value());
}

void BioXASControlSetpointEditor::updatePrecision()
{
	if (control_ && useControlPrecisionAsPrecision_)
		BioXASValueSetpointEditor::setPrecision(control_->displayPrecision());
}

void BioXASControlSetpointEditor::updateMinimumValue()
{
	if (control_ && useControlMinimumAsMinimum_)
		BioXASValueSetpointEditor::setMinimumValue(control_->minimumValue());
}

void BioXASControlSetpointEditor::updateMaximumValue()
{
	if (control_ && useControlMaximumAsMaximum_)
		BioXASValueSetpointEditor::setMaximumValue(control_->maximumValue());
}

void BioXASControlSetpointEditor::updateValues()
{
	if (control_ && useControlMoveValuesAsValues_)
		BioXASValueSetpointEditor::setValues(control_->moveEnumNames());
}

void BioXASControlSetpointEditor::updateUnits()
{
	if (control_ && useControlUnitsAsUnits_)
		BioXASValueSetpointEditor::setUnits(control_->units());
}

void BioXASControlSetpointEditor::updateInputType()
{
	if (control_ && useControlEnumStatusAsEditorInputType_)
		BioXASValueSetpointEditor::setInputType(control_->isEnum() ? BioXASValueSetpointEditor::TypeEnum : BioXASValueSetpointEditor::TypeDouble );
}
