#include "BioXASControlSetpointEditor.h"

BioXASControlSetpointEditor::BioXASControlSetpointEditor(AMControl *control, BioXASValueSetpointEditor::InputType type, bool showFeedback, QWidget *parent) :
	BioXASValueSetpointEditor(type, showFeedback, parent)
{
	// Initialize class variables.

	control_ = 0;

	useControlValueSetpointAsValue_ = true;
	useControlPrecisionAsPrecision_ = true;
	useControlMinimumAsMinimum_ = true;
	useControlMaximumAsMaximum_ = true;
	useControlValuesAsValues_ = true;
	useControlMoveValuesAsMoveValues_ = true;
	useControlUnitsAsUnits_ = true;

	// Current settings.

	setControl(control);
}

BioXASControlSetpointEditor::~BioXASControlSetpointEditor()
{

}

void BioXASControlSetpointEditor::refresh()
{
	updateValueLabel();
	updateActions();
}

void BioXASControlSetpointEditor::setControl(AMControl *newControl)
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
		}

		updateValue();
		updatePrecision();
		updateMinimumValue();
		updateMaximumValue();
		updateValues();
		updateMoveValues();
		updateUnits();

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASControlSetpointEditor::setValue(double newValue)
{
	setUseControlValueAsValue(false);
	BioXASValueSetpointEditor::setValue(newValue);
}

void BioXASControlSetpointEditor::setUseControlValueSetpointAsValue(bool useValue)
{
	if (useControlValueSetpointAsValue_ != useValue) {
		useControlValueSetpointAsValue_ = useValue;
		updateValueLabel();

		emit useControlValueSetpointAsValueChanged(useControlValueSetpointAsValue_);
	}
}

void BioXASControlSetpointEditor::setValueFeedback(double newValue)
{
	setUseControlValueFeedbackAsValueFeedback(false);
	BioXASValueSetpointEditor::setValueFeedback(newValue);
}

void BioXASControlSetpointEditor::setUseControlValueFeedbackAsValueFeedback(bool useValue)
{
	if (useControlValueFeedbackAsValueFeedback_ != useValue) {
		useControlValueFeedbackAsValueFeedback_ = useValue;
		updateValueLabel();

		emit useControlValueFeedbackAsValueFeedbackChanged(useControlValueFeedbackAsValueFeedback_);
	}
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
		updateValueLabel();

		emit useControlPrecisionAsPrecisionChanged(useControlPrecisionAsPrecision_);
	}
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
}

void BioXASControlSetpointEditor::setValues(const QStringList &newValues)
{
	setUseControlValuesAsValues(false);
	BioXASValueSetpointEditor::setValues(newValues);

}

void BioXASControlSetpointEditor::setUseControlValuesAsValues(bool useValues)
{
	if (useControlValuesAsValues_ != useValues) {
		useControlValuesAsValues_ = useValues;

		emit useControlValuesAsValuesChanged(useControlValuesAsValues_);
	}
}

void BioXASControlSetpointEditor::setMoveValues(const QStringList &newValues)
{
	setUseControlMoveValuesAsMoveValues(false);
	BioXASValueSetpointEditor::setMoveValues(newValues);
}

void BioXASControlSetpointEditor::setUseControlMoveValuesAsMoveValues(bool useValues)
{
	if (useControlMoveValuesAsMoveValues_ != useValues) {
		useControlMoveValuesAsMoveValues_ = useValues;

		emit useControlMoveValuesAsMoveValuesChanged(useControlMoveValuesAsMoveValues_);
	}
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
		updateValueLabel();

		emit useControlUnitsAsUnitsChanged(useControlUnitsAsUnits_);
	}
}

void BioXASControlSetpointEditor::updateValueSetpoint()
{
	if (useControlValueSetpointAsValue_) {

		if (control_ && control_->canMeasure())
			BioXASValueSetpointEditor::setValue(control_->value());
	}
}

void BioXASControlSetpointEditor::updateValueFeedback()
{

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
	if (control_ && useControlValuesAsValues_)
		BioXASValueSetpointEditor::setValues(control_->enumNames());
}

void BioXASControlSetpointEditor::updateMoveValues()
{
	if (control_ && useControlMoveValuesAsMoveValues_)
		BioXASValueSetpointEditor::setMoveValues(control_->moveEnumNames());
}

void BioXASControlSetpointEditor::updateUnits()
{
	if (control_ && useControlUnitsAsUnits_)
		BioXASValueSetpointEditor::setUnits(control_->units());
}
