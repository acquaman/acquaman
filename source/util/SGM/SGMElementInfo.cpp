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

SGMEnergyPosition::SGMEnergyPosition(double energy, int monoEncoderTarget, int undulatorStepSetpoint, double exitSlitDistance, int sgmGrating, const QString &name)
{
	setName(name);
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

SGMScanRangeInfo::SGMScanRangeInfo(const SGMEnergyPosition &start, const SGMEnergyPosition &middle, const SGMEnergyPosition &end, const QString &name)
{
	setName(name);
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

SGMScanRangeInfo& SGMScanRangeInfo::operator =(const SGMScanRangeInfo &other){
	if(this != &other){
		start_ = other.start();
		middle_ = other.middle();
		end_ = other.end();
	}
	return *this;
}

QDebug operator<<(QDebug d, const SGMScanRangeInfo &rangeInfo){
	d << "[" << rangeInfo.start() << ", " << rangeInfo.middle() << ", " << rangeInfo.end() << "]";
	return d;
}

SGMEdgeInfo::SGMEdgeInfo(const QString &edge, double energy, const SGMScanRangeInfo &standardRange, const QString &name) :
		standardRange_(standardRange)
{
	setName(name);
	setEdge(edge);
	setEnergy(energy);
}

SGMEdgeInfo::SGMEdgeInfo(QPair<QString, QString> edgeAndEnergy, const SGMScanRangeInfo &standardRange, const QString &name) :
		standardRange_(standardRange)
{
	setName(name);
	setEdge(edgeAndEnergy.first);
	setEnergy(edgeAndEnergy.second.toDouble());
}

SGMEdgeInfo& SGMEdgeInfo::operator =(const SGMEdgeInfo &other){
	if(this != &other){
		setEdge(other.edge());
		setEnergy(other.energy());
		standardRange_ = other.standardRange();
	}
	return *this;
}

QDebug operator<<(QDebug d, const SGMEdgeInfo &edgeInfo){
	d << QString("{%1, %2, ").arg(edgeInfo.edge()).arg(edgeInfo.energy()) << edgeInfo.standardRange() << "}";
	return d;
}

SGMStandardScanInfo::SGMStandardScanInfo(const QString &scanName, const SGMScanRangeInfo &standardRange, const QString &name) :
		standardRange_(standardRange)
{
	setName(name);
	setScanName(scanName);
}

SGMStandardScanInfo& SGMStandardScanInfo::operator =(const SGMStandardScanInfo &other){
	if(this != &other){
		setScanName(other.scanName());
		standardRange_ = other.standardRange();
	}
	return *this;
}

QDebug operator<<(QDebug d, const SGMStandardScanInfo &standardScanInfo){
	d << QString("{%1, ").arg(standardScanInfo.scanName()) << standardScanInfo.standardRange() << "}";
	return d;
}

SGMElementInfo::SGMElementInfo(AMElement *element, QObject *parent, const QString &name) :
		AMDbObject(parent)
{
	setName(name);
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

AMDbObjectList SGMElementInfo::dbReadSGMFastScanParameters(){
	AMDbObjectList rv;
	for(int x = 0; x < availableFastScanParameters_.count(); x++)
		rv << availableFastScanParameters_[x];
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

void SGMElementInfo::dbLoadSGMFastScanParameters(const AMDbObjectList &sgmFastScanParameters){
	availableFastScanParameters_.clear();

	for(int x = 0; x < sgmFastScanParameters.count(); x++){
		SGMFastScanParameters* newFastScanParameter = qobject_cast<SGMFastScanParameters*>(sgmFastScanParameters.at(x));
		if(newFastScanParameter)
			availableFastScanParameters_.append(newFastScanParameter, (int)newFastScanParameter->runSeconds());// note: makes a copy of object pointed to by newStandardScanInfo, and stores in our internal list.

		//delete sgmFastScanParameters.at(x); // we're copying these; don't need to keep these ones around. They're our responsibility to delete.
	}
}


SGMFastScanSettings::SGMFastScanSettings(double runSeconds, int motorSettings, double scalerTime, int baseLine, int undulatorVelocity, QObject *parent, const QString &name) :
	AMDbObject(parent)
{
	setName(name);
	runSeconds_ = runSeconds;
	motorSettings_ = motorSettings;
	scalerTime_ = scalerTime;
	baseLine_ = baseLine;
	undulatorVelocity_ = undulatorVelocity;
}

double SGMFastScanSettings::runSeconds() const{
	return runSeconds_;
}

int SGMFastScanSettings::motorSettings() const{
	return motorSettings_;
}

double SGMFastScanSettings::scalerTime() const{
	return scalerTime_;
}

int SGMFastScanSettings::baseLine() const{
	return baseLine_;
}

int SGMFastScanSettings::undulatorVelocity() const{
	return undulatorVelocity_;
}

SGMFastScanSettings& SGMFastScanSettings::operator =(const SGMFastScanSettings &other){
	if(this != &other){
		setRunSeconds(other.runSeconds());
		setMotorSettings(other.motorSettings());
		setScalerTime(other.scalerTime());
		setBaseLine(other.baseLine());
		setUndulatorVelocity(other.undulatorVelocity());
	}
	return *this;
}

void SGMFastScanSettings::setRunSeconds(double runSeconds){
	if(runSeconds_ != runSeconds){
		runSeconds_ = runSeconds;
		emit runSecondsChanged(runSeconds_);
	}
}

void SGMFastScanSettings::setMotorSettings(int motorSettings){
	if(motorSettings_ != motorSettings){
		motorSettings_ = motorSettings;
		emit motorSettingsChanged(motorSettings_);
	}
}

void SGMFastScanSettings::setScalerTime(double scalerTime){
	if(scalerTime_ != scalerTime){
		scalerTime_ = scalerTime;
		emit scalerTimeChanged(scalerTime_);
	}
}

void SGMFastScanSettings::setBaseLine(int baseLine){
	if(baseLine_ != baseLine){
		baseLine_ = baseLine;
		emit baseLineChanged(baseLine_);
	}
}

void SGMFastScanSettings::setUndulatorVelocity(int undulatorVelocity){
	if(undulatorVelocity_ != undulatorVelocity){
		undulatorVelocity_ = undulatorVelocity;
		emit undulatorVelocityChanged(undulatorVelocity_);
	}
}


SGMFastScanParameters::SGMFastScanParameters(QObject *parent) : AMDbObject(parent)
{
	usingEdgeInfo_ = false;
}

SGMFastScanParameters::SGMFastScanParameters(const QString &element, const SGMEdgeInfo &edgeInfo, const SGMFastScanSettings &fastScanSettings, QObject *parent, const QString &name) :
		AMDbObject(parent)
{
	setName(name);
	setElement(element);

	usingEdgeInfo_ = true;
	edgeInfo_ = edgeInfo;
	fastScanSettings_ = fastScanSettings;
}

SGMFastScanParameters::SGMFastScanParameters(const QString &element, const SGMStandardScanInfo &standardScanInfo, const SGMFastScanSettings &fastScanSettings, QObject *parent, const QString &name) :
		AMDbObject(parent)
{
	setName(name);
	setElement(element);

	usingEdgeInfo_ = false;
	standardScanInfo_ = standardScanInfo;
	fastScanSettings_ = fastScanSettings;
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

QString SGMFastScanParameters::element() const{
	return element_;
}

QString SGMFastScanParameters::edge() const{
	if(usingEdgeInfo_)
		return edgeInfo_.edge();
	else
		return standardScanInfo_.scanName();
}

double SGMFastScanParameters::runSeconds() const{
	return fastScanSettings_.runSeconds();
}

double SGMFastScanParameters::energyStart() const{
	if(usingEdgeInfo_)
		return edgeInfo_.standardStart().energy();
	else
		return standardScanInfo_.standardStart().energy();
}

double SGMFastScanParameters::energyMidpoint() const{
	if(usingEdgeInfo_)
		return edgeInfo_.standardMiddle().energy();
	else
		return standardScanInfo_.standardMiddle().energy();
}

double SGMFastScanParameters::energyEnd() const{
	if(usingEdgeInfo_)
		return edgeInfo_.standardEnd().energy();
	else
		return standardScanInfo_.standardEnd().energy();
}

int SGMFastScanParameters::velocity() const{
	return fastScanSettings_.motorSettings();
}

int SGMFastScanParameters::velocityBase() const{
	return fastScanSettings_.motorSettings();
}

int SGMFastScanParameters::acceleration() const{
	return fastScanSettings_.motorSettings();
}

double SGMFastScanParameters::scalerTime() const{
	return fastScanSettings_.scalerTime();
}

int SGMFastScanParameters::baseLine() const{
	return fastScanSettings_.baseLine();
}

int SGMFastScanParameters::undulatorStartStep() const{
	if(usingEdgeInfo_)
		return edgeInfo_.standardStart().undulatorStepSetpoint();
	else
		return standardScanInfo_.standardStart().undulatorStepSetpoint();
}

int SGMFastScanParameters::undulatorVelocity() const{
	return fastScanSettings_.undulatorVelocity();
}

int SGMFastScanParameters::undulatorRelativeStep() const{
	if(usingEdgeInfo_)
		return edgeInfo_.standardEnd().undulatorStepSetpoint() - edgeInfo_.standardStart().undulatorStepSetpoint();
	else
		return standardScanInfo_.standardEnd().undulatorStepSetpoint() - standardScanInfo_.standardStart().undulatorStepSetpoint();
}

double SGMFastScanParameters::exitSlitDistance() const{
	if(usingEdgeInfo_)
		return edgeInfo_.standardMiddle().exitSlitDistance();
	else
		return standardScanInfo_.standardMiddle().exitSlitDistance();
}

int SGMFastScanParameters::sgmGrating() const{
	if(usingEdgeInfo_)
		return edgeInfo_.standardStart().sgmGrating();
	else
		return standardScanInfo_.standardStart().sgmGrating();
}

SGMEdgeInfo SGMFastScanParameters::edgeInfo() const{
	if(usingEdgeInfo_)
		return edgeInfo_;
	else
		return SGMEdgeInfo();
}

SGMStandardScanInfo SGMFastScanParameters::standardScanInfo() const{
	if(!usingEdgeInfo_)
		return standardScanInfo_;
	else
		return SGMStandardScanInfo();
}

SGMFastScanSettings SGMFastScanParameters::fastScanSettings() const{
	return fastScanSettings_;
}

void SGMFastScanParameters::setElement(const QString &element){
	element_ = element;
}

void SGMFastScanParameters::setEdge(const QString &edge){
	if(usingEdgeInfo_)
		edgeInfo_.setEdge(edge);
	else
		standardScanInfo_.setScanName(edge);
}

void SGMFastScanParameters::setRunSeconds(double runSeconds){
	fastScanSettings_.setRunSeconds(runSeconds);
}

void SGMFastScanParameters::setEnergyStart(double energyStart){
	if(usingEdgeInfo_)
		edgeInfo_.standardStart().setEnergy(energyStart);
	else
		standardScanInfo_.standardStart().setEnergy(energyStart);
}

void SGMFastScanParameters::setEnergyMidpoint(double energyMidpoint){
	if(usingEdgeInfo_)
		edgeInfo_.standardMiddle().setEnergy(energyMidpoint);
	else
		standardScanInfo_.standardMiddle().setEnergy(energyMidpoint);
}

void SGMFastScanParameters::setEnergyEnd(double energyEnd){
	if(usingEdgeInfo_)
		edgeInfo_.standardEnd().setEnergy(energyEnd);
	else
		standardScanInfo_.standardEnd().setEnergy(energyEnd);
}

void SGMFastScanParameters::setVelocity(int velocity){
	fastScanSettings_.setMotorSettings(velocity);
}

void SGMFastScanParameters::setVelocityBase(int velocityBase){
	fastScanSettings_.setMotorSettings(velocityBase);
}

void SGMFastScanParameters::setAcceleration(int acceleration){
	fastScanSettings_.setMotorSettings(acceleration);
}

void SGMFastScanParameters::setScalerTime(double scalerTime){
	fastScanSettings_.setScalerTime(scalerTime);
}

void SGMFastScanParameters::setBaseLine(int baseLine){
	fastScanSettings_.setBaseLine(baseLine);
}

void SGMFastScanParameters::setUndulatorStartStep(int undulatorStartStep){
	if(usingEdgeInfo_)
		edgeInfo_.standardStart().setUndulatorStepSetpoint(undulatorStartStep);
	else
		standardScanInfo_.standardStart().setUndulatorStepSetpoint(undulatorStartStep);
}

void SGMFastScanParameters::setUndulatorVelocity(int undulatorVelocity){
	fastScanSettings_.setUndulatorVelocity(undulatorVelocity);
}

void SGMFastScanParameters::setUndulatorRelativeStep(int undulatorRelativeStep){
	if(usingEdgeInfo_)
		edgeInfo_.standardEnd().setUndulatorStepSetpoint(edgeInfo_.standardStart().undulatorStepSetpoint()+undulatorRelativeStep);
	else
		standardScanInfo_.standardEnd().setUndulatorStepSetpoint(standardScanInfo_.standardStart().undulatorStepSetpoint()+undulatorRelativeStep);
}

void SGMFastScanParameters::setExitSlitDistance(double exitSlitDistance){
	if(usingEdgeInfo_)
		edgeInfo_.standardMiddle().setExitSlitDistance(exitSlitDistance);
	else
		standardScanInfo_.standardMiddle().setExitSlitDistance(exitSlitDistance);
}

void SGMFastScanParameters::setSGMGrating(int sgmGrating){
	if(usingEdgeInfo_)
		edgeInfo_.standardStart().setSGMGrating(sgmGrating);
	else
		standardScanInfo_.standardStart().setSGMGrating(sgmGrating);
}

void SGMFastScanParameters::setEdgeInfo(SGMEdgeInfo &edgeInfo){
	usingEdgeInfo_ = true;
	edgeInfo_ = edgeInfo;
}

void SGMFastScanParameters::setStandardScanInfo(SGMStandardScanInfo &standardScanInfo){
	usingEdgeInfo_ = false;
	standardScanInfo_ = standardScanInfo;
}

void SGMFastScanParameters::setFastScanSettings(SGMFastScanSettings &fastScanSettings){
	fastScanSettings_ = fastScanSettings;
}

void SGMFastScanParameters::setUsingEdgeInfo(bool usingEdgeInfo){
	usingEdgeInfo_ = usingEdgeInfo;
}

bool SGMFastScanParameters::usingEdgeInfo() const{
	return usingEdgeInfo_;
}
