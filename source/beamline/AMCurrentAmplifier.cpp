/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMCurrentAmplifier.h"

#include "source/ui/beamline/AMCurrentAmplifierSingleView.h"

AMCurrentAmplifier::AMCurrentAmplifier(const QString &name, QObject *parent) :
    QObject(parent)
{
    name_ = name;
    connected_ = false;
    supportsGainMode_ = false;
    supportsSensitivityMode_ = false;

    setAmplifierMode(None);
}

AMCurrentAmplifier::~AMCurrentAmplifier()
{

}

AMCurrentAmplifier::AmplifierMode AMCurrentAmplifier::amplifierMode() const
{
    return amplifierMode_;
}

bool AMCurrentAmplifier::supports(AmplifierMode mode)
{
    bool support = false;

    if (mode == None)
        support = true;

    else if (mode == Gain && supportsGainMode_)
        support = true;

    else if (mode == Sensitivity && supportsSensitivityMode_)
        support = true;

    return support;
}

bool AMCurrentAmplifier::supportsGainMode() const
{
    return supportsGainMode_;
}

bool AMCurrentAmplifier::supportsSensitivityMode() const
{
    return supportsSensitivityMode_;
}

bool AMCurrentAmplifier::inGainMode() const
{
    return (amplifierMode() == AMCurrentAmplifier::Gain);
}

bool AMCurrentAmplifier::inSensitivityMode() const
{
    return (amplifierMode() == AMCurrentAmplifier::Sensitivity);
}

bool AMCurrentAmplifier::atMinimumValue() const
{
    if (inGainMode())
        return atMinimumGain();

    else if (inSensitivityMode())
        return atMinimumSensitivity();

    // this statement should never be reached.
    return true;
}

bool AMCurrentAmplifier::atMaximumValue() const
{
    if (inGainMode())
        return atMaximumGain();

    else if (inSensitivityMode())
        return atMaximumSensitivity();

    // this statement should never be reached.
    return true;
}

AMCurrentAmplifierSingleView* AMCurrentAmplifier::createView()
{
    return new AMCurrentAmplifierSingleView(this);
}

bool AMCurrentAmplifier::setAmplifierMode(AmplifierMode newMode)
{
    if (newMode != amplifierMode_ && supports(newMode)) {
        amplifierMode_ = newMode;
        emit amplifierModeChanged();
        return true;
    }

    return false;
}

bool AMCurrentAmplifier::setToGainMode()
{
    return setAmplifierMode(AMCurrentAmplifier::Gain);
}

bool AMCurrentAmplifier::setToSensitivityMode()
{
    return setAmplifierMode(AMCurrentAmplifier::Sensitivity);
}

void AMCurrentAmplifier::setName(const QString &newName)
{
    name_ = newName;
    emit nameChanged(name_);
}

void AMCurrentAmplifier::setValue(const QString &valueArg)
{
    setValueImplementation(valueArg);
}

bool AMCurrentAmplifier::increaseValue()
{
    if (amplifierMode_ == Gain) {
        return increaseGain();

    } else if (amplifierMode_ == Sensitivity) {
        return increaseSensitivity();

    } else {
        return false;
    }
}

bool AMCurrentAmplifier::decreaseValue()
{
    if (amplifierMode_ == Gain) {
        return decreaseGain();

    } else if (amplifierMode_ == Sensitivity) {
        return decreaseSensitivity();

    } else {
        return false;
    }
}

bool AMCurrentAmplifier::atMinimumGain() const
{
    return false;
}

bool AMCurrentAmplifier::atMaximumGain() const
{
    return false;
}

bool AMCurrentAmplifier::atMinimumSensitivity() const
{
    return false;
}

bool AMCurrentAmplifier::atMaximumSensitivity() const
{
    return false;
}

bool AMCurrentAmplifier::increaseGain()
{
    return false;
}

bool AMCurrentAmplifier::decreaseGain()
{
    return false;
}

bool AMCurrentAmplifier::increaseSensitivity()
{
    return false;
}

bool AMCurrentAmplifier::decreaseSensitivity()
{
    return false;
}
