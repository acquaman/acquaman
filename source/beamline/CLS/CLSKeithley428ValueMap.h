#ifndef CLSKEITHLEY428VALUEMAP_H
#define CLSKEITHLEY428VALUEMAP_H

#include <QMultiMap>
#include <QStringList>

class CLSKeithley428ValueMap : public QObject
{
    Q_OBJECT

public:
    enum ValueMode {
        Gain = 0,
        Sensitivity = 1
    };

    explicit CLSKeithley428ValueMap();
    virtual ~CLSKeithley428ValueMap();

    QMultiMap<int, double>* map() const;

    double valueAt(int index, ValueMode mode);

    QStringList valueStringList(ValueMode mode) const;

    bool isMinIndex(ValueMode mode, int index);
    bool isMaxIndex(ValueMode mode, int index);

    int findMinIndex(ValueMode mode);
    int findMaxIndex(ValueMode mode);

signals:
    void valuesAdded(int index, double gain, double sensitivity);

public slots:
    void addValue(int index, ValueMode mode, double value);

protected:
    double toGain(double sensitivity);
    double toSensitivity(double gain);
    void addValues(int index, double gain, double sensitivity);

protected:
    QMultiMap<int, double> *map_;
    double tolerance_;

};

#endif // CLSKEITHLEY428VALUEMAP_H
