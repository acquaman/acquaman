#include "CLSKeithley428ValueMap.h"



#include <QDebug>

CLSKeithley428ValueMap::CLSKeithley428ValueMap() :
    QObject()
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

double CLSKeithley428ValueMap::valueAt(int index, CLSKeithley428::AmplifierMode mode)
{
    return map_->values(index).at(mode);
}

QStringList CLSKeithley428ValueMap::valueStringList(CLSKeithley428::AmplifierMode mode) const
{
    double toAdd = -1;
    QStringList *valueList = new QStringList();

    foreach (int key, map_->uniqueKeys()) {
        toAdd = map_->values(key).at(mode);
        valueList->append(QString("%1").arg(toAdd, 0, 'e', 2));
    }

    return *valueList;
}

bool CLSKeithley428ValueMap::isMinIndex(CLSKeithley428::AmplifierMode mode, int index)
{
    return (findMinIndex(mode) == index);
}

bool CLSKeithley428ValueMap::isMaxIndex(CLSKeithley428::AmplifierMode mode, int index)
{
    return (findMaxIndex(mode) == index);
}

int CLSKeithley428ValueMap::findMinIndex(CLSKeithley428::AmplifierMode mode)
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

int CLSKeithley428ValueMap::findMaxIndex(CLSKeithley428::AmplifierMode mode)
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

void CLSKeithley428ValueMap::addValue(int index, CLSKeithley428::AmplifierMode mode, double value)
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

        addValues(index, gain, sensitivity);
    }
}

double CLSKeithley428ValueMap::toGain(double sensitivity)
{
    return 1.0/sensitivity;
}

double CLSKeithley428ValueMap::toSensitivity(double gain)
{
    return 1.0/gain;
}

void CLSKeithley428ValueMap::addValues(int index, double gain, double sensitivity)
{
    map_->insert(index, gain);
    map_->insert(index, sensitivity);

    emit valuesAdded(index, gain, sensitivity);

}
