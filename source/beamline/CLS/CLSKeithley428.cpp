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


#include "CLSKeithley428.h"

#include "math.h"

CLSKeithley428::CLSKeithley428(const QString &name, const QString &baseName, QObject *parent) :
    AMCurrentAmplifier(name, parent)
{
    supportsSensitivityMode_ = true;
    supportsGainMode_ = true;

    valueControl_ = new AMProcessVariable(baseName+":Gain", true, this);
    biasVoltageEnabled_ = new AMPVControl("BiasVoltageEnabled", baseName+":BiasEnable:fbk", baseName+":BiasEnable", QString(), this);
    biasVoltage_ = new AMPVControl("BiasVoltage", baseName+":BiasVoltage:fbk", baseName+":BiasValue", QString(), this);

    // Listen for changes to connect states.

    connect( valueControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedStateChanged(bool)) );
    connect( biasVoltageEnabled_, SIGNAL(connected(bool)), this, SLOT(onConnectedStateChanged(bool)) );
    connect( biasVoltage_, SIGNAL(connected(bool)), this, SLOT(onConnectedStateChanged(bool)) );

    // Listen for value changes.

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

double CLSKeithley428::minimumValueForUnits(const QString &units) const
{
    Q_UNUSED(units)

    double value;

    if (amplifierMode_ == Sensitivity)
        value = toSensitivity(gains_.last());
    else if (amplifierMode_ == Gain)
        value = gains_.first();
    else
        value = 0;

    return value;
}

double CLSKeithley428::maximumValueForUnits(const QString &units) const
{
    Q_UNUSED(units)

    double value;

    if (amplifierMode_ == Sensitivity)
        value = toSensitivity(gains_.first());
    else if (amplifierMode_ == Gain)
        value = gains_.last();
    else
        value = 0;

    return value;
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

void CLSKeithley428::setBiasVoltageEnabled(bool enabled)
{
	if (enabled)
		biasVoltageEnabled_->move(BiasVoltage::On);
	else
		biasVoltageEnabled_->move(BiasVoltage::Off);
}

void CLSKeithley428::setBiasVoltage(double newValue)
{
	biasVoltage_->move(newValue);
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
	bool connectedNow = false;

	if (connectState == true) {
		connectedNow = (
					valueControl_->isConnected() &&
					biasVoltageEnabled_->isConnected() &&
					biasVoltage_->isConnected()
					);
	}

	if (connected_ != connectedNow) {
		connected_ = connectedNow;
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
