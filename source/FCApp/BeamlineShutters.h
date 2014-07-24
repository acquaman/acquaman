#ifndef BEAMLINESHUTTERS_H
#define BEAMLINESHUTTERS_H

#include <QObject>
#include <QString>
#include <QLabel>
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

signals:
    void psh1Changed(int emtValue);
    void psh2Changed(int emtValue);
    void ssh1Changed(int emitValue);


public slots:

protected slots:
    void psh1Changed(int value);
    void psh2Changed(int value);
    void ssh1Changed(int value);

protected:
    QString nameOfBeamline_;

    QString pvPSH1_;
    QString pvPSH2_;
    QString pvSSH1_;


};

#endif // BEAMLINESHUTTERS_H
