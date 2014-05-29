#include "CLSKeithley428.h"

#include "math.h"

#include <QDebug>

CLSKeithley428::CLSKeithley428(const QString &name, const QString &valueName, QObject *parent) :
    QObject(parent)
{
    connect( this, SIGNAL(atMaximumValue(bool)), this, SLOT(onMaximumGain()) );
    connect( this, SIGNAL(atMinimumValue(bool)), this, SLOT(onMinimumGain()) );

    isConnected_ = false;

    name_ = name;

    valueControl_ = new AMProcessVariable(valueName, true, this);
    connect( valueControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedStateChanged(bool)) );
    connect( valueControl_, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)) );

    valueMap_ = new CLSKeithley428ValueMap();
    setValueMap();

}

CLSKeithley428::~CLSKeithley428()
{

}

double CLSKeithley428::value()
{
    return valueMap_->valueAt(valueControl_->getInt(), CLSKeithley428ValueMap::Gain);
}

int CLSKeithley428::index()
{
    return valueControl_->getInt();
}

QString CLSKeithley428::units(CLSKeithley428ValueMap::ValueMode mode) const
{
    if (mode == CLSKeithley428ValueMap::Gain) {
        return "V/A";

    } else if (mode == CLSKeithley428ValueMap::Sensitivity) {
        return "A/V";

    } else {
        return "";
    }
}

bool CLSKeithley428::isConnected()
{
    return isConnected_;
}

CLSKeithley428ValueMap* CLSKeithley428::valueMap() const
{
    return valueMap_;
}

bool CLSKeithley428::atMinimumGain() const
{
    return valueMap_->isMinIndex(CLSKeithley428ValueMap::Gain, valueControl_->getInt());
}

bool CLSKeithley428::atMaximumGain() const
{
    return valueMap_->isMaxIndex(CLSKeithley428ValueMap::Gain, valueControl_->getInt());
}

void CLSKeithley428::setValueIndex(int valueIndex)
{
    // check that the given index corresponds to a gain value, then set the value control.
    if (valueMap_->map()->contains(valueIndex)) {
        valueControl_->setValue(valueIndex);
    }

    if (atMaximumGain()) {
        emit atMaximumGain();
    }

    if (atMinimumGain()) {
        emit atMinimumGain();
    }
}

bool CLSKeithley428::increaseGain()
{
    // Don't do anything if the gain is already at a maximum.
    if (atMaximumGain())
        return false;

    // Increase gain!
    int newIndex = nextIndex(IncreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

bool CLSKeithley428::decreaseGain()
{
    // Don't do anything if the gain is already at a minimum.
    if (atMinimumGain()) {
        return false;
    }

    // Decrease gain!
    int newIndex = nextIndex(DecreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

void CLSKeithley428::onValueChanged(int newIndex)
{
    Q_UNUSED(newIndex)

    emit valueChanged(value());
    emit indexChanged(index());

    if (atMaximumGain()) {
        emit atMaximumValue(true);
    }

    if (atMinimumGain()) {
        emit atMinimumValue(true);
    }
}

void CLSKeithley428::onConnectedStateChanged(bool isConnected)
{
    if (isConnected != isConnected_) {
        isConnected_ = isConnected;
        qDebug() << "New connection state, is connected : " << isConnected_;
        emit connected(isConnected_);
    }
}

void CLSKeithley428::onMaximumGain()
{
    qDebug() << "Maximum gain.";
}

void CLSKeithley428::onMinimumGain()
{
    qDebug() << "Minimum gain.";
}

int CLSKeithley428::nextIndex(IndexChange change, int currentIndex)
{
    int nextIndex = -1;

    if (valueMap_->map()->contains(currentIndex)) {
        qDebug() << "Invalid initial index.";
        nextIndex = -1;
    }

    if (change == IncreaseOne && atMaximumGain()) {
        qDebug() << "At max index.";
        nextIndex = currentIndex;

    } else if (change == IncreaseOne) {
        nextIndex = currentIndex++;

    } else if (change == DecreaseOne && atMinimumGain()) {
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
    valueMap_->addValue(0, CLSKeithley428ValueMap::Gain, pow(10, 3));
    valueMap_->addValue(1, CLSKeithley428ValueMap::Gain, pow(10, 4));
    valueMap_->addValue(2, CLSKeithley428ValueMap::Gain, pow(10, 5));
    valueMap_->addValue(3, CLSKeithley428ValueMap::Gain, pow(10, 6));
    valueMap_->addValue(4, CLSKeithley428ValueMap::Gain, pow(10, 7));
    valueMap_->addValue(5, CLSKeithley428ValueMap::Gain, pow(10, 8));
    valueMap_->addValue(6, CLSKeithley428ValueMap::Gain, pow(10, 9));
    valueMap_->addValue(7, CLSKeithley428ValueMap::Gain, pow(10, 10));
}
