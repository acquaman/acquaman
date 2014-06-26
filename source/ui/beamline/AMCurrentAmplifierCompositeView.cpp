#include "AMCurrentAmplifierCompositeView.h"

AMCurrentAmplifierCompositeView::AMCurrentAmplifierCompositeView(AMCurrentAmplifier *amp1, AMCurrentAmplifier *amp2, QWidget *parent) :
    AMCurrentAmplifierView(parent)
{
    amplifier1_ = amp1;
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
    Q_UNUSED(newText)
}

void AMCurrentAmplifierCompositeView::onMinusClicked()
{

}

void AMCurrentAmplifierCompositeView::onPlusClicked()
{

}


