#include "CLSKeithley428.h"
#include "beamline/CLS/CLSKeithley428ValueMap.h"

#include "math.h"

#include <QDebug>

CLSKeithley428::CLSKeithley428(const QString &name, const QString &valueName, QObject *parent) :
    AMCurrentAmplifier(name, parent)
{
    // temporary, for testing.
    connect( this, SIGNAL(maximumValue(bool)), this, SLOT(onMaximumValue()) );
    connect( this, SIGNAL(minimumValue(bool)), this, SLOT(onMinimumValue()) );

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

bool CLSKeithley428::atMinimumSensitivity() const
{
    bool min = false;

    if (amplifierMode_ == AMCurrentAmplifier::Sensitivity)
        min = atMinimumValue();
    else
        min = atMaximumValue();

    return min;
}

bool CLSKeithley428::atMaximumSensitivity() const
{
    bool max = false;

    if (amplifierMode_ == AMCurrentAmplifier::Sensitivity)
        max = atMaximumValue();
    else
        max = atMinimumValue();

    return max;
}

bool CLSKeithley428::atMaximumValue() const
{
    return valueMap_->isIndexOfMax(amplifierMode_, valueControl_->getInt());
}

bool CLSKeithley428::atMinimumValue() const
{
    return valueMap_->isIndexOfMin(amplifierMode_, valueControl_->getInt());
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

void CLSKeithley428::setValueIndex(int valueIndex)
{
    // check that the given index corresponds to a value, then set the value control.
    if (valueMap_->map()->contains(valueIndex) && valueIndex != valueControl_->getInt()) {
        valueControl_->setValue(valueIndex);
    }
}

bool CLSKeithley428::increaseSensitivity()
{
    return false;
}

bool CLSKeithley428::decreaseSensitivity()
{
    return false;
}

bool CLSKeithley428::increaseValue()
{
    // Don't do anything if the value is already at a maximum.
    if (atMaximumValue())
        return false;

    // Increase value
    int newIndex = valueMap_->nextIndex(IncreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

bool CLSKeithley428::decreaseValue()
{
    // Don't do anything if the value is already at a minimum.
    if (atMinimumValue()) {
        return false;
    }

    // Decrease value
    int newIndex = valueMap_->nextIndex(DecreaseOne, valueControl_->getInt());
    setValueIndex(newIndex);

    return true;
}

void CLSKeithley428::onValueChanged(int newIndex)
{
    emit valueChanged();
    emit indexChanged(newIndex);

    if (atMaximumValue()) {
        emit maximumValue(true);
    }

    if (atMinimumValue()) {
        emit minimumValue(true);
    }
}

void CLSKeithley428::onConnectedStateChanged(bool connectState)
{
    if (connected_ != connectState) {
        connected_ = connectState;
        emit isConnected(connected_);
    }
}

void CLSKeithley428::onMaximumValue()
{
    qDebug() << "Maximum value.";
}

void CLSKeithley428::onMinimumValue()
{
    qDebug() << "Minimum value.";
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

    valueMap_->setValues(amplifierMode_, valueList);
}
