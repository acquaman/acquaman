#include "CLSKeithley428.h"

CLSKeithley428::CLSKeithley428(const QString &name, QObject *parent) :
    AMCurrentAmplifier(name, parent)
{
    atMaximumSensitivity_ = false;
    atMinimumSensitivity_ = true;
}

CLSKeithley428::~CLSKeithley428()
{

}

bool CLSKeithley428::atMaximumSensitivity() const
{
    return atMaximumSensitivity_;
}

bool CLSKeithley428::atMinimumSensitivity() const
{
    return atMinimumSensitivity_;
}

bool CLSKeithley428::increaseSensitivity()
{
    // Don't do anything if we have already reached max sensitivity.
    if (atMaximumSensitivity_) {
        return false;
    }

    // Increase sensitivity.

    return true;
}

bool CLSKeithley428::decreaseSensitivity()
{
    // Don't do anything if the minimum sensitivity has already been reached.
    if (atMinimumSensitivity_) {
        return false;
    }

    // Decrease sensitivity.

    return true;
}
