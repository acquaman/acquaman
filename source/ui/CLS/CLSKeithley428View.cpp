#include "CLSKeithley428View.h"

#include <QDebug>

CLSKeithley428View::CLSKeithley428View(CLSKeithley428 *amplifier, QWidget *parent) :
    QWidget(parent)
{
    amplifier_ = 0;

    value_ = new QComboBox();
    connect( value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)) );

    units_ = new QLabel();

    QHBoxLayout *valueLayout = new QHBoxLayout();
    valueLayout->addWidget(value_);
    valueLayout->addWidget(units_);

    gainButton_ = new QRadioButton("Gain", this);
    connect( gainButton_, SIGNAL(clicked()), this, SLOT(onButtonClicked()) );

    sensitivityButton_ = new QRadioButton("Sensitivity", this);
    connect( sensitivityButton_, SIGNAL(clicked()), this, SLOT(onButtonClicked()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(gainButton_);
    buttonLayout->addWidget(sensitivityButton_);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(valueLayout);
    layout->addLayout(buttonLayout);

    setLayout(layout);

    setAmplifier(amplifier);
    refreshDisplayValues();
}

CLSKeithley428View::~CLSKeithley428View()
{

}

CLSKeithley428::AmplifierMode CLSKeithley428View::valueMode() const
{
    return amplifier_->amplifierMode();
}

CLSKeithley428* CLSKeithley428View::currentAmplifier() const
{
    return amplifier_;
}

void CLSKeithley428View::setValueMode(CLSKeithley428::AmplifierMode newMode)
{
    if (amplifier_) {
        amplifier_->setAmplifierMode(newMode);
        emit viewModeChanged();
    }
}

void CLSKeithley428View::setAmplifier(CLSKeithley428 *amplifier)
{
//    if (amplifier_) {
//        disconnect( amplifier_, SIGNAL(indexChanged(int)), this, SLOT(onValueChanged(int)) );
//        disconnect( amplifier_, SIGNAL(amplifierModeChanged(CLSKeithley428::AmplifierMode)), this, SLOT(refreshDisplayValues()) );
//    }

    amplifier_ = amplifier;

    if (amplifier_) {
        if (amplifier_->amplifierMode() == CLSKeithley428::Gain) {
            gainButton_->setChecked(true);

        } else {
            sensitivityButton_->setChecked(true);
        }

        connect( amplifier_, SIGNAL(indexChanged(int)), this, SLOT(onValueChanged(int)) );
        connect( amplifier_, SIGNAL(amplifierModeChanged(CLSKeithley428::AmplifierMode)), this, SLOT(refreshDisplayValues()) );
    }

    emit amplifierChanged();
}

void CLSKeithley428View::refreshDisplayValues()
{
    value_->clear();
    units_->clear();

    value_->addItems(amplifier_->valueStringList());
    units_->setText(amplifier_->units());
}

void CLSKeithley428View::onButtonClicked()
{
    if (amplifier_) {
        if (gainButton_->isChecked())
            amplifier_->setAmplifierMode(CLSKeithley428::Gain);

        else
            amplifier_->setAmplifierMode(CLSKeithley428::Sensitivity);
    }
}

void CLSKeithley428View::onValueComboBoxChanged(int newIndex)
{
    amplifier_->setValueIndex(newIndex);
}

void CLSKeithley428View::onValueChanged(int valueIndex)
{
    amplifier_->blockSignals(true);
    value_->setCurrentIndex(valueIndex);
    amplifier_->blockSignals(false);
}
