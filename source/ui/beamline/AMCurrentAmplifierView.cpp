#include "AMCurrentAmplifierView.h"

AMCurrentAmplifierView::AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, QWidget *parent) :
    QWidget(parent)
{
//    minus_ = new QToolButton();
//    minus_->setMaximumSize(25, 25);
//    minus_->setIcon(QIcon(":/22x22/list-remove.png"));

//    if (amplifier_->isConnected() && amplifier_->atMinimumSensitivity()) {
//        minus_->setDisabled(true);
//    }

//    plus_ = new QToolButton();
//    plus_->setMaximumSize(25, 25);
//    plus_->setIcon(QIcon(":/22x22/list-add.png"));

//    if (amplifier_->isConnected() && amplifier_->atMaximumSensitivity()) {
//        plus_->setDisabled(true);
//    }

    value_ = new QComboBox();
    connect( value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)) );

    units_ = new QComboBox();
    connect( units_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsComboBoxChanged(int)) );

    QHBoxLayout *layout = new QHBoxLayout();
//    layout->addWidget(minus_);
//    layout->addWidget(plus_);
    layout->addWidget(value_);
    layout->addWidget(units_);

    setLayout(layout);

    setCurrentAmplifier(amplifier);
}

AMCurrentAmplifierView::~AMCurrentAmplifierView()
{

}

AMCurrentAmplifier* AMCurrentAmplifierView::currentAmplifier() const
{
    return amplifier_;
}

void AMCurrentAmplifierView::setCurrentAmplifier(AMCurrentAmplifier *amplifier)
{
    if (amplifier_) {
        // disconnects.
    }

    value_->clear();
    units_->clear();

    amplifier_ = amplifier;
    emit currentAmplifierChanged(amplifier_);

    if (amplifier_) {
        // view should detect what information needs to be displayed--gain vs sensitivity.
    }
}
