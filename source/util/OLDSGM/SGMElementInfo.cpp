/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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
#include <cmath>

#include "util/AMPeriodicTable.h"

 SGMEnergyPosition::~SGMEnergyPosition(){}
SGMEnergyPosition::SGMEnergyPosition(const QString &name, double energy, int monoEncoderTarget, int undulatorStepSetpoint, double exitSlitDistance, int sgmGrating)
{
	setName(name);
	setEnergy(energy);
	setMonoEncoderTarget(monoEncoderTarget);
	setUndulatorStepSetpoint(undulatorStepSetpoint);
	setExitSlitDistance(exitSlitDistance);
	setSGMGrating(sgmGrating);
}

QString SGMEnergyPosition::descriptionFromName() const{
	if(name().isEmpty())
		return QString();
	QString description = name();
	for(int x = 1; x < description.size(); x++){
		if(description.at(x).isUpper() || (description.at(x).isDigit() && !description.at(x-1).isDigit()) ){
			description.insert(x, " ");
			x++;
		}
	}
	return description;
}

double SGMEnergyPosition::energy() const {
	return energy_;
}

int SGMEnergyPosition::monoEncoderTarget() const {
	return monoEncoderTarget_;
}

int SGMEnergyPosition::undulatorStepSetpoint() const {
	return undulatorStepSetpoint_;
}

double SGMEnergyPosition::exitSlitDistance() const {
	return exitSlitDistance_;
}

int SGMEnergyPosition::sgmGrating() const {
	return sgmGrating_;
}

SGMEnergyPosition& SGMEnergyPosition::operator=(const SGMEnergyPosition &other){
	if(this != &other){
		AMDbObject::operator=(other);
		setEnergy(other.energy());
		setMonoEncoderTarget(other.monoEncoderTarget());
		setUndulatorStepSetpoint(other.undulatorStepSetpoint());
		setExitSlitDistance(other.exitSlitDistance());
		setSGMGrating(other.sgmGrating());
	}
	return *this;
}

bool SGMEnergyPosition::operator ==(const SGMEnergyPosition &other) const{
	if( fabs(energy() - other.energy()) < 0.01
			&& monoEncoderTarget() == other.monoEncoderTarget()
			&& undulatorStepSetpoint() == other.undulatorStepSetpoint()
			&& fabs(exitSlitDistance() - other.exitSlitDistance()) < 0.01
			&& sgmGrating() == other.sgmGrating())
		return true;
	return false;
}

bool SGMEnergyPosition::operator !=(const SGMEnergyPosition &other) const{
	return !(this->operator ==(other));
}

QDebug operator<<(QDebug d, const SGMEnergyPosition &energyPosition){
	d << QString("(%1 %2 %3 %4)").arg(energyPosition.energy()).arg(energyPosition.monoEncoderTarget()).arg(energyPosition.undulatorStepSetpoint()).arg(energyPosition.exitSlitDistance());
	return d;
}

void SGMEnergyPosition::setEnergy(double energy) {
	if(energy_ != energy){
		energy_ = energy;
		setModified(true);
		emit energyChanged(energy_);
		emit energyPositionChanged();
	}
}

void SGMEnergyPosition::setMonoEncoderTarget(int monoEncoderTarget) {
	if(monoEncoderTarget_ != monoEncoderTarget){
		monoEncoderTarget_ = monoEncoderTarget;
		setModified(true);
		emit monoEncoderTargetChanged(monoEncoderTarget_);
		emit energyPositionChanged();
	}
}

void SGMEnergyPosition::setUndulatorStepSetpoint(int undulatorStepSetpoint) {
	if(undulatorStepSetpoint_ != undulatorStepSetpoint){
		undulatorStepSetpoint_ = undulatorStepSetpoint;
		setModified(true);
		emit undulatorStepSetpointChanged(undulatorStepSetpoint_);
		emit energyPositionChanged();
	}
}

void SGMEnergyPosition::setExitSlitDistance(double exitSlitDistance) {
	if(exitSlitDistance_ != exitSlitDistance){
		exitSlitDistance_ = exitSlitDistance;
		setModified(true);
		emit exitSlitDistanceChanged(exitSlitDistance_);
		emit energyPositionChanged();
	}
}

