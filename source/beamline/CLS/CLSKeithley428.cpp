#include "CLSKeithley428.h"
#include "beamline/CLS/CLSKeithley428ValueMap.h"

#include "math.h"

#include <QDebug>

CLSKeithley428::CLSKeithley428(const QString &name, const QString &valueName, QObject *parent) :
    AMCurrentAmplifier(name, parent)
{
    connect( this, SIGNAL(maximumValue(bool)), this, SLOT(onMaximumValue()) );
    connect( this, SIGNAL(minimumValue(bool)), this, SLOT(onMinimumValue()) );

    valueControl_ = new AMProcessVariable(valueName, true, this);
    connect( valueControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedStateChanged(bool)) );
    connect( valueControl_, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)) );

    valueMap_ = new CLSKeithley428ValueMap();

    setAmplifierMode(Gain);
    setValueMap();
}

CLSKeithley428::~CLSKeithley428()
{

}

CLSKeithley428::AmplifierMode CLSKeithley428::amplifierMode() const
{
    return amplifierMode_;
}

double CLSKeithley428::value()
{
    return valueMap_->valueAt(index(), amplifierMode_);
}

int CLSKeithley428::index()
{
    return valueControl_->getInt();
}

QString CLSKeithley428::units() const
{
    if (amplifierMode_ == Gain) {
        return "V/A";

    } else if (amplifierMode_ == Sensitivity) {
        return "A/V";

    } else {
        return "";
    }
}

CLSKeithley428ValueMap* CLSKeithley428::valueMap() const
{
    return valueMap_;
}

bool CLSKeithley428::atMinimumSensitivity() const
{
    bool min = false;

    if (amplifierMode_ == Sensitivity)
        min = atMinimumValue();
    else
        min = atMaximumValue();

    return min;
}

bool CLSKeithley428::atMaximumSensitivity() const
{
    bool max = false;

    if (amplifierMode_ == Sensitivity)
        max = atMaximumValue();
    else
        max = atMinimumValue();

    return max;
}

bool CLSKeithley428::atMaximumValue() const
{
    return valueMap_->isMaxIndex(amplifierMode_, valueControl_->getInt());
}

bool CLSKeithley428::atMinimumValue() const
{
    return valueMap_->isMinIndex(amplifierMode_, valueControl_->getInt());
}

QStringList CLSKeithley428::valueStringList()
{
    return valueMap_->valueStringList(amplifierMode_);
}

//QStringList CLSKeithley428::unitsStringList()
//{
//    return
//}

void CLSKeithley428::setAmplifierMode(AmplifierMode newMode)
{
    if (newMode != amplifierMode_) {
        amplifierMode_ = newMode;
        emit amplifierModeChanged(newMode);
    }
}

void CLSKeithley428::setValueIndex(int valueIndex)
{
    // check that the given index corresponds to a value, then set the value control.
    if (valueMap_->map()->contains(valueIndex)) {
        valueControl_->setValue(valueIndex);
    }
}

bool CLSKeithley428::increaseSensitivity()
{
    return false;
}

bool CLSKeithley428::decreaseSensitivity()
{
    return false;
}

bool CLSKeithley428::increaseValue()
{
    // Don't do anything if the value is already at a maximum.
    if (atMaximumValue())
        return false;

    // Increase value
    int newIndex = nextIndex(IncreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

bool CLSKeithley428::decreaseValue()
{
    // Don't do anything if the value is already at a minimum.
    if (atMinimumValue()) {
        return false;
    }

    // Decrease value
    int newIndex = nextIndex(DecreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

void CLSKeithley428::onValueChanged(int newIndex)
{
    emit valueChanged();
    emit indexChanged(newIndex);

    if (atMaximumValue()) {
        emit maximumValue(true);
    }

    if (atMinimumValue()) {
        emit minimumValue(true);
    }
}

void CLSKeithley428::onConnectedStateChanged(bool connectState)
{
    if (connected_ != connectState) {
        connected_ = connectState;
        emit isConnected(connected_);
    }
}

void CLSKeithley428::onMaximumValue()
{
    qDebug() << "Maximum value.";
}

void CLSKeithley428::onMinimumValue()
{
    qDebug() << "Minimum value.";
}

int CLSKeithley428::nextIndex(IndexChange change, int currentIndex)
{
    int nextIndex = -1;

    if (!valueMap_->map()->contains(currentIndex)) {
        qDebug() << "Invalid initial index.";
        nextIndex = -1;
    }

    if (change == IncreaseOne && atMaximumValue()) {
        qDebug() << "At max index.";
        nextIndex = currentIndex;

    } else if (change == IncreaseOne) {
        nextIndex = currentIndex++;

    } else if (change == DecreaseOne && atMinimumValue()) {
        qDebug() << "At min index.";
        nextIndex = currentIndex;

    } else if (change == DecreaseOne) {
        nextIndex = currentIndex--;

    } else {
        qDebug() << "CLSKeithley428 : Unknown change discovered--neither IncreaseOne nor DecreaseOne.";
        nextIndex = currentIndex;
    }

    return nextIndex;
}

void CLSKeithley428::setValueMap()
{
    valueMap_->map()->clear();
    valueMap_->addValue(0, amplifierMode_, pow(10, 3));
    valueMap_->addValue(1, amplifierMode_, pow(10, 4));
    valueMap_->addValue(2, amplifierMode_, pow(10, 5));
    valueMap_->addValue(3, amplifierMode_, pow(10, 6));
    valueMap_->addValue(4, amplifierMode_, pow(10, 7));
    valueMap_->addValue(5, amplifierMode_, pow(10, 8));
    valueMap_->addValue(6, amplifierMode_, pow(10, 9));
    valueMap_->addValue(7, amplifierMode_, pow(10, 10));
}
