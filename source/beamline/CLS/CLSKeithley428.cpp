#include "CLSKeithley428.h"

#include "math.h"

#include <QDebug>

CLSKeithley428::CLSKeithley428(const QString &name, const QString &valueName, QObject *parent) :
    QObject(parent)
{
    atMinimumGain_ = false;
    atMaximumGain_ = false;

    valueControl_ = new AMProcessVariable(valueName, true, this);
    connect( valueControl_, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)) );

//    if (unitsName != "") {
//        unitsControl_ = new AMProcessVariable(unitsName, true, this);
//        connect( unitsControl_, SIGNAL(valueChanged(QString)), this, SLOT(onUnitsChanged(QString)) );
//    } else {
//        unitsControl_ = 0;
//    }

    gainUnits_ = "V/A";
}

CLSKeithley428::~CLSKeithley428()
{

}

double CLSKeithley428::value()
{
    return sensitivity();
}

QString CLSKeithley428::units() const
{
    return "A/V";
}

bool CLSKeithley428::atMaximumSensitivity() const
{
    return atMinimumGain();
}

bool CLSKeithley428::atMinimumSensitivity() const
{
    return atMaximumGain();
}

void CLSKeithley428::setGainValue(int newValue)
{
    int newIndex = expToIndex(valueToExp(newValue));

    if (indexOkay(newIndex)) {
        valueControl_->setValue(newIndex);
        emit valueChanged(gain());
        emit valueIndexChanged(gainIndex());
    }
}

void CLSKeithley428::setGainIndex(int newIndex)
{
    if (indexOkay(newIndex)) {
        valueControl_->setValue(newIndex);
        emit valueChanged(gain());
        emit valueIndexChanged(gainIndex());
    }
}

void CLSKeithley428::setUnits(QString newUnits)
{
    if (unitsOkay(newUnits)) {
        gainUnits_ = newUnits;
        emit unitsChanged(gainUnits_);
    }
}

bool CLSKeithley428::increaseSensitivity()
{
    return decreaseGain();
}

bool CLSKeithley428::decreaseSensitivity()
{
    return increaseGain();
}

bool CLSKeithley428::increaseGain()
{
    // Don't do anything if the gain is already at a maximum.
    if (atMaximumGain_)
        return false;

    // Increase gain!
    int newIndex = nextIndex(IncreaseOne, valueControl_->getInt());
    setGainIndex(newIndex);

    return true;
}

bool CLSKeithley428::decreaseGain()
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
    emit gainChanged();
    emit valueIndexChanged(gainIndex());
}

void CLSKeithley428::onUnitsChanged(QString newUnits)
{
    qDebug() << "Gain units changed : " << newUnits;
    emit unitsChanged(newUnits);
}

double CLSKeithley428::gain()
{
    int exp = indexToExp(valueControl_->getInt());
    return expToValue(exp);
}

QString CLSKeithley428::gainUnits() const
{
    return gainUnits_;
}

int CLSKeithley428::gainIndex() const
{
    return valueControl_->getInt();
}

double CLSKeithley428::sensitivity()
{
    return 1/gain();
}

int CLSKeithley428::expToIndex(int exp) const
{
    int index = -1;

    switch (exp) {
    case 3:
        index = 0;
        break;
    case 4:
        index = 1;
        break;
    case 5:
        index = 2;
        break;
    case 6:
        index = 3;
        break;
    case 7:
        index = 4;
        break;
    case 8:
        index = 5;
        break;
    case 9:
        index = 6;
        break;
    case 10:
        index = 7;
        break;
    }

    return index;
}

int CLSKeithley428::indexToExp(int index) const
{
    int exp = -1;

    switch (index) {
    case 0:
        exp = 3;
        break;
    case 1:
        exp = 4;
        break;
    case 2:
        exp = 5;
        break;
    case 3:
        exp = 6;
        break;
    case 4:
        exp = 7;
        break;
    case 5:
        exp = 8;
        break;
    case 6:
        exp = 9;
        break;
    case 7:
        exp = 10;
        break;
    }

    return exp;
}

double CLSKeithley428::expToValue(int exp)
{
    return 10^exp;
}

int CLSKeithley428::valueToExp(double value)
{
    return log10(value);
}

int CLSKeithley428::nextIndex(IndexChange change, int currentIndex)
{
    if (!indexOkay(currentIndex))
        return -1;

    if (change == IncreaseOne) {
        if (!atMaximumGain_) {
            return currentIndex + 1;
        } else {
            return -1;
        }

    } else if (change == DecreaseOne) {
        if (!atMinimumGain_) {
            return currentIndex - 1;
        } else {
            return -1;
        }

    } else {
        qDebug() << "CLSKeithley428 : Unknown change discovered--neither IncreaseOne nor DecreaseOne.";
        return -1;
    }
}

bool CLSKeithley428::atMinimumGain() const
{
    return atMinimumGain_;
}

bool CLSKeithley428::atMaximumGain() const
{
    return atMaximumGain_;
}

bool CLSKeithley428::valueOkay(int value)
{
    if (exponentOkay(valueToExp(value)))
        return true;
    else
        return false;
}

bool CLSKeithley428::indexOkay(int index)
{
    if (index >=0 && index <= 7)
        return true;
    else
        return false;
}

bool CLSKeithley428::exponentOkay(int exp)
{
    if (exp >= 3 && exp <= 10)
        return true;
    else
        return false;
}

bool CLSKeithley428::unitsOkay(QString &units)
{
    if (units == "V/A")
        return true;
    else
        return false;
}
