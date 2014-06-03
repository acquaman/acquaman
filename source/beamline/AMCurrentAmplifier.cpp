#include "AMCurrentAmplifier.h"

AMCurrentAmplifier::AMCurrentAmplifier(const QString &name, QObject *parent) :
    QObject(parent)
{
    name_ = name;
    connected_ = false;
    supportsGainMode_ = false;
    supportsSensitivityMode_ = false;
}

AMCurrentAmplifier::~AMCurrentAmplifier()
{

}

AMCurrentAmplifier::AmplifierMode AMCurrentAmplifier::amplifierMode() const
{
    return amplifierMode_;
}

bool AMCurrentAmplifier::supportsGainMode() const
{
    return supportsGainMode_;
}

bool AMCurrentAmplifier::atMinimumGain() const
{
    if (!supportsGainMode())
        return false;

    return atMinimumGainImplementation();
}

bool AMCurrentAmplifier::atMaximumGain() const
{
    if (!supportsGainMode())
        return false;

    return atMaximumGainImplementation();
}

bool AMCurrentAmplifier::supportsSensitivityMode() const
{
    return supportsSensitivityMode_;
}

bool AMCurrentAmplifier::atMinimumSensitivity() const
{
    if (!supportsSensitivityMode())
        return false;

    return atMinimumSensitivityImplementation();
}

bool AMCurrentAmplifier::atMaximumSensitivity() const
{
    if (!supportsSensitivityMode())
        return false;

    return atMaximumSensitivityImplementation();
}

void AMCurrentAmplifier::setAmplifierMode(AmplifierMode newMode)
{
    if (newMode != amplifierMode_) {
        amplifierMode_ = newMode;
        emit amplifierModeChanged(newMode);
    }
}

void AMCurrentAmplifier::setName(const QString &newName)
{
    name_ = newName;
    emit nameChanged(name_);
}

void AMCurrentAmplifier::setValue(int valueIndex)
{
    setValueImplementation(valueIndex);
}

bool AMCurrentAmplifier::increaseGain() const
{
    if (!supportsGainMode())
        return false;

    return increaseGainImplementation();
}

bool AMCurrentAmplifier::decreaseGain() const
{
    if (!supportsGainMode())
        return false;

    return decreaseGainImplementation();
}

bool AMCurrentAmplifier::increaseSensitivity() const
{
    if (!supportsSensitivityMode())
        return false;

    return increaseSensitivityImplementation();
}

bool AMCurrentAmplifier::decreaseSensitivity() const
{
    if (!supportsSensitivityMode())
        return false;

    return decreaseSensitivityImplementation();
}
