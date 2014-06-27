#include "AMCurrentAmplifierCompositeView.h"

AMCurrentAmplifierCompositeView::AMCurrentAmplifierCompositeView(AMCurrentAmplifier *amp1, AMCurrentAmplifier *amp2, QWidget *parent) :
    AMCurrentAmplifierView(parent)
{
    amplifier1_ = amp1;

    if (amplifier1_) {
        connect( amplifier1_, SIGNAL(minimumValue(bool)), minus_, SLOT(setDisabled(bool)) );
        connect( amplifier1_, SIGNAL(maximumValue(bool)), plus_, SLOT(setDisabled(bool)) );
        connect( amplifier1_, SIGNAL(valueChanged()), this, SLOT(onAmplifierValueChanged()) );
    }

    amplifier2_ = amp2;

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
    if (amplifier1_ && amplifier2_ && amplifier1_->amplifierMode() == amplifier2_->amplifierMode())
        return true;
    else
        return false;
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
    value_->clear();

    QStringList unitsList = amplifier1_->unitsList();
    QList<double> valuesList = amplifier1_->values();

    foreach (QString units, unitsList) {
        foreach (double value, valuesList) {
            QString item = valueToString(value, units);
            value_->addItem(item);
        }
    }
}

void AMCurrentAmplifierCompositeView::onAmplifierValueChanged()
{
    if (isValid()) {
        amplifier1_->blockSignals(true);
        amplifier2_->blockSignals(true);

        int newIndex = value_->findText( valueToString(amplifier1_->value(), amplifier1_->units()) );

        if (newIndex != -1) {
            value_->setCurrentIndex(newIndex);
        }

        amplifier1_->blockSignals(false);
        amplifier2_->blockSignals(false);
    }
}

QString AMCurrentAmplifierCompositeView::valueToString(double value, const QString &units) const
{
    return QString("%1 %2").arg(value, 0, 'g', 2).arg(units);
}