void SGMEnergyPosition::setSGMGrating(int sgmGrating) {
	if(sgmGrating_ != sgmGrating){
		sgmGrating_ = sgmGrating;
		setModified(true);
		emit sgmGratingChanged(sgmGrating_);
		emit energyPositionChanged();
	}
}

 SGMScanInfo::~SGMScanInfo(){}
SGMScanInfo::SGMScanInfo(const QString &scanName, QPair<QString, double> edgeAndEnergy, SGMEnergyPosition start, SGMEnergyPosition middle, SGMEnergyPosition end, QObject *parent) :
	AMDbObject(parent)
{
	setName(scanName);
	setScanName(scanName);
	if(!edgeAndEnergy.first.isEmpty() && edgeAndEnergy.second > 0)
		hasEdge_ = true;
	else
		hasEdge_ = false;
	edge_ = edgeAndEnergy.first;
	energy_ = edgeAndEnergy.second;
	setStart(start);
	setMiddle(middle);
	setEnd(end);
}

SGMScanInfo::SGMScanInfo(const QString &scanName, AMAbsorptionEdge edge, SGMEnergyPosition start, SGMEnergyPosition middle, SGMEnergyPosition end, QObject *parent)
	: AMDbObject(parent)
{
	setName(scanName);
	setScanName(scanName);
	hasEdge_ = !edge.isNull();
	edge_ = edge.edgeName().left(1);
	energy_ = edge.energy();
	setStart(start);
	setMiddle(middle);
	setEnd(end);
}

QString SGMScanInfo::scanName() const {
	return scanName_;
}

QString SGMScanInfo::descriptionFromName() const{
	if(name().isEmpty())
		return QString();
	QString description = name();
	for(int x = 1; x < description.size(); x++){
		if(description.at(x).isUpper() || (description.at(x).isDigit() && !description.at(x-1).isDigit()) ){
			description.insert(x, " ");
			x++;
		}
	}
	return description;
}

bool SGMScanInfo::hasEdge() const {
	return hasEdge_;
}

QString SGMScanInfo::edge() const {
	return edge_;
}

double SGMScanInfo::energy() const {
	return energy_;
}

const SGMEnergyPosition& SGMScanInfo::start() const {
	return start_;
}

const SGMEnergyPosition& SGMScanInfo::middle() const {
	return middle_;
}

const SGMEnergyPosition& SGMScanInfo::end() const {
	return end_;
}

SGMScanInfo& SGMScanInfo::operator =(const SGMScanInfo &other){
	if(this != &other){
		AMDbObject::operator=(other);
		setScanName(other.scanName());
		setHasEdge(other.hasEdge());
		setEdge(other.edge());
		setEnergy(other.energy());
		setStart(other.start());
		setMiddle(other.middle());
		setEnd(other.end());
	}
	return *this;
}

bool SGMScanInfo::operator ==(const SGMScanInfo &other) const{
	if(hasEdge() == other.hasEdge()
			&& edge() == other.edge()
			&& fabs(energy() - other.energy()) < 0.01
			&& start() == other.start()
			&& middle() == other.middle()
			&& end() == other.end())
		return true;
	return false;
}

bool SGMScanInfo::operator !=(const SGMScanInfo &other) const{
	return !(this->operator ==(other));
}

QDebug operator<<(QDebug d, const SGMScanInfo &edgeInfo){
	if(edgeInfo.hasEdge())
		d << QString("{%1, %2, %3, ").arg(edgeInfo.scanName()).arg(edgeInfo.energy()).arg(edgeInfo.edge()) << edgeInfo.start() << edgeInfo.middle() << edgeInfo.end() << "}";
	else
		d << QString("{%1, ").arg(edgeInfo.scanName()) << edgeInfo.start() << edgeInfo.middle() << edgeInfo.end() << "}";
	return d;
}

void SGMScanInfo::setScanName(const QString &scanName) {
	scanName_ = scanName;
}

