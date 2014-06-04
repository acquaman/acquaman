#include "CLSKeithley428.h"
#include "beamline/CLS/CLSKeithley428ValueMap.h"

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

    valueMap_ = new CLSKeithley428ValueMap(this);

    setAmplifierMode(AMCurrentAmplifier::Gain);
    setValueMap();
}

CLSKeithley428::~CLSKeithley428()
{

}

double CLSKeithley428::value() const
{
    return valueMap_->valueAt(index(), amplifierMode_);
}

double CLSKeithley428::valueAt(int index) const
{
    return valueMap_->valueAt(index, amplifierMode_);
}

int CLSKeithley428::index() const
{
    return valueControl_->getInt();
}

QString CLSKeithley428::units() const
{
    if (amplifierMode_ == AMCurrentAmplifier::Gain) {
        return "V/A";

    } else if (amplifierMode_ == AMCurrentAmplifier::Sensitivity) {
        return "A/V";

    } else {
        return "";
    }
}

CLSKeithley428ValueMap* CLSKeithley428::valueMap() const
{
    return valueMap_;
}

bool CLSKeithley428::atMaximumGain() const
{
    return valueMap_->isIndexOfMax(AMCurrentAmplifier::Gain, valueControl_->getInt());
}

bool CLSKeithley428::atMinimumGain() const
{
    return valueMap_->isIndexOfMin(AMCurrentAmplifier::Gain, valueControl_->getInt());
}

bool CLSKeithley428::atMaximumSensitivity() const
{
    return valueMap_->isIndexOfMax(AMCurrentAmplifier::Sensitivity, valueControl_->getInt());
}

bool CLSKeithley428::atMinimumSensitivity() const
{
    return valueMap_->isIndexOfMin(AMCurrentAmplifier::Sensitivity, valueControl_->getInt());
}

QStringList* CLSKeithley428::valueStringList() const
{
    return valueMap_->valueStringList(amplifierMode_);
}

QStringList* CLSKeithley428::unitsStringList() const
{
    QStringList *unitsList = new QStringList();

    if (amplifierMode_ == AMCurrentAmplifier::Gain) {
        unitsList->append("V/A");
    } else {
        unitsList->append("A/V");
    }

    return unitsList;
}

void CLSKeithley428::setValue(double newValue)
{
    int index = valueMap_->map()->key(newValue);
    setValueIndex(index);
}

void CLSKeithley428::setValueIndex(int newIndex)
{
    if (valueMap_->map()->contains(newIndex) && newIndex != valueControl_->getInt()) {
        valueControl_->setValue(newIndex);
    }
}

bool CLSKeithley428::increaseGain()
{
    // Don't do anything if the value is already at a maximum.
    if (atMaximumGain())
        return false;

    // Increase value
    int newIndex = valueMap_->nextIndex(IncreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

bool CLSKeithley428::decreaseGain()
{
    // Don't do anything if the value is already at a minimum.
    if (atMinimumGain()) {
        return false;
    }

    // Decrease value
    int newIndex = valueMap_->nextIndex(DecreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

bool CLSKeithley428::increaseSensitivity()
{
    if (atMaximumSensitivity()) {
        return false;
    }

    // the values in the valueMap are organized by increasing gain.
    // to increase sensitivity, must decrease index.

    int newIndex = valueMap_->nextIndex(DecreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

bool CLSKeithley428::decreaseSensitivity()
{
    if (atMinimumSensitivity()) {
        return false;
    }

    // the values in the valueMap are organized by increasing gain.
    // to decrease sensitivity, must increase index.

    int newIndex = valueMap_->nextIndex(IncreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

void CLSKeithley428::onValueChanged(int newIndex)
{
    Q_UNUSED(newIndex)

    if (amplifierMode_ == AMCurrentAmplifier::Gain) {

        emit gainChanged();

        if (atMinimumGain())
            emit minimumGain(true);
        else
            emit minimumGain(false);

        if (atMaximumGain())
            emit maximumGain(true);
        else
            emit maximumGain(false);

    } else if (amplifierMode_ == AMCurrentAmplifier::Sensitivity) {

        emit sensitivityChanged();

        if (atMinimumSensitivity())
            emit minimumSensitivity(true);
        else
            emit minimumSensitivity(false);

        if (atMaximumSensitivity())
            emit maximumSensitivity(true);
        else
            emit maximumSensitivity(false);
    }

}

void CLSKeithley428::onConnectedStateChanged(bool connectState)
{
    if (connected_ != connectState) {
        connected_ = connectState;
        emit isConnected(connected_);
    }
}

void CLSKeithley428::setValueMap()
{
    QList<double> *valueList = new QList<double>();
    valueList->append(pow(10, 3));
    valueList->append(pow(10, 4));
    valueList->append(pow(10, 5));
    valueList->append(pow(10, 6));
    valueList->append(pow(10, 7));
    valueList->append(pow(10, 8));
    valueList->append(pow(10, 9));
    valueList->append(pow(10, 10));

    valueMap_->setValues(AMCurrentAmplifier::Gain, valueList);
}

void CLSKeithley428::setValueImplementation(const QString &valueArg)
{
//    int newIndex = valueArg.at(0).toInt();
//    setValueIndex(newIndex);
}
