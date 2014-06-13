#include "CLSKeithley428.h"

#include "math.h"

#include <QDebug>

CLSKeithley428::CLSKeithley428(const QString &name, const QString &valueName, QObject *parent) :
    AMCurrentAmplifier(name, parent)
{
    supportsSensitivityMode_ = true;
    supportsGainMode_ = true;

    valueControl_ = new AMProcessVariable(valueName, true, this);
    connect( valueControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedStateChanged(bool)) );
    connect( valueControl_, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)) );

    // populate gains list in increasing order.
    gains_ << pow(10, 3) << pow(10, 4) << pow(10, 5) << pow(10, 6) << pow(10, 7) << pow(10, 8) << pow(10, 9) << pow(10, 10);
    units_ = "V/A";

    setAmplifierMode(AMCurrentAmplifier::Gain);
}

CLSKeithley428::~CLSKeithley428()
{

}

double CLSKeithley428::value() const
{
    return valueAt(valueControl_->getInt());
}

double CLSKeithley428::valueAt(int index) const
{
    double value = -1;

    if (amplifierMode_ == Gain)
        value = gains_.at(index);
    else if (amplifierMode_ == Sensitivity)
        value = toSensitivity(gains_.at(index));

    return value;
}

int CLSKeithley428::valueIndex() const
{
    return valueControl_->getInt();
}

QList<double> CLSKeithley428::values() const
{
    QList<double> values;

    if (amplifierMode_ == Gain) {
        values = gains_;

    } else if (amplifierMode_ == Sensitivity) {
        QList<double> sensitivities;

        foreach (double gain, gains_) {
            sensitivities.append(toSensitivity(gain));
        }

        values = sensitivities;
    }

    return values;
}

QString CLSKeithley428::units() const
{
    if (amplifierMode_ == Gain)
        return units_;

    else if (amplifierMode_ == Sensitivity)
        return "A/V";

    else
        return "";
}

QStringList CLSKeithley428::unitsList() const
{
    return QStringList() << units();
}

bool CLSKeithley428::atMaximumGain() const
{
    return (valueControl_->getInt() == gains_.size() - 1);
}

bool CLSKeithley428::atMinimumGain() const
{
    return (valueControl_->getInt() == 0);
}

bool CLSKeithley428::atMaximumSensitivity() const
{
    return atMinimumGain();
}

bool CLSKeithley428::atMinimumSensitivity() const
{
    return atMaximumGain();
}

void CLSKeithley428::setValueIndex(int newIndex)
{   
    if (newIndex >= 0 && newIndex < gains_.size() && newIndex != valueControl_->getInt())
        valueControl_->setValue(newIndex);
}

bool CLSKeithley428::increaseGain()
{
    // Don't do anything if the value is already at a maximum.
    if (atMaximumGain())
        return false;

    int currentIndex = valueControl_->getInt();

    // The values are listed in order of increasing gain. Increasing index = increasing gain.
    setValueIndex(currentIndex + 1);

    return true;
}

bool CLSKeithley428::decreaseGain()
{
    // Don't do anything if the value is already at a minimum.
    if (atMinimumGain()) {
        return false;
    }

    int currentIndex = valueControl_->getInt();

    // The values are listed in order of increasing gain. Decreasing index = decreasing gain.
    setValueIndex(currentIndex - 1);

    return true;
}

bool CLSKeithley428::increaseSensitivity()
{
    if (atMaximumSensitivity()) {
        return false;
    }

    // Decreasing index = decreasing gain = increasing sensitivity.
    decreaseGain();

    return true;
}

bool CLSKeithley428::decreaseSensitivity()
{
    if (atMinimumSensitivity()) {
        return false;
    }

    // Increasing index = increasing gain = decreasing sensitivity.
    increaseGain();

    return true;
}

void CLSKeithley428::onValueChanged(int newIndex)
{
    Q_UNUSED(newIndex)

    emit valueChanged();

    if (atMinimumValue())
        emit minimumValue(true);
    else
        emit minimumValue(false);

    if (atMaximumValue())
        emit maximumValue(true);
    else
        emit maximumValue(false);
}

void CLSKeithley428::onConnectedStateChanged(bool connectState)
{
    if (connected_ != connectState) {
        connected_ = connectState;
        emit isConnected(connected_);
    }
}

double CLSKeithley428::toSensitivity(double gain) const
{
    return 1.0/gain;
}

double CLSKeithley428::toGain(double sensitivity) const
{
    return 1.0/sensitivity;
}

void CLSKeithley428::setValueImplementation(const QString &valueArg)
{
    double newValue = valueArg.split(" ").at(0).toDouble();

    if (amplifierMode_ == Gain) {
        if (gains_.contains(newValue)) {
            setValueIndex(gains_.indexOf(newValue));
        }

    } else if (amplifierMode_ == Sensitivity) {
        double newGain = toGain(newValue);
        if (gains_.contains(newGain)) {
            setValueIndex(gains_.indexOf(newGain));
        }
    }
}
