#include "BeamlineShutters.h"


BeamlineShutters::BeamlineShutters(const QString &name, const QString &psh1, const QString &psh2, const QString &ssh1, QObject *parent) :
    QObject(parent)
{
    nameOfBeamline_ = name;

    pvPSH1_ = new AMProcessVariable(psh1, true, this);
    pvPSH2_ = new AMProcessVariable(psh2, true, this);
    pvSSH1_ = new AMProcessVariable(ssh1, true, this);


    connect( pvPSH1_, SIGNAL(valueChanged(int)), this, SIGNAL(psh1Changed(int));
    connect( pvPSH2_, SIGNAL(valueChanged(int)), this, SIGNAL(psh2Changed(int));
    connect( pvSSH1_, SIGNAL(valueChanged(int)), this, SIGNAL(ssh1Changed(int));



}


void BeamlineShutters::psh1Changed(int value){

    emit psh1Changed(value);

}

void BeamlineShutters::psh2Changed(int value){

    emit psh2Changed(value);

}
void BeamlineShutters::ssh1Changed(int value){

    emit ssh1Changed(value);

}

