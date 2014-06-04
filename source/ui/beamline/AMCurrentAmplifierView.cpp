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
        disconnect( amplifier_, SIGNAL(amplifierModeChanged(AMCurrentAmplifier::AmplifierMode)), this, SLOT(setViewMode(AMCurrentAmplifier::AmplifierMode)) );
        disconnect( amplifier_, SIGNAL(gainChanged(int)), value_, SLOT(setCurrentIndex(int)) );
        disconnect( amplifier_, SIGNAL(sensitivityChanged(int)), value_, SLOT(setCurrentIndex(int)) );
        disconnect( amplifier_, SIGNAL(unitsChanged(int)), units_, SLOT(setCurrentIndex(int)) );
    }

    amplifier_ = amplifier;
    emit currentAmplifierChanged(amplifier_);

    if (amplifier_) {
        setViewMode(amplifier_->amplifierMode());

        connect( amplifier_, SIGNAL(amplifierModeChanged(AMCurrentAmplifier::AmplifierMode)), this, SLOT(setViewMode(AMCurrentAmplifier::AmplifierMode)) );
        connect( amplifier_, SIGNAL(gainChanged(int)), value_, SLOT(setCurrentIndex(int)) );
        connect( amplifier_, SIGNAL(sensitivityChanged(int)), value_, SLOT(setCurrentIndex(int)) );
        connect( amplifier_, SIGNAL(unitsChanged(int)), units_, SLOT(setCurrentIndex(int)) );
    }
}

void AMCurrentAmplifierView::setViewMode(AMCurrentAmplifier::AmplifierMode newMode)
{
    viewMode_ = newMode;
    refreshDisplayValues();
}

void AMCurrentAmplifierView::refreshDisplayValues()
{
    clearDisplayValues();

    // repopulate value_ and units_ with appropriate options from the amplifier.
}

void AMCurrentAmplifierView::clearDisplayValues()
{
    value_->clear();
    units_->clear();
}

void AMCurrentAmplifierView::onValueComboBoxChanged(int index)
{
    amplifier_->setValue(QString::number(index));
}

void AMCurrentAmplifierView::onUnitsComboBoxChanged(int index)
{

}

void AMCurrentAmplifierView::onValueChanged(int value)
{

}

void AMCurrentAmplifierView::onUnitsChanged(QString units)
{

}

void AMCurrentAmplifierView::onCustomContextMenuRequested(QPoint position)
{

}
