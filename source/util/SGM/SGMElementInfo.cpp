#include "SGMElementInfo.h"

SGMEnergyPosition::SGMEnergyPosition(double energy, int monoEncoderTarget, int undulatorStepSetpoint, double exitSlitDistance)
{
	energy_ = energy;
	monoEncoderTarget_ = monoEncoderTarget;
	undulatorStepSetpoint_ = undulatorStepSetpoint;
	exitSlitDistance_ = exitSlitDistance;
}

SGMEnergyPosition& SGMEnergyPosition::operator=(const SGMEnergyPosition &other){
	if(this != &other){
		energy_ = other.energy();
		monoEncoderTarget_ = other.monoEncoderTarget();
		undulatorStepSetpoint_ = other.undulatorStepSetpoint();
		exitSlitDistance_ = other.exitSlitDistance();
	}
	return *this;
}

QDebug operator<<(QDebug d, const SGMEnergyPosition &energyPosition){
	d << QString("(%1 %2 %3 %4)").arg(energyPosition.energy()).arg(energyPosition.monoEncoderTarget()).arg(energyPosition.undulatorStepSetpoint()).arg(energyPosition.exitSlitDistance());
	return d;
}

SGMScanRangeInfo::SGMScanRangeInfo(const SGMEnergyPosition &start, const SGMEnergyPosition &middle, const SGMEnergyPosition &end)
{
	start_ = start;
	middle_ = middle;
	end_ = end;
}

SGMScanRangeInfo::SGMScanRangeInfo(const SGMScanRangeInfo &other)
{
	start_ = other.start();
	middle_ = other.middle();
	end_ = other.end();
}

QDebug operator<<(QDebug d, const SGMScanRangeInfo &rangeInfo){
	d << "[" << rangeInfo.start() << ", " << rangeInfo.middle() << ", " << rangeInfo.end() << "]";
	return d;
}

SGMEdgeInfo::SGMEdgeInfo(const QString &edge, double energy, const SGMScanRangeInfo &standardRange) :
		standardRange_(standardRange)
{
	edge_ = edge;
	energy_ = energy;
}

SGMEdgeInfo::SGMEdgeInfo(QPair<QString, QString> edgeAndEnergy, const SGMScanRangeInfo &standardRange) :
		standardRange_(standardRange)
{
	edge_ = edgeAndEnergy.first;
	energy_ = edgeAndEnergy.second.toDouble();
}

QDebug operator<<(QDebug d, const SGMEdgeInfo &edgeInfo){
	d << QString("{%1, %2, ").arg(edgeInfo.edge()).arg(edgeInfo.energy()) << edgeInfo.standardRange() << "}";
	return d;
}

SGMStandardScanInfo::SGMStandardScanInfo(const QString &name, const SGMScanRangeInfo &standardRange) :
		standardRange_(standardRange)
{
	name_ = name;
}

QDebug operator<<(QDebug d, const SGMStandardScanInfo &standardScanInfo){
	d << QString("{%1, ").arg(standardScanInfo.name()) << standardScanInfo.standardRange() << "}";
	return d;
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

AMOrderedSet<QString, SGMEdgeInfo> SGMElementInfo::sgmEdgeInfos() const{
	return sgmEdgeInfos_;
}

AMOrderedSet<QString, SGMStandardScanInfo> SGMElementInfo::sgmStandardScanInfos() const{
	return sgmStandardScanInfos_;
}

AMOrderedSet<int, SGMFastScanParameters*> SGMElementInfo::availableFastScanParameters() const{
	return availableFastScanParameters_;
}

bool SGMElementInfo::addEdgeInfo(const SGMEdgeInfo &edgeInfo){
	return sgmEdgeInfos_.append(edgeInfo, edgeInfo.edge());
}

bool SGMElementInfo::addStandardScanInfo(const SGMStandardScanInfo &standardScanInfo){
	return sgmStandardScanInfos_.append(standardScanInfo, standardScanInfo.name());
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

SGMFastScanParameters::SGMFastScanParameters(const QString &element, const SGMEdgeInfo &edgeInfo, double runSeconds, int motorSettings, double scalerTime, int baseLine, int undulatorVelocity, QObject *parent) :
		QObject(parent)
{
	setElement(element);
	setEdge(edgeInfo.edge());
	setRunSeconds(runSeconds);
	setEnergyStart(edgeInfo.standardStart().energy());
	setEnergyMidpoint(edgeInfo.standardMiddle().energy());
	setEnergyEnd(edgeInfo.standardEnd().energy());
	setVelocity(motorSettings);
	setVelocityBase(motorSettings);
	setAcceleration(motorSettings);
	setScalerTime(scalerTime);
	setBaseLine(baseLine);
	setUndulatorStartStep(edgeInfo.standardStart().undulatorStepSetpoint());
	setUndulatorVelocity(undulatorVelocity);
	setUndulatorRelativeStep(edgeInfo.standardEnd().undulatorStepSetpoint()-edgeInfo.standardStart().undulatorStepSetpoint());
}

SGMFastScanParameters::SGMFastScanParameters(const QString &element, const SGMStandardScanInfo &standardScanInfo, double runSeconds, int motorSettings, double scalerTime, int baseLine, int undulatorVelocity, QObject *parent) :
		QObject(parent)
{
	setElement(element);
	setEdge(standardScanInfo.name());
	setRunSeconds(runSeconds);
	setEnergyStart(standardScanInfo.standardStart().energy());
	setEnergyMidpoint(standardScanInfo.standardMiddle().energy());
	setEnergyEnd(standardScanInfo.standardEnd().energy());
	setVelocity(motorSettings);
	setVelocityBase(motorSettings);
	setAcceleration(motorSettings);
	setScalerTime(scalerTime);
	setBaseLine(baseLine);
	setUndulatorStartStep(standardScanInfo.standardStart().undulatorStepSetpoint());
	setUndulatorVelocity(undulatorVelocity);
	setUndulatorRelativeStep(standardScanInfo.standardEnd().undulatorStepSetpoint()-standardScanInfo.standardStart().undulatorStepSetpoint());
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
