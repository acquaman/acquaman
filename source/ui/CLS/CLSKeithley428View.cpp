#include "CLSKeithley428View.h"

#include <QDebug>

CLSKeithley428ContainerView::CLSKeithley428ContainerView(CLSKeithley428 *amplifier, QWidget *parent)
    : QTabWidget(parent)
{
    gainTab_ = new CLSKeithley428View(amplifier, CLSKeithley428ValueMap::Gain, this);
    sensitivityTab_ = new CLSKeithley428View(amplifier, CLSKeithley428ValueMap::Sensitivity, this);

    addTab(gainTab_, "Gain");
    addTab(sensitivityTab_, "Sensitivity");
}

CLSKeithley428ContainerView::~CLSKeithley428ContainerView()
{

}

CLSKeithley428View::CLSKeithley428View(CLSKeithley428 *amplifier, CLSKeithley428ValueMap::ValueMode mode, QWidget *parent) :
    QWidget(parent)
{
    amplifier_ = amplifier;
    connect( amplifier_, SIGNAL(indexChanged(int)), this, SLOT(onValueChanged(int)) );

    value_ = new QComboBox();
    connect( value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)) );

    units_ = new QLabel();

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(value_);
    layout->addWidget(units_);

    setLayout(layout);
    setValueMode(mode);
}

CLSKeithley428View::~CLSKeithley428View()
{

}

CLSKeithley428ValueMap::ValueMode CLSKeithley428View::valueMode() const
{
    return valueMode_;
}

CLSKeithley428* CLSKeithley428View::currentAmplifier() const
{
    return amplifier_;
}

void CLSKeithley428View::setValueMode(CLSKeithley428ValueMap::ValueMode newValueMode)
{
    valueMode_ = newValueMode;

    value_->clear();
    units_->clear();

    value_->addItems(amplifier_->valueMap()->valueStringList(valueMode_));
    units_->setText(amplifier_->units(valueMode_));

    emit valueModeChanged(valueMode_);
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

void CLSKeithley428View::onUnitsChanged(CLSKeithley428ValueMap::ValueMode mode, const QString &units)
{
    if (mode == valueMode_) {
        units_->setText(units);
    }
}
