#ifndef BEAMLINESHUTTERS_H
#define BEAMLINESHUTTERS_H

#include <QObject>
#include <QString>
#include "beamline/AMProcessVariable.h"

// BeamlineShutters Class is a Model that'll contain information on the shutter status of a particular beamline

class BeamlineShutters : public QObject
{
    Q_OBJECT

public:
    explicit BeamlineShutters(const QString &name,
                              const QString &psh1,
                              const QString &psh2,
                              const QString &ssh1,
                              QObject *parent = 0);

     QString nameOfBeamline_;

signals:
    void psh1Changed(int emtValue);
    void psh2Changed(int emtValue);
    void ssh1Changed(int emitValue);


public slots:

protected slots:
    void pvPSH1Changed(int value);
    void pvPSH2Changed(int value);
    void pvSSH1Changed(int value);

protected:
    QString sPSH1_;
    QString sPSH2_;
    QString sSSH1_;


};

#endif // BEAMLINESHUTTERS_H
