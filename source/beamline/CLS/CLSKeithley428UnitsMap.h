#ifndef CLSKEITHLEY428UNITSMAP_H
#define CLSKEITHLEY428UNITSMAP_H

#include <QObject>

#include "beamline/CLS/CLSKeithley428.h"

class CLSKeithley428UnitsMap : public QObject
{
    Q_OBJECT

public:
    explicit CLSKeithley428UnitsMap(QObject *parent = 0);
//    virtual ~CLSKeithley428UnitsMap();

//    int multiplier() const;
//    QStringList unitsStringList(CLSKeithley428::AmplifierMode mode) const;

signals:

public slots:

protected:

};

#endif // CLSKEITHLEY428UNITSMAP_H
