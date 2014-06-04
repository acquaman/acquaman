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
    return false;
}

bool AMCurrentAmplifier::atMaximumGain() const
{
    return false;
}

bool AMCurrentAmplifier::supportsSensitivityMode() const
{
    return supportsSensitivityMode_;
}

bool AMCurrentAmplifier::atMinimumSensitivity() const
{
    return false;
}

bool AMCurrentAmplifier::atMaximumSensitivity() const
{
    return false;
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

void AMCurrentAmplifier::setValue(double newValue)
{
    setValueIndexImplementation(newValue);
}

void AMCurrentAmplifier::setValueIndex(int valueIndex)
{
    setValueIndexImplementation(valueIndex);
}

bool AMCurrentAmplifier::increaseGain() const
{
    return false;
}

bool AMCurrentAmplifier::decreaseGain() const
{
    return false;
}

bool AMCurrentAmplifier::increaseSensitivity() const
{
    return false;
}

bool AMCurrentAmplifier::decreaseSensitivity() const
{
    return false;
}
