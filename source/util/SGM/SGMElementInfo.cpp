#include "SGMElementInfo.h"

SGMEnergyPosition::SGMEnergyPosition(double energy, int monoEncoderTarget, int undulatorStepSetpoint, double exitSlitDistance)
{
	energy_ = energy;
	monoEncoderTarget_ = monoEncoderTarget;
	undulatorStepSetpoint_ = undulatorStepSetpoint;
	exitSlitDistance_ = exitSlitDistance;
}

double SGMEnergyPosition::energy() const{
	return energy_;
}

int SGMEnergyPosition::monoEncoderTarget() const{
	return monoEncoderTarget_;
}

int SGMEnergyPosition::undulatorStepSetpoint() const{
	return undulatorStepSetpoint_;
}

double SGMEnergyPosition::exitSlitDistance() const{
	return exitSlitDistance_;
}

SGMElementInfo::SGMElementInfo(AMElement *element, const SGMEnergyPosition &standardStart, const SGMEnergyPosition &standardMiddle, const SGMEnergyPosition &standardEnd, QObject *parent) :
		QObject(parent)
{
	element_ = element;
	standardStart_ = standardStart;
	standardMiddle_ = standardMiddle;
	standardEnd_ = standardEnd;
}

AMElement* SGMElementInfo::element() const{
	return element_;
}

const SGMEnergyPosition& SGMElementInfo::standardStart() const{
	return standardStart_;
}

const SGMEnergyPosition& SGMElementInfo::standardMiddle() const{
	return standardMiddle_;
}

const SGMEnergyPosition& SGMElementInfo::standardEnd() const{
	return standardEnd_;
}

AMOrderedSet<int, SGMFastScanParameters*> SGMElementInfo::availableFastScanParameters() const{
	return availableFastScanParameters_;
}

bool SGMElementInfo::addFastScanParameters(SGMFastScanParameters *parameters){
	if(parameters)
		return availableFastScanParameters_.append(parameters, (int)parameters->runSeconds());
	else
		return false;
}

SGMFastScanParameters::SGMFastScanParameters(QObject *parent) : QObject(parent)
{
}

SGMFastScanParameters::SGMFastScanParameters(const QString &element, double runSeconds, double energyStart, double energyMidpoint, double energyEnd, int velocity, int velocityBase, int acceleration, double scalerTime, int baseLine, int undulatorStartStep, int undulatorVelocity, int undulatorRelativeStep, QObject *parent) :
		QObject(parent)
{
	setElement(element);
	setRunSeconds(runSeconds);
	setEnergyStart(energyStart);
	setEnergyMidpoint(energyMidpoint);
	setEnergyEnd(energyEnd);
	setVelocity(velocity);
	setVelocityBase(velocityBase);
	setAcceleration(acceleration);
	setScalerTime(scalerTime);
	setBaseLine(baseLine);
	setUndulatorStartStep(undulatorStartStep);
	setUndulatorVelocity(undulatorVelocity);
	setUndulatorRelativeStep(undulatorRelativeStep);
}

bool SGMFastScanParameters::operator ==(const SGMFastScanParameters &other){
	if( element() == other.element() &&
	    runSeconds() == other.runSeconds() &&
	    energyStart() == other.energyStart() &&
	    energyMidpoint() == other.energyMidpoint() &&
	    energyEnd() == other.energyEnd() &&
	    velocity() == other.velocity() &&
	    velocityBase() == other.velocityBase() &&
	    acceleration() == other.acceleration() &&
	    scalerTime() == other.scalerTime() &&
	    baseLine() == other.baseLine() &&
	    undulatorVelocity() == other.undulatorVelocity() &&
	    undulatorRelativeStep() == other.undulatorRelativeStep() ){
		return true;
	}
	return false;
}
