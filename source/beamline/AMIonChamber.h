#ifndef AMIONCHAMBER_H
#define AMIONCHAMBER_H

#include <QObject>

#include "beamline/AMDetector.h"

class AMIonChamber : public QObject, public AMDetector
{
    Q_OBJECT
public:
    explicit AMIonChamber(QObject *parent = 0);

signals:

public slots:

};

#endif // AMIONCHAMBER_H
