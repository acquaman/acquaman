#include "AMCurrentAmplifierCompositeView.h"

AMCurrentAmplifierCompositeView::AMCurrentAmplifierCompositeView(AMCurrentAmplifier *amp1, AMCurrentAmplifier *amp2, QWidget *parent) :
    AMCurrentAmplifierView(parent)
{
    amplifier1_ = amp1;
    connect( amplifier1_, SIGNAL(minimumValue(bool)), minus_, SLOT(setDisabled(bool)) );
    connect( amplifier1_, SIGNAL(maximumValue(bool)), plus_, SLOT(setDisabled(bool)) );
    connect( amplifier1_, SIGNAL(valueChanged()), this, SLOT(onAmplifierValueChanged()) );

    amplifier2_ = amp2;
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

void AMCurrentAmplifierCompositeView::onValueComboBoxChanged(const QString &newText)
{
    // the initialized_ boolean prevents the display from setting the amplifier values while view is initializing <- undesirable behavior.
    if (!initialized_ )
        return;

    if (amplifier1_ && amplifier2_ && amplifier1_->amplifierMode() == amplifier2_->amplifierMode()) {
        amplifier1_->setValue(newText);
        amplifier2_->setValue(newText);
    }
}

void AMCurrentAmplifierCompositeView::onMinusClicked()
{
    if (amplifier1_)
        amplifier1_->decreaseValue();

    if (amplifier2_)
        amplifier2_->decreaseValue();
}

void AMCurrentAmplifierCompositeView::onPlusClicked()
{
    if (amplifier1_)
        amplifier1_->increaseValue();

    if (amplifier2_)
        amplifier2_->increaseValue();
}

void AMCurrentAmplifierCompositeView::onAmplifierValueChanged()
{
    amplifier1_->blockSignals(true);
    amplifier2_->blockSignals(true);

    int newIndex = value_->findText( valueToString(amplifier1_->value(), amplifier1_->units()) );

    if (newIndex != -1) {
        value_->setCurrentIndex(newIndex);
    }

    amplifier1_->blockSignals(false);
    amplifier2_->blockSignals(false);
}

QString AMCurrentAmplifierCompositeView::valueToString(double value, const QString &units) const
{
    return QString("%1 %2").arg(value, 0, 'g', 2).arg(units);
}


