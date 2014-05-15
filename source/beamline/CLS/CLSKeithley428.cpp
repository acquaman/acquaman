#include "CLSKeithley428.h"

#include <QDebug>

CLSKeithley428::CLSKeithley428(const QString &name, const QString &valueName, const QString &unitsName, QObject *parent) :
    AMCurrentAmplifier(name, AMCurrentAmplifier::Gain, parent)
{
    atMinimumGain_ = true;
    atMaximumGain_ = false;

    valueControl_ = new AMProcessVariable(valueName, true, this);
    connect( valueControl_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)) );

    unitsControl_ = new AMProcessVariable(unitsName, true, this);
    connect( unitsControl_, SIGNAL(valueChanged(QString)), this, SLOT(onUnitsChanged(QString)) );
}

CLSKeithley428::~CLSKeithley428()
{

}

int CLSKeithley428::value()
{
    return indexToValue(valueControl_->getInt());
}

int CLSKeithley428::valueIndex() const
{
    return valueControl_->getInt();
}

QString CLSKeithley428::units() const
{
    return unitsControl_->getString();
}


bool CLSKeithley428::atMaximumValue() const
{
    return atMaximumGain_;
}

bool CLSKeithley428::atMinimumValue() const
{
    return atMinimumGain_;
}

void CLSKeithley428::setValue(int newValue)
{
    valueControl_->setValue(valueToIndex(newValue));
}

void CLSKeithley428::setUnits(QString newUnits)
{
    if (unitsOkay(newUnits)) {
        unitsControl_->setValue(newUnits);
    }
}

bool CLSKeithley428::increaseValue()
{
    // Don't do anything if the gain is already at a maximum.
    if (atMaximumGain_)
        return false;

    // Increase gain!

    return true;
}

bool CLSKeithley428::decreaseValue()
{
    // Don't do anything if the gain is already at a minimum.
    if (atMinimumGain_)
        return false;

    // Decrease gain!

    return true;
}

void CLSKeithley428::onValueChanged(int newIndex)
{
    qDebug() << "Gain value index changed : " << newIndex;
    emit gainChanged(indexToValue(newIndex));
}

void CLSKeithley428::onUnitsChanged(QString newUnits)
{
    qDebug() << "Gain units changed : " << newUnits;
    emit unitsChanged(newUnits);
}

int CLSKeithley428::valueToIndex(int value) const
{
    return 0;
}

int CLSKeithley428::indexToValue(int index) const
{
    return 0;
}

bool CLSKeithley428::indexOkay(int index)
{
    if (index > 0 && index <= 8)
        return true;
    else
        return false;
}

bool CLSKeithley428::unitsOkay(QString units)
{
    if (units == "V/A")
        return true;
    else
        return false;
}
