#include "PGMBpmControl.h"
#include <qdebug.h>
PGMBpmControl::PGMBpmControl(const QString &name, const QString &pvName, const int avgValue, const int variance, QObject *parent)
   : AMReadOnlyPVControl( name, pvName, parent )
{
    currentValue_ = 0;
    averageValue_ = avgValue;
    variance_ = variance;

    // Get updated pv value from parent class.
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(setCurrentValue(double)));

}

void PGMBpmControl::isValid(){

    int upperBound = averageValue_ + variance_;
    int lowerBound = averageValue_ - variance_;

    if( currentValue_ > upperBound || currentValue_ < lowerBound ){
        emit isValidStateChanged(false);
    } else {
        emit isValidStateChanged(true);
    }
}

void PGMBpmControl::setCurrentValue(double value){

    currentValue_ = value;
    isValid();
    emit onValueChanged(QString("%1 um").arg(value, 0, 'f', 0));
}
