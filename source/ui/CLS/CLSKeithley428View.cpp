//#include "CLSKeithley428View.h"

//#include <QDebug>

//CLSKeithley428View::CLSKeithley428View(CLSKeithley428 *amplifier, QWidget *parent) :
//    QWidget(parent)
//{
//    amplifier_ = 0;

//    value_ = new QComboBox();
//    connect( value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)) );

//    units_ = new QComboBox();
//    connect( units_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsComboBoxChanged(int)) );

//    QHBoxLayout *valueLayout = new QHBoxLayout();
//    valueLayout->addWidget(value_);
//    valueLayout->addWidget(units_);

//    gainButton_ = new QRadioButton("Gain", this);
//    connect( gainButton_, SIGNAL(clicked()), this, SLOT(onButtonClicked()) );

//    sensitivityButton_ = new QRadioButton("Sensitivity", this);
//    connect( sensitivityButton_, SIGNAL(clicked()), this, SLOT(onButtonClicked()) );

//    QHBoxLayout *buttonLayout = new QHBoxLayout();
//    buttonLayout->addWidget(gainButton_);
//    buttonLayout->addWidget(sensitivityButton_);

//    QVBoxLayout *layout = new QVBoxLayout();
//    layout->addLayout(valueLayout);
//    layout->addLayout(buttonLayout);

//    setLayout(layout);

//    setAmplifier(amplifier);
//    refreshDisplayValues();
//}

//CLSKeithley428View::~CLSKeithley428View()
//{

//}

//CLSKeithley428::AmplifierMode CLSKeithley428View::valueMode() const
//{
//    return amplifier_->amplifierMode();
//}

//void CLSKeithley428View::setAmplifierMode(AMCurrentAmplifier::AmplifierMode newMode)
//{
//    if (amplifier_) {
//        amplifier_->setAmplifierMode(newMode);
//    }
//}

//void CLSKeithley428View::setAmplifier(CLSKeithley428 *amplifier)
//{
//    if (amplifier_) {
//        disconnect( amplifier_, SIGNAL(isConnected(bool)), this, SLOT(refreshDisplayValues()) );
//        disconnect( amplifier_, SIGNAL(indexChanged(int)), this, SLOT(onValueChanged(int)) );
//        disconnect( amplifier_, SIGNAL(amplifierModeChanged(AMCurrentAmplifier::AmplifierMode)), this, SLOT(refreshDisplayValues()) );
//    }

//    amplifier_ = amplifier;

//    if (amplifier_) {
//        if (amplifier_->amplifierMode() == AMCurrentAmplifier::Gain) {
//            gainButton_->setChecked(true);

//        } else {
//            sensitivityButton_->setChecked(true);
//        }

//        connect( amplifier_, SIGNAL(isConnected(bool)), this, SLOT(refreshDisplayValues()) );
//        connect( amplifier_, SIGNAL(indexChanged(int)), this, SLOT(onValueChanged(int)) );
//        connect( amplifier_, SIGNAL(amplifierModeChanged(AmplifierMode)), this, SLOT(refreshDisplayValues()) );
//    }

//    refreshDisplayValues();
//    emit amplifierChanged(amplifier_);
//}

//void CLSKeithley428View::refreshDisplayValues()
//{
//    value_->clear();
//    units_->clear();

//    if (amplifier_) {
//        value_->addItems(amplifier_->valuesList());
//        value_->setCurrentIndex(amplifier_->valueIndex());
//        units_->addItems(amplifier_->unitsList());
//    }
//}

//void CLSKeithley428View::onButtonClicked()
//{
//    if (amplifier_) {
//        if (gainButton_->isChecked())
//            amplifier_->setAmplifierMode(AMCurrentAmplifier::Gain);

//        else
//            amplifier_->setAmplifierMode(AMCurrentAmplifier::Sensitivity);
//    }
//}

//void CLSKeithley428View::onValueComboBoxChanged(int newIndex)
//{
////    amplifier_->setValueIndex(newIndex);
//}

//void CLSKeithley428View::onUnitsComboBoxChanged(int index)
//{
//    Q_UNUSED(index)
//}

//void CLSKeithley428View::onValueChanged(int valueIndex)
//{
//    amplifier_->blockSignals(true);
//    value_->setCurrentIndex(valueIndex);
//    amplifier_->blockSignals(false);
//}

//void CLSKeithley428View::onUnitsChanged(QString units)
//{
//    Q_UNUSED(units)
//}
