#include "BeamPositionMonitor.h"
#include <QDebug>


BeamPositionMonitor::BeamPositionMonitor(const QString &pvXbpm, const QString &pvYbpm, QObject *parent)
    : QObject(parent)
{

    xPV_ = pvXbpm;
    yPV_ = pvYbpm;

    AMProcessVariable *processVariableY = new AMProcessVariable(yPV_, true, this);
    AMProcessVariable *processVariableX = new AMProcessVariable(xPV_, true, this);

    connect(processVariableX, SIGNAL(valueChanged(double)) ,this, SLOT(updateValueX(double) ) );
    connect(processVariableY, SIGNAL(valueChanged(double)), this, SLOT(updateValueY(double))  );



}

//Emit individual dimension value to be caught by BPM::updateValue
void BeamPositionMonitor::updateValueX(double value){

    xValues_.append(value);

    qDebug() << "xValues_.append(value) : " << value;

}

void BeamPositionMonitor::updateValueY(double value){

    yValues_.append(value);
     qDebug() << "yValues_.append(value) : " << value;

}

//Emited (x,y) values caught by View
void BeamPositionMonitor::sendValues(int xIndex, int yIndex){

    emit requestedValues(xValues_[xIndex], yValues_[yIndex]);
    qDebug() << "emit requestededValues";
}
