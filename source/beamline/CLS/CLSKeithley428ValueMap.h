#ifndef CLSKEITHLEY428VALUEMAP_H
#define CLSKEITHLEY428VALUEMAP_H

#include <QMultiMap>
#include <QStringList>

//#include "beamline/AMCurrentAmplifier.h"
#include "beamline/CLS/CLSKeithley428.h"

class CLSKeithley428ValueMap : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSKeithley428ValueMap(QObject *parent = 0);
    /// Destructor.
    virtual ~CLSKeithley428ValueMap();

    QMultiMap<int, double>* map() const;

    double valueAt(int index, AMCurrentAmplifier::AmplifierMode mode);

    QStringList* valueStringList(AMCurrentAmplifier::AmplifierMode mode) const;

    bool isIndexOfMin(AMCurrentAmplifier::AmplifierMode mode, int index);
    bool isIndexOfMax(AMCurrentAmplifier::AmplifierMode mode, int index);

    int findIndexOfMin(AMCurrentAmplifier::AmplifierMode mode);
    int findIndexOfMax(AMCurrentAmplifier::AmplifierMode mode);

    int nextIndex(CLSKeithley428::IndexChange change, int currentIndex);

signals:
    void valuesAdded(int index, double gain, double sensitivity);

public slots:
    void setValues(AMCurrentAmplifier::AmplifierMode mode, QList<double>* toAdd);

protected:
    void addValue(int index, AMCurrentAmplifier::AmplifierMode mode, double value);
    void addIndexValues(int index, double gain, double sensitivity);

    double toGain(double sensitivity);
    double toSensitivity(double gain);

protected:
    QMultiMap<int, double> *map_;
    double tolerance_;

};

#endif // CLSKEITHLEY428VALUEMAP_H
