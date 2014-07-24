#include "BeamlineShutters.h"

BeamlineShutters::BeamlineShutters(const QString &name, const QString &psh1, const QString &psh2, const QString &ssh1, QObject *parent) :
    QObject(parent)
{
    nameOfBeamline_ = name;
    pvPSH1_ = psh1;
    pvPSH2_ = psh2;
    pvSSH1_ = ssh1;


}
