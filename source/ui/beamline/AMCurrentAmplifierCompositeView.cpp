#include "AMCurrentAmplifierCompositeView.h"

AMCurrentAmplifierCompositeView::AMCurrentAmplifierCompositeView(AMCurrentAmplifier *amp1, AMCurrentAmplifier *amp2, QWidget *parent) :
	AMCurrentAmplifierView(parent)
{
	amplifier1_ = amp1;

	if (amplifier1_) {
		connect( amplifier1_, SIGNAL(minimumValue(bool)), minus_, SLOT(setDisabled(bool)) );
		connect( amplifier1_, SIGNAL(maximumValue(bool)), plus_, SLOT(setDisabled(bool)) );
		connect( amplifier1_, SIGNAL(valueChanged()), this, SLOT(onAmplifierValueChanged()) );
		connect( amplifier1_, SIGNAL(isConnected(bool)), this, SLOT(refreshView()));
	}

	amplifier2_ = amp2;

	if (amplifier2_) {
		connect( amplifier2_, SIGNAL(minimumValue(bool)), minus_, SLOT(setDisabled(bool)) );
		connect( amplifier2_, SIGNAL(maximumValue(bool)), plus_, SLOT(setDisabled(bool)) );
		connect( amplifier2_, SIGNAL(valueChanged()), this, SLOT(onAmplifierValueChanged()) );
		connect( amplifier2_, SIGNAL(isConnected(bool)), this, SLOT(refreshView()));
	}

	refreshView();
}

AMCurrentAmplifierCompositeView::~AMCurrentAmplifierCompositeView()
{

}

AMCurrentAmplifier* AMCurrentAmplifierCompositeView::amplifier1() const
{
	return amplifier1_;
}

AMCurrentAmplifier* AMCurrentAmplifierCompositeView::amplifier2() const
{
	return amplifier2_;
}

bool AMCurrentAmplifierCompositeView::isValid() const
{
	return (amplifier1_ && amplifier2_
			&& amplifier1_->isConnected() && amplifier2_->isConnected()
			&& amplifier1_->amplifierMode() == amplifier2_->amplifierMode());
}

void AMCurrentAmplifierCompositeView::onAmplifierValueChanged()
{
	if (isValid()) {
		amplifier1_->blockSignals(true);
		amplifier2_->blockSignals(true);

		int newIndex = value_->findText( toDisplay(amplifier1_->value(), amplifier1_->units()) );

		if (newIndex != -1) {
			value_->setCurrentIndex(newIndex);
		}

		amplifier1_->blockSignals(false);
		amplifier2_->blockSignals(false);
	}
}

void AMCurrentAmplifierCompositeView::onValueComboBoxChangedImplementation(const QString &newText)
{
	amplifier1_->setValue(newText);
	amplifier2_->setValue(newText);
}

void AMCurrentAmplifierCompositeView::onMinusClickedImplementation()
{
	amplifier1_->decreaseValue();
	amplifier2_->decreaseValue();
}

void AMCurrentAmplifierCompositeView::onPlusClickedImplementation()
{
	amplifier1_->increaseValue();
	amplifier2_->increaseValue();
}

void AMCurrentAmplifierCompositeView::refreshViewImplementation()
{
	refreshValues();
	refreshButtons();
}

void AMCurrentAmplifierCompositeView::refreshValues()
{
	value_->clear();

	double minValue = 0;
	double maxValue = 0;

	// (re)populate value_ with appropriate options provided by the amplifier.
	QStringList unitsList = amplifier1_->unitsList();
	QList<double> valuesList = amplifier1_->values();

	foreach (QString units, unitsList) {
		foreach (double value, valuesList) {

			minValue = amplifier1_->minimumValueForUnits(units);
			maxValue = amplifier1_->maximumValueForUnits(units);

			if (value >= minValue && value <= maxValue) {
				QString item = toDisplay(value, units);
				value_->addItem(item);
			}
		}
	}

	// values displayed should represent the amplifier's current state.
	onAmplifierValueChanged();
}

void AMCurrentAmplifierCompositeView::refreshButtons()
{
	minus_->setDisabled(amplifier1_->atMinimumValue());
	plus_->setDisabled(amplifier1_->atMaximumValue());
}


