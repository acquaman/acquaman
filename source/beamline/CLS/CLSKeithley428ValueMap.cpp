#include "CLSKeithley428ValueMap.h"

#include <QDebug>

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
    int nextIndex = -1;

    if (!map_->contains(currentIndex)) {
        qDebug() << "Invalid initial index.";
        nextIndex = -1;

    } else if (currentIndex <= 0) {
        qDebug() << "Min index.";
        nextIndex = currentIndex;

    } else if (currentIndex >= map_->uniqueKeys().size()) {
        qDebug() << "Max index.";
        nextIndex = currentIndex;

    } else if (change == CLSKeithley428::IncreaseOne) {
        nextIndex = currentIndex++;

    } else if (change == CLSKeithley428::DecreaseOne) {
        nextIndex = currentIndex--;

    } else {
        qDebug() << "CLSKeithley428 : Unknown change discovered--neither IncreaseOne nor DecreaseOne.";
        nextIndex = currentIndex;
    }

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

        if (mode == CLSKeithley428::Gain) {
            gain = value;
            sensitivity = toSensitivity(value);

        } else if (mode == CLSKeithley428::Sensitivity) {
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
