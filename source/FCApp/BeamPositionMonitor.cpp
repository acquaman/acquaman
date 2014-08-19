#include "BeamPositionMonitor.h"
#include <QDebug>


BeamPositionMonitor::BeamPositionMonitor(const QString &name, const QString &pv, QObject *parent)
    : QObject(parent)
{
    axisName_ = name;
    pvAxisRMS_ = pv;

    AMProcessVariable *procv = new AMProcessVariable(pvAxisRMS_, true, this);

    connect(procv, SIGNAL(valueChanged(double)), this, SLOT(bpmPVChanged(double)));


}

void BeamPositionMonitor::bpmPVChanged(double value){

 emit rmsChanged(value);

}