void SGMScanInfo::setHasEdge(bool hasEdge) {
	if(hasEdge_ != hasEdge){
		hasEdge_ = hasEdge;
		setModified(true);
		emit hasEdgeChanged(hasEdge_);
		emit scanInfoChanged();
	}
}

void SGMScanInfo::setEdge(const QString &edge) {
	if(edge_ != edge){
		edge_ = edge;
		setModified(true);
		emit edgeChanged(edge_);
		emit scanInfoChanged();
	}
}

void SGMScanInfo::setEnergy(double energy) {
	if(energy_ != energy){
		energy_ = energy;
		setModified(true);
		emit energyChanged(energy_);
		emit scanInfoChanged();
	}
}

void SGMScanInfo::setStart(const SGMEnergyPosition &start, bool ignoreDatabaseId) {
	if( (start_ != start) || (!ignoreDatabaseId && ((start_.id() != start.id()) || (start_.name() != start.name())) ) ){
		start_ = start;
		setModified(true);
		emit startChanged();
		emit scanInfoChanged();
	}
}

void SGMScanInfo::setMiddle(const SGMEnergyPosition &middle, bool ignoreDatabaseId) {
	if( (middle_ != middle)  || (!ignoreDatabaseId && ((middle_.id() != middle.id()) || (middle_.name() != middle.name())) ) ){
		middle_ = middle;
		setModified(true);
		emit middleChanged();
		emit scanInfoChanged();
	}
}

void SGMScanInfo::setEnd(const SGMEnergyPosition &end, bool ignoreDatabaseId) {
	if( (end_ != end)  || (!ignoreDatabaseId && ((end_.id() != end.id()) || (end_.name() != end.name())) ) ){
		end_ = end;
		setModified(true);
		emit endChanged();
		emit scanInfoChanged();
	}
}

 SGMElementInfo::~SGMElementInfo(){}
SGMElementInfo::SGMElementInfo(const QString &name, AMElement *element, QObject *parent) :
		AMDbObject(parent)
{
	setName(name);
	element_ = element;
}

AMElement* SGMElementInfo::element() const{
	return element_;
}

AMOrderedSet<QString, SGMScanInfo> SGMElementInfo::sgmEdgeInfos() const{
	return sgmEdgeInfos_;
}

AMOrderedSet<int, SGMFastScanParameters*> SGMElementInfo::availableFastScanParameters() const{
	return availableFastScanParameters_;
}

bool SGMElementInfo::loadFromDb(AMDatabase *db, int id){
	bool retVal = AMDbObject::loadFromDb(db, id);
	if(retVal && availableFastScanParameters_.count() > 0)
		element_ = AMPeriodicTable::table()->elementByName(availableFastScanParameters_.at(0)->element());
	return retVal;
}

