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

    valueMap_ = new CLSKeithley428ValueMap(this);

    setAmplifierMode(AMCurrentAmplifier::Gain);
    setValueMap();
}

CLSKeithley428::~CLSKeithley428()
{

}

double CLSKeithley428::value() const
{
    return valueMap_->valueAt(valueIndex(), amplifierMode_);
}

double CLSKeithley428::valueAt(int index) const
{
    return valueMap_->valueAt(index, amplifierMode_);
}

int CLSKeithley428::valueIndex() const
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

QStringList CLSKeithley428::valuesList() const
{
    return *valueMap_->valueStringList(amplifierMode_);
}

QStringList CLSKeithley428::unitsList() const
{
    QStringList *unitsList = new QStringList();

    if (amplifierMode_ == AMCurrentAmplifier::Gain) {
        unitsList->append("V/A");
    } else {
        unitsList->append("A/V");
    }

    return *unitsList;
}

void CLSKeithley428::setValueIndex(int newIndex)
{
    if (valueMap_->map()->contains(newIndex) && newIndex != valueControl_->getInt()) {
        qDebug() << "Setting amplifier value to " << newIndex;
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
    if (amplifierMode_ == AMCurrentAmplifier::Gain) {

        emit gainChanged(newIndex);

        if (atMinimumGain())
            emit minimumGain(true);
        else
            emit minimumGain(false);

        if (atMaximumGain())
            emit maximumGain(true);
        else
            emit maximumGain(false);

    } else if (amplifierMode_ == AMCurrentAmplifier::Sensitivity) {

        emit sensitivityChanged(newIndex);

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
    int newIndex = valueArg.toInt();
    setValueIndex(newIndex);
}

CLSKeithley428ValueMap::CLSKeithley428ValueMap(QObject *parent) : QObject(parent)
{
    tolerance_ = 0.01;
    map_ = new QMultiMap<int, double>();
}

CLSKeithley428ValueMap::~CLSKeithley428ValueMap()
{

}

QMultiMap<int, double>* CLSKeithley428ValueMap::map() const
{
    return map_;
}

double CLSKeithley428ValueMap::valueAt(int index, AMCurrentAmplifier::AmplifierMode mode)
{
    return map_->values(index).at(mode);
}

QStringList* CLSKeithley428ValueMap::valueStringList(AMCurrentAmplifier::AmplifierMode mode) const
{
    double toAdd = -1;
    QStringList *valueList = new QStringList();

    foreach (int key, map_->uniqueKeys()) {
        toAdd = map_->values(key).at(mode);
        valueList->append(QString("%1").arg(toAdd, 0, 'e', 2));
    }

    return valueList;
}

bool CLSKeithley428ValueMap::isIndexOfMin(AMCurrentAmplifier::AmplifierMode mode, int index)
{
    return (findIndexOfMin(mode) == index);
}

bool CLSKeithley428ValueMap::isIndexOfMax(AMCurrentAmplifier::AmplifierMode mode, int index)
{
    return (findIndexOfMax(mode) == index);
}

int CLSKeithley428ValueMap::findIndexOfMin(AMCurrentAmplifier::AmplifierMode mode)
{
    double firstIndex = map_->uniqueKeys().first();
    double firstValue = map_->values(firstIndex).at(mode);
    double temp = firstValue;

    int minIndex = firstIndex;
    double minValue = firstValue;

    foreach (int key, map_->uniqueKeys()) {
        temp = map_->values(key).at(mode);

        if (temp < minValue) {
            minIndex = key;
            minValue = temp;
        }
    }

    return minIndex;
}

int CLSKeithley428ValueMap::findIndexOfMax(AMCurrentAmplifier::AmplifierMode mode)
{
    double firstIndex = map_->uniqueKeys().first();
    double firstValue = map_->values(firstIndex).at(mode);
    double temp = firstValue;

    int maxIndex = firstIndex;
    double maxValue = firstValue;

    foreach (int key, map_->uniqueKeys()) {
        temp = map_->values(key).at(mode);

        if (temp > maxValue) {
            maxIndex = key;
            maxValue = temp;
        }
    }

    return maxIndex;
}

int CLSKeithley428ValueMap::nextIndex(CLSKeithley428::IndexChange change, int currentIndex)
{
    qDebug() << "Initial index : " << currentIndex;

    int nextIndex = -1;

    if (!map_->contains(currentIndex)) {
        qDebug() << "Invalid initial index.";

    } else if (change == CLSKeithley428::IncreaseOne) {
        qDebug() << "Increasing index.";
        nextIndex = currentIndex + 1;

    } else if (change == CLSKeithley428::DecreaseOne) {
        qDebug() << "Decreasing index.";
        nextIndex = currentIndex - 1;
    }

    qDebug() << "Next index : " << nextIndex;

    return nextIndex;
}

void CLSKeithley428ValueMap::setValues(AMCurrentAmplifier::AmplifierMode mode, QList<double>* toAdd)
{
    map_->clear();

    for (int i = 0; i < toAdd->size(); i++) {
        addValue(i, mode, toAdd->at(i));
    }
}

void CLSKeithley428ValueMap::addValue(int index, AMCurrentAmplifier::AmplifierMode mode, double value)
{
    if (!map_->contains(index)) {
        double gain, sensitivity;

        if (mode == AMCurrentAmplifier::Gain) {
            gain = value;
            sensitivity = toSensitivity(value);

        } else if (mode == AMCurrentAmplifier::Sensitivity) {
            gain = toGain(value);
            sensitivity = value;

        } else {
            return;
        }

        addIndexValues(index, gain, sensitivity);
    }
}

void CLSKeithley428ValueMap::addIndexValues(int index, double gain, double sensitivity)
{
    map_->insert(index, gain);
    map_->insert(index, sensitivity);

    emit valuesAdded(index, gain, sensitivity);

}

double CLSKeithley428ValueMap::toGain(double sensitivity)
{
    return 1.0/sensitivity;
}

double CLSKeithley428ValueMap::toSensitivity(double gain)
{
    return 1.0/gain;
}
