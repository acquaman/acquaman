#include "beamline/PGM/PGMBPMControl.h"

PGMBPMControl::PGMBPMControl(const QString &name, const QString &pvName, int averagValue, int variance, QObject *parent, const QString description)
	: AMReadOnlyPVControl(name, pvName, parent, description)

{
	averageValue_ = averagValue;
    variance_ = variance;
	validityState_ = true;

	// Get updated value from AMReadOnlyPVControl.
	connect(this, SIGNAL(valueChanged(double)), this, SLOT(onPVValueChanged(double)));

}

QString PGMBPMControl::valueWithUnitString() const
{
	return QString("%1 um").arg(value(), 0, 'f', 0);
}


void PGMBPMControl::onPVValueChanged(double value)
{
	updateValidityState(value);

	emit onBPMValueChanged(valueWithUnitString());
}

void PGMBPMControl::updateValidityState(double newValue){

	int upperBound = averageValue_ + variance_;
	int lowerBound = averageValue_ - variance_;

	bool newState = (newValue < upperBound || newValue > lowerBound);

	if (newState != validityState_){

		validityState_ = newState;
		emit validityStateChanged(validityState_);
	}
}