bool SGMElementInfo::addEdgeInfo(const SGMScanInfo &edgeInfo){
	return sgmEdgeInfos_.append(edgeInfo, edgeInfo.edge());
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

AMDbObjectList SGMElementInfo::dbReadSGMFastScanParameters(){
	AMDbObjectList rv;
	for(int x = 0; x < availableFastScanParameters_.count(); x++)
		rv << availableFastScanParameters_[x];
	return rv;
}

void SGMElementInfo::dbLoadSGMEdgeInfos(const AMDbObjectList &sgmEdgeInfos){
	sgmEdgeInfos_.clear();

	for(int x = 0; x < sgmEdgeInfos.count(); x++){
		SGMScanInfo* newEdgeInfo = qobject_cast<SGMScanInfo*>(sgmEdgeInfos.at(x));
		if(newEdgeInfo)
			sgmEdgeInfos_.append(*newEdgeInfo, newEdgeInfo->edge());// note: makes a copy of object pointed to by newStandardScanInfo, and stores in our internal list.

		sgmEdgeInfos.at(x)->deleteLater(); // we're copying these, don't need to keep these ones around. They're our responsibility to delete.
	}
}

void SGMElementInfo::dbLoadSGMFastScanParameters(const AMDbObjectList &sgmFastScanParameters){
	availableFastScanParameters_.clear();

	for(int x = 0; x < sgmFastScanParameters.count(); x++){
		SGMFastScanParameters* newFastScanParameter = qobject_cast<SGMFastScanParameters*>(sgmFastScanParameters.at(x));
		if(newFastScanParameter)
			availableFastScanParameters_.append(newFastScanParameter, (int)newFastScanParameter->runSeconds());// note: makes a copy of object pointed to by newStandardScanInfo, and stores in our internal list.
	}
}


 SGMFastScanSettings::~SGMFastScanSettings(){}
SGMFastScanSettings::SGMFastScanSettings(const QString &name, double runSeconds, int motorSettings, double scalerTime, int baseLine, int undulatorVelocity, QObject *parent) :
	AMDbObject(parent)
{
	setName(name);
	runSeconds_ = runSeconds;
	motorSettings_ = motorSettings;
	scalerTime_ = scalerTime;
	baseLine_ = baseLine;
	undulatorVelocity_ = undulatorVelocity;
}

QString SGMFastScanSettings::descriptionFromName() const{
	if(name().isEmpty())
		return QString();
	QString description = name();
	for(int x = 1; x < description.size(); x++){
		if(description.at(x).isUpper() || (description.at(x).isDigit() && !description.at(x-1).isDigit()) ){
			description.insert(x, " ");
			x++;
		}
	}
	return description;
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
		AMDbObject::operator=(other);
		setRunSeconds(other.runSeconds());
		setMotorSettings(other.motorSettings());
		setScalerTime(other.scalerTime());
		setBaseLine(other.baseLine());
		setUndulatorVelocity(other.undulatorVelocity());
		setModified(false);
	}
	return *this;
}

bool SGMFastScanSettings::operator ==(const SGMFastScanSettings &other) const{
	if( fabs(runSeconds() - other.runSeconds()) < 0.01
			&& motorSettings() == other.motorSettings()
			&& fabs(scalerTime() - other.scalerTime()) < 0.01
			&& baseLine() == other.baseLine()
			&& undulatorVelocity() == other.undulatorVelocity())
		return true;
	return false;
}

bool SGMFastScanSettings::operator !=(const SGMFastScanSettings &other) const{
	return !(this->operator ==(other));
}

void SGMFastScanSettings::setRunSeconds(double runSeconds){
	if(runSeconds_ != runSeconds){
		runSeconds_ = runSeconds;
		setModified(true);
		emit runSecondsChanged(runSeconds_);
		emit fastScanSettingsChanged();
	}
}

void SGMFastScanSettings::setMotorSettings(int motorSettings){
	if(motorSettings_ != motorSettings){
		motorSettings_ = motorSettings;
		setModified(true);
		emit motorSettingsChanged(motorSettings_);
		emit fastScanSettingsChanged();
	}
}

void SGMFastScanSettings::setScalerTime(double scalerTime){
	if(scalerTime_ != scalerTime){
		scalerTime_ = scalerTime;
		setModified(true);
		emit scalerTimeChanged(scalerTime_);
		emit fastScanSettingsChanged();
	}
}

void SGMFastScanSettings::setBaseLine(int baseLine){
	if(baseLine_ != baseLine){
		baseLine_ = baseLine;
		setModified(true);
		emit baseLineChanged(baseLine_);
		emit fastScanSettingsChanged();
	}
}

void SGMFastScanSettings::setUndulatorVelocity(int undulatorVelocity){
	if(undulatorVelocity_ != undulatorVelocity){
		undulatorVelocity_ = undulatorVelocity;
		setModified(true);
		emit undulatorVelocityChanged(undulatorVelocity_);
		emit fastScanSettingsChanged();
	}
}

 SGMFastScanParameters::~SGMFastScanParameters(){}
SGMFastScanParameters::SGMFastScanParameters(const QString &name, const QString &element, const SGMScanInfo &scanInfo, const SGMFastScanSettings &fastScanSettings, QObject *parent) :
		AMDbObject(parent)
{
	setName(name);
	setElement(element);

	setScanInfo(scanInfo);
	setFastScanSettings(fastScanSettings);
}

