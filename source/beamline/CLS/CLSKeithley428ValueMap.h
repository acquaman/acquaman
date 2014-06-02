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

    QStringList* valueStringList(CLSKeithley428::AmplifierMode mode) const;

    bool isIndexOfMin(CLSKeithley428::AmplifierMode mode, int index);
    bool isIndexOfMax(CLSKeithley428::AmplifierMode mode, int index);

    int findIndexOfMin(CLSKeithley428::AmplifierMode mode);
    int findIndexOfMax(CLSKeithley428::AmplifierMode mode);

    int nextIndex(CLSKeithley428::IndexChange change, int currentIndex);

signals:
    void valuesAdded(int index, double gain, double sensitivity);

public slots:
    void setValues(CLSKeithley428::AmplifierMode mode, QList<double>* toAdd);

protected:
    void addValue(int index, CLSKeithley428::AmplifierMode mode, double value);
    void addIndexValues(int index, double gain, double sensitivity);

    double toGain(double sensitivity);
    double toSensitivity(double gain);

protected:
    QMultiMap<int, double> *map_;
    double tolerance_;

};

#endif // CLSKEITHLEY428VALUEMAP_H
