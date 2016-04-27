#include "beamline/PGM/PGMBPMControl.h"

PGMBPMControl::PGMBPMControl(const QString &name, const QString &pvName, int avgValue, int variance, QObject *parent)
    : QObject(parent)

{
    currentValue_ = 0;
    averageValue_ = avgValue;
    variance_ = variance;
    state_ = true;
    bpmPV_ = new AMReadOnlyPVControl(name, pvName, this);

    // Get updated pv value from AMReadOnlyPVControl.
    connect(bpmPV_, SIGNAL(valueChanged(double)), this, SLOT(setCurrentValue(double)));

}

void PGMBPMControl::updateValidity(double newValue){

    int upperBound = averageValue_ + variance_;
    int lowerBound = averageValue_ - variance_;

    bool newState = (newValue < upperBound || newValue > lowerBound);

    if (newState != state_){

        state_ = newState;
        emit stateValidityChanged(state_);
    }
}

void PGMBPMControl::setCurrentValue(double value){

    currentValue_ = value;
    updateValidity(currentValue_);
    emit onBPMValueChanged(QString("%1 um").arg(value, 0, 'f', 0));
}

