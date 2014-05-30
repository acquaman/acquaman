#ifndef CLSKEITHLEY428VALUEMAP_H
#define CLSKEITHLEY428VALUEMAP_H

#include <QMultiMap>
#include <QStringList>

#include "beamline/CLS/CLSKeithley428.h"

class CLSKeithley428ValueMap : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSKeithley428ValueMap();
    /// Destructor.
    virtual ~CLSKeithley428ValueMap();

    QMultiMap<int, double>* map() const;

    double valueAt(int index, CLSKeithley428::AmplifierMode mode);

    QStringList valueStringList(CLSKeithley428::AmplifierMode mode) const;

    bool isMinIndex(CLSKeithley428::AmplifierMode mode, int index);
    bool isMaxIndex(CLSKeithley428::AmplifierMode mode, int index);

    int findMinIndex(CLSKeithley428::AmplifierMode mode);
    int findMaxIndex(CLSKeithley428::AmplifierMode mode);

signals:
    void valuesAdded(int index, double gain, double sensitivity);

public slots:
    void addValue(int index, CLSKeithley428::AmplifierMode mode, double value);

protected:
    double toGain(double sensitivity);
    double toSensitivity(double gain);
    void addValues(int index, double gain, double sensitivity);

protected:
    QMultiMap<int, double> *map_;
    double tolerance_;

};

#endif // CLSKEITHLEY428VALUEMAP_H