SGMFastScanParameters& SGMFastScanParameters::operator =(const SGMFastScanParameters &other){
	if(this != &other){
		AMDbObject::operator=(other);
		setElement(other.element());
		setScanInfo(other.scanInfo());
		setFastScanSettings(other.fastScanSettings());
	}
	return *this;
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

QDebug operator<<(QDebug d, const SGMFastScanParameters &fastScanParameters){
	d << QString("{%1, ").arg(fastScanParameters.element()) << fastScanParameters.scanInfo() << "}";
	return d;
}

QString SGMFastScanParameters::element() const{
	return element_;
}

QString SGMFastScanParameters::edge() const{
	return scanInfo_.edge();
}

double SGMFastScanParameters::runSeconds() const{
	return fastScanSettings_.runSeconds();
}

double SGMFastScanParameters::energyStart() const{
	return scanInfo_.start().energy();
}

double SGMFastScanParameters::energyMidpoint() const{
	return scanInfo_.middle().energy();
}

double SGMFastScanParameters::energyEnd() const{
	return scanInfo_.end().energy();
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
	return scanInfo_.start().undulatorStepSetpoint();
}

int SGMFastScanParameters::undulatorVelocity() const{
	return fastScanSettings_.undulatorVelocity();
}

int SGMFastScanParameters::undulatorRelativeStep() const{
	return scanInfo_.end().undulatorStepSetpoint() - scanInfo_.start().undulatorStepSetpoint();
}

double SGMFastScanParameters::exitSlitDistance() const{
	return scanInfo_.middle().exitSlitDistance();
}

int SGMFastScanParameters::sgmGrating() const{
	return scanInfo_.start().sgmGrating();
}

SGMScanInfo SGMFastScanParameters::scanInfo() const{
	return scanInfo_;
}

SGMFastScanSettings SGMFastScanParameters::fastScanSettings() const{
	return fastScanSettings_;
}

void SGMFastScanParameters::setElement(const QString &element){
	if(element_ != element){
		element_ = element;
		emit elementChanged(element_);
	}
}

void SGMFastScanParameters::setEdge(const QString &edge){
	scanInfo_.setEdge(edge);
}

void SGMFastScanParameters::setRunSeconds(double runSeconds){
	fastScanSettings_.setRunSeconds(runSeconds);
}

void SGMFastScanParameters::setEnergyStart(double energyStart){
	SGMEnergyPosition newEnergyPosition = scanInfo_.start();
	newEnergyPosition.setEnergy(energyStart);
	scanInfo_.setStart(newEnergyPosition);
}

void SGMFastScanParameters::setEnergyMidpoint(double energyMidpoint){
	SGMEnergyPosition newEnergyPosition = scanInfo_.middle();
	newEnergyPosition.setEnergy(energyMidpoint);
	scanInfo_.setMiddle(newEnergyPosition);
}

void SGMFastScanParameters::setEnergyEnd(double energyEnd){
	SGMEnergyPosition newEnergyPosition = scanInfo_.end();
	newEnergyPosition.setEnergy(energyEnd);
	scanInfo_.setEnd(newEnergyPosition);
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
	SGMEnergyPosition newEnergyPosition = scanInfo_.start();
	newEnergyPosition.setUndulatorStepSetpoint(undulatorStartStep);
	scanInfo_.setStart(newEnergyPosition);
}

void SGMFastScanParameters::setUndulatorVelocity(int undulatorVelocity){
	fastScanSettings_.setUndulatorVelocity(undulatorVelocity);
}

void SGMFastScanParameters::setUndulatorRelativeStep(int undulatorRelativeStep){
	SGMEnergyPosition newEnergyPosition = scanInfo_.end();
	newEnergyPosition.setUndulatorStepSetpoint(scanInfo_.start().undulatorStepSetpoint()+undulatorRelativeStep);
	scanInfo_.setEnd(newEnergyPosition);
}

void SGMFastScanParameters::setExitSlitDistance(double exitSlitDistance){
	SGMEnergyPosition newEnergyPosition = scanInfo_.middle();
	newEnergyPosition.setExitSlitDistance(exitSlitDistance);
	scanInfo_.setMiddle(newEnergyPosition);
}

void SGMFastScanParameters::setSGMGrating(int sgmGrating){
	SGMEnergyPosition newEnergyPosition = scanInfo_.start();
	newEnergyPosition.setSGMGrating(sgmGrating);
	scanInfo_.setStart(newEnergyPosition);
}

void SGMFastScanParameters::setScanInfo(const SGMScanInfo &scanInfo, bool ignoreDatabaseId){
	disconnect(&scanInfo_, 0);

	scanInfo_ = scanInfo;
	if(!ignoreDatabaseId && (scanInfo_.start().id() != scanInfo.start().id()) )
		scanInfo_.setStart(scanInfo.start(), false);
	if(!ignoreDatabaseId && (scanInfo_.middle().id() != scanInfo.middle().id()) )
		scanInfo_.setMiddle(scanInfo.middle(), false);
	if(!ignoreDatabaseId && (scanInfo_.end().id() != scanInfo.end().id()) )
		scanInfo_.setEnd(scanInfo.end(), false);

	connect(&scanInfo_, SIGNAL(startChanged()), this, SLOT(onStartChanged()));
	connect(&scanInfo_, SIGNAL(middleChanged()), this, SLOT(onMiddleChanged()));
	connect(&scanInfo_, SIGNAL(endChanged()), this, SLOT(onEndChanged()));
	connect(&scanInfo, SIGNAL(scanInfoChanged()), this, SIGNAL(scanInfoChanged()));
}

void SGMFastScanParameters::setStartPosition(const SGMEnergyPosition &start){
	scanInfo_.setStart(start);
}

void SGMFastScanParameters::setMiddlePosition(const SGMEnergyPosition &middle){
	scanInfo_.setMiddle(middle);
}

void SGMFastScanParameters::setEndPosition(const SGMEnergyPosition &end){
	scanInfo_.setEnd(end);
}

void SGMFastScanParameters::setFastScanSettings(const SGMFastScanSettings &fastScanSettings){
	if(fastScanSettings_ != fastScanSettings){
		disconnect(&fastScanSettings_, 0);
		fastScanSettings_ = fastScanSettings;
		setModified(true);
		connect(&fastScanSettings_, SIGNAL(runSecondsChanged(double)), this, SIGNAL(runSecondsChanged(double)));
		connect(&fastScanSettings_, SIGNAL(motorSettingsChanged(int)), this, SIGNAL(velocityChanged(int)));
		connect(&fastScanSettings_, SIGNAL(motorSettingsChanged(int)), this, SIGNAL(velocityBaseChanged(int)));
		connect(&fastScanSettings_, SIGNAL(motorSettingsChanged(int)), this, SIGNAL(accelerationChanged(int)));
		connect(&fastScanSettings_, SIGNAL(scalerTimeChanged(double)), this, SIGNAL(scalerTimeChanged(double)));
		connect(&fastScanSettings_, SIGNAL(baseLineChanged(int)), this, SIGNAL(baseLineChanged(int)));
		connect(&fastScanSettings_, SIGNAL(undulatorVelocityChanged(int)), this, SIGNAL(undulatorVelocityChanged(int)));
		connect(&fastScanSettings_, SIGNAL(fastScanSettingsChanged()), this, SIGNAL(fastScanSettingsChanged()));
	}
}

void SGMFastScanParameters::onStartChanged(){
	emit energyStartChanged(scanInfo_.start().energy());
	emit undulatorStartStepChanged(scanInfo_.start().undulatorStepSetpoint());
	emit sgmGratingChanged(scanInfo_.start().sgmGrating());
	emit startPositionChanged();
}

void SGMFastScanParameters::onMiddleChanged(){
	emit energyMidpointChanged(scanInfo_.middle().energy());
	emit exitSlitDistanceChanged(scanInfo_.middle().exitSlitDistance());
	emit middlePositionChanged();
}

void SGMFastScanParameters::onEndChanged(){
	emit energyEndChanged(scanInfo_.end().energy());
	emit undulatorRelativeStepChanged(scanInfo_.end().undulatorStepSetpoint()-scanInfo_.start().undulatorStepSetpoint());
	emit endPositionChanged();
}
