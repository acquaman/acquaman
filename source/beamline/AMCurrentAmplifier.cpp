#include "AMCurrentAmplifier.h"

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

bool AMCurrentAmplifier::inGainMode() const
{
    return (amplifierMode() == AMCurrentAmplifier::Gain);
}

bool AMCurrentAmplifier::inSensitivityMode() const
{
    return (amplifierMode() == AMCurrentAmplifier::Sensitivity);
}

bool AMCurrentAmplifier::setAmplifierMode(AmplifierMode newMode)
{
    if (newMode != amplifierMode_ && supports(newMode)) {
        amplifierMode_ = newMode;
        emit amplifierModeChanged(newMode);
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
