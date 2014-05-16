#include "CLSKeithley428.h"

#include <QDebug>

CLSKeithley428::CLSKeithley428(const QString &name, const QString &valueName, const QString &unitsName, QObject *parent) :
    AMCurrentAmplifier(name, parent)
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

bool CLSKeithley428::increaseSensitivity()
{
    // Don't do anything if the gain is already at a maximum.
    if (atMaximumGain_)
        return false;

    // Increase gain!
    int currentValue = valueControl_->getInt();
//    int nextValue = nextValue()

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
    emit sensitivityChanged();
}

void CLSKeithley428::onUnitsChanged(QString newUnits)
{
    qDebug() << "Gain units changed : " << newUnits;
    emit unitsChanged(newUnits);
}

int CLSKeithley428::valueToIndex(int value) const
{
    int index = -1;

//    switch (value) {
//    case 10000:
//        index = 0;
//        break;
//    case 100000:
//        index = 1;
//        break;
//    case 1000000:
//        index = 2;
//        break;
//    case 10000000:
//        index = 3;
//        break;
//    case 100000000:
//        index = 4;
//        break;
//    case 1000000000:
//        index = 5;
//        break;
//    case 10000000000:
//        index = 6;
//        break;
//    case 100000000000:
//        index = 7;
//        break;
//    }

    return index;
}

int CLSKeithley428::indexToValue(int index) const
{
    int value = -1;

//    switch (index) {
//    case 0:
//        value = 10000;
//        break;
//    case 1:
//        value = 100000;
//        break;
//    case 2:
//        value = 1000000;
//        break;
//    case 3:
//        value = 10000000;
//        break;
//    case 4:
//        value = 100000000;
//        break;
//    case 5:
//        value = 1000000000;
//        break;
//    case 6:
//        value = 10000000000;
//        break;
//    case 7:
//        value = 100000000000;
//        break;
//    }

    return value;
}

bool CLSKeithley428::indexOkay(int index)
{
    if (index >= 0 && index < 8)
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

int CLSKeithley428::nextValue(CLSKeithley428::ValueChange change, int currentValue)
{

}
