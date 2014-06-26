#include "AMCurrentAmplifierCompositeView.h"

AMCurrentAmplifierCompositeView::AMCurrentAmplifierCompositeView(AMCurrentAmplifier *amp1, AMCurrentAmplifier *amp2, QWidget *parent) :
    QWidget(parent)
{
    amplifier1_ = amp1;
    amplifier2_ = amp2;
}

AMCurrentAmplifierCompositeView::~AMCurrentAmplifierCompositeView()
{

}

AMCurrentAmplifier* AMCurrentAmplifierView::amplifier1() const
{
    return amplifier1_;
}

AMCurrentAmplifier* AMCurrentAmplifierView::amplifier2() const
{
    return amplifier2_;
}


