#include "CLSKeithley428.h"

#include <QDebug>

CLSKeithley428::CLSKeithley428(const QString &name, const QString &valueName, const QString &unitsName, QObject *parent) :
    AMCurrentAmplifier(name, parent)
{
    atMinimumGain_ = true;
    atMaximumGain_ = false;

    valueControl_ = new AMProcessVariable(valueName, true, this);
    connect( valueControl_, SIGNAL(valueChanged(double)), this, SLOT(onGainChanged(double)) );

    unitsControl_ = new AMProcessVariable(unitsName, true, this);

}

CLSKeithley428::~CLSKeithley428()
{

}

bool CLSKeithley428::atMaximumSensitivity() const
{
    return atMinimumGain();
}

bool CLSKeithley428::atMinimumSensitivity() const
{
    return atMaximumGain();
}

bool CLSKeithley428::increaseSensitivity()
{
    return decreaseGain();
}

bool CLSKeithley428::decreaseSensitivity()
{
    return increaseGain();
}

void CLSKeithley428::onGainChanged(double newVal)
{
    qDebug() << "Gain changed : " << newVal;
}

bool CLSKeithley428::decreaseGain()
{
    // Don't do anything if the gain is already at a minimum.
    if (atMinimumGain_)
        return false;

    // Decrease gain!

    return true;

}

bool CLSKeithley428::increaseGain()
{
    // Don't do anything if the gain is already at a maximum.
    if (atMaximumGain_)
        return false;

    // Increase gain!

    return true;
}

bool CLSKeithley428::atMinimumGain() const
{
    return atMinimumGain_;
}

bool CLSKeithley428::atMaximumGain() const
{
    return atMaximumGain_;
}
