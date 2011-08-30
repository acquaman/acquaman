/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SGMElementInfo.h"

SGMEnergyPosition::SGMEnergyPosition(double energy, int monoEncoderTarget, int undulatorStepSetpoint, double exitSlitDistance, int sgmGrating)
{
	setEnergy(energy);
	setMonoEncoderTarget(monoEncoderTarget);
	setUndulatorStepSetpoint(undulatorStepSetpoint);
	setExitSlitDistance(exitSlitDistance);
	setSGMGrating(sgmGrating);
}

SGMEnergyPosition& SGMEnergyPosition::operator=(const SGMEnergyPosition &other){
	if(this != &other){
		setEnergy(other.energy());
		setMonoEncoderTarget(other.monoEncoderTarget());
		setUndulatorStepSetpoint(other.undulatorStepSetpoint());
		setExitSlitDistance(other.exitSlitDistance());
		setSGMGrating(other.sgmGrating());
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

SGMScanRangeInfo::SGMScanRangeInfo(const SGMScanRangeInfo &other) :
		AMDbObject(other)
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
	setEdge(edge);
	setEnergy(energy);
}

SGMEdgeInfo::SGMEdgeInfo(QPair<QString, QString> edgeAndEnergy, const SGMScanRangeInfo &standardRange) :
		standardRange_(standardRange)
{
	setEdge(edgeAndEnergy.first);
	setEnergy(edgeAndEnergy.second.toDouble());
}

QDebug operator<<(QDebug d, const SGMEdgeInfo &edgeInfo){
	d << QString("{%1, %2, ").arg(edgeInfo.edge()).arg(edgeInfo.energy()) << edgeInfo.standardRange() << "}";
	return d;
}

SGMStandardScanInfo::SGMStandardScanInfo(const QString &scanName, const SGMScanRangeInfo &standardRange) :
		standardRange_(standardRange)
{
	setScanName(scanName);
}

QDebug operator<<(QDebug d, const SGMStandardScanInfo &standardScanInfo){
	d << QString("{%1, ").arg(standardScanInfo.scanName()) << standardScanInfo.standardRange() << "}";
	return d;
}

SGMElementInfo::SGMElementInfo(AMElement *element, QObject *parent) :
		AMDbObject(parent)
//		QObject(parent)
{
	element_ = element;
}

AMElement* SGMElementInfo::element() const{
	return element_;
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
	return sgmStandardScanInfos_.append(standardScanInfo, standardScanInfo.scanName());
}

bool SGMElementInfo::addFastScanParameters(SGMFastScanParameters *parameters){
	if(parameters)
		return availableFastScanParameters_.append(parameters, (int)parameters->runSeconds());
	else
		return false;
}

AMDbObjectList SGMElementInfo::dbReadSGMEdgeInfos(){
	AMDbObjectList rv;
	for(int x = 0; x < sgmEdgeInfos_.count(); x++)
		rv << &(sgmEdgeInfos_[x]);
	return rv;
}

AMDbObjectList SGMElementInfo::dbReadSGMStandardScanInfos(){
	AMDbObjectList rv;
	for(int x = 0; x < sgmStandardScanInfos_.count(); x++)
		rv << &(sgmStandardScanInfos_[x]);
	return rv;
}

void SGMElementInfo::dbLoadSGMEdgeInfos(const AMDbObjectList &sgmEdgeInfos){
	sgmEdgeInfos_.clear();

	for(int x = 0; x < sgmEdgeInfos.count(); x++){
		SGMEdgeInfo* newEdgeInfo = qobject_cast<SGMEdgeInfo*>(sgmEdgeInfos.at(x));
		if(newEdgeInfo)
			sgmEdgeInfos_.append(*newEdgeInfo, newEdgeInfo->edge());// note: makes a copy of object pointed to by newStandardScanInfo, and stores in our internal list.

		delete sgmEdgeInfos.at(x); // we're copying these; don't need to keep these ones around. They're our responsibility to delete.
	}
}

void SGMElementInfo::dbLoadSGMStandardScanInfos(const AMDbObjectList &sgmStandardScanInfos){
	sgmStandardScanInfos_.clear();

	for(int x = 0; x < sgmStandardScanInfos.count(); x++){
		SGMStandardScanInfo* newStandardScanInfo = qobject_cast<SGMStandardScanInfo*>(sgmStandardScanInfos.at(x));
		if(newStandardScanInfo)
			sgmStandardScanInfos_.append(*newStandardScanInfo, newStandardScanInfo->scanName());// note: makes a copy of object pointed to by newStandardScanInfo, and stores in our internal list.

		delete sgmStandardScanInfos.at(x); // we're copying these; don't need to keep these ones around. They're our responsibility to delete.
	}
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
	setExitSlitDistance(edgeInfo.standardMiddle().exitSlitDistance());
	setSGMGrating(edgeInfo.standardStart().sgmGrating());
}

SGMFastScanParameters::SGMFastScanParameters(const QString &element, const SGMStandardScanInfo &standardScanInfo, double runSeconds, int motorSettings, double scalerTime, int baseLine, int undulatorVelocity, QObject *parent) :
		QObject(parent)
{
	setElement(element);
	setEdge(standardScanInfo.scanName());
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
	setExitSlitDistance(standardScanInfo.standardMiddle().exitSlitDistance());
	setSGMGrating(standardScanInfo.standardStart().sgmGrating());
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
	    undulatorStartStep() == other.undulatorStartStep() &&
	    undulatorVelocity() == other.undulatorVelocity() &&
	    undulatorRelativeStep() == other.undulatorRelativeStep() &&
	    exitSlitDistance() == other.exitSlitDistance() &&
	    sgmGrating() == other.sgmGrating()){
		return true;
	}
	return false;
}
