#include "BeamlineShutters.h"
#include <QIcon>


BeamlineShutters::BeamlineShutters(const QString &name, const QString &psh1, const QString &psh2, const QString &ssh1, QObject *parent)
    :      QObject(parent)

{
    nameOfBeamline_ = name;

    sPSH1_ = psh1;
    sPSH2_ = psh2;
    sSSH1_ = ssh1;

    AMProcessVariable *pvPSH1_ = new AMProcessVariable(sPSH1_, true, this);
    AMProcessVariable *pvPSH2_ = new AMProcessVariable(sPSH2_, true, this);
    AMProcessVariable *pvSSH1_ = new AMProcessVariable(sSSH1_, true, this);


    connect( pvPSH1_, SIGNAL(valueChanged(int)), this, SLOT(pvPSH1Changed(int)));
    connect( pvPSH2_, SIGNAL(valueChanged(int)), this, SLOT(pvPSH2Changed(int)));
    connect( pvSSH1_, SIGNAL(valueChanged(int)), this, SLOT(pvSSH1Changed(int)));


}


void BeamlineShutters::pvPSH1Changed(int value){

    emit psh1Changed(value);

}

void BeamlineShutters::pvPSH2Changed(int value){

    emit psh2Changed(value);

}
void BeamlineShutters::pvSSH1Changed(int value){

    emit ssh1Changed(value);

}

