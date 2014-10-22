#include "BeamPositionMonitor.h"
#include <QDebug>


BeamPositionMonitor::BeamPositionMonitor(const QString &pvXbpm, const QString &pvYbpm, QObject *parent)
    : QObject(parent)
{

    AMProcessVariable *processVariableY = new AMProcessVariable(pvYbpm, true, this);
    AMProcessVariable *processVariableX = new AMProcessVariable(pvXbpm, true, this);

    connect(processVariableX, SIGNAL(valueChanged(double)), this, SLOT(updateXListfromPV(double)));
    connect(processVariableY, SIGNAL(valueChanged(double)), this, SLOT(updateYListfromPV(double)));

    numberOfPoints = 3;

}


//Check to see size of QList, if it's less than 10 values then append the new one
// otherwise clear the list and start again.
void BeamPositionMonitor::updateXListfromPV(double value){

    if(xValuesModel_.count() < NumberOfPoints())
    {
        xValuesModel_.append(value);

    }
    else if(xValuesModel_.count() == NumberOfPoints())
    {
        xValuesModel_.append(xValuesModel_.first());
        yValuesModel_.append(yValuesModel_.first());
        emit specifiedValuesCollected();
    }



    else
    {
        xValuesModel_.clear();
        xValuesModel_.append(value);
    }

    emit newValuesFromPV();


}


void BeamPositionMonitor::updateYListfromPV(double value){

    if(yValuesModel_.count() < NumberOfPoints())
    {
        yValuesModel_.append(value);
    }
    else
    {
        yValuesModel_.clear();
        yValuesModel_.append(value);
    }

    emit newValuesFromPV();

}

