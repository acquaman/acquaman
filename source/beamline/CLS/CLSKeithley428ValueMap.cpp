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

double CLSKeithley428ValueMap::valueAt(int index, ValueMode mode)
{
    return map_->values(index).at(mode);
}

QStringList CLSKeithley428ValueMap::valueStringList(ValueMode mode) const
{
    double toAdd = -1;
    QStringList *valueList = new QStringList();

    qDebug() << "Mode : " << mode;

    foreach (int key, map_->uniqueKeys()) {
        toAdd = map_->values(key).at(mode);
        qDebug() << toAdd;
        valueList->append(QString("%1").arg(toAdd, 0, 'e', 2));
    }

    return *valueList;
}

bool CLSKeithley428ValueMap::isMinIndex(int index)
{
    return (map_->keys().first() == index);
}

bool CLSKeithley428ValueMap::isMaxIndex(int index)
{
    return (map_->keys().last() == index);
}

void CLSKeithley428ValueMap::addValue(int index, ValueMode mode, double value)
{
    if (!map_->contains(index)) {
        double gain, sensitivity;

        if (mode == Gain) {
            gain = value;
            sensitivity = toSensitivity(value);

        } else if (mode == Sensitivity) {
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
