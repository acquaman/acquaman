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


#include "CLSAmptekSDD123DetectorNew.h"

#include <QSignalMapper>

#include "ui/CLS/CLSAmptekSDD123DetailedDetectorView.h"

 CLSAmptekSDD123DetectorNew::~CLSAmptekSDD123DetectorNew(){}
CLSAmptekSDD123DetectorNew::CLSAmptekSDD123DetectorNew(const QString &name, const QString &description, const QString &baseName, QObject *parent) :
	AMXRFDetector(name, description, parent)
{	
	baseName_ = baseName;

	units_ = "Counts";

	eVPerBin_ = 8.25;

	// This can be done in the constructor because it doesn't depend on anything.
	axes_ = QList<AMAxisInfo>();
	AMAxisInfo ai("Energy", 1024, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.isUniform = true;
	ai.increment = eVPerBin_;
	axes_ << ai;

	acquireControl_ = new AMPVControl(name+"StartAcquisition", baseName+":spectrum:start", baseName+":spectrum:start", baseName+":spectrum:stop", this, 0.5);
	acquireControl_->setAllowsMovesWhileMoving(true);
	acquisitionStatusControl_ = new AMReadOnlyPVControl(name+"Status", baseName+":spectrum:dwellState", this);
	acquireTimeControl_ = new AMPVControl(name+"IntegrationTime", baseName+":spectrum:dwellTime", baseName+":spectrum:dwellTime", QString() , this, 0.05);
	elapsedTimeControl_ = new AMReadOnlyPVControl(name+"Elapsed Time", baseName+":spectrum:elapsedTime", this);

	fastCountsControl_ = new AMReadOnlyPVControl("FastCounts", QString("%1:spectrum:fastCounts").arg(baseName_), this);
	averageFastCountsControl_ = new AMReadOnlyPVControl("AverageFastCounts", QString("%1:spectrum:average:fastCounts").arg(baseName_), this);
	slowCountsControl_ = new AMReadOnlyPVControl("SlowCounts", QString("%1:spectrum:slowCounts").arg(baseName_), this);
	averageSlowCountsControl_ = new AMReadOnlyPVControl("AverageSlowCounts", QString("%1:spectrum:average:slowCounts").arg(baseName_), this);

	icrControls_.append(fastCountsControl_);
	ocrControls_.append(slowCountsControl_);
	spectraControls_.append(new AMReadOnlyPVControl("Raw Spectrum", baseName+":spectrum", this));

	lowIndicesSignalMapper_ = new QSignalMapper();
	highIndicesSignalMapper_ = new QSignalMapper();
	AMSinglePVControl *oneLowIndex;
	AMSinglePVControl *oneHighIndex;
	for(int x = 0; x < 8; x++){
		oneLowIndex = new AMSinglePVControl(QString("ROI%1Low").arg(x+1), QString("%1:ROI:%2:lowChannel").arg(baseName_).arg(x+1), this, 0.5);
		oneHighIndex = new AMSinglePVControl(QString("ROI%1Low").arg(x+1), QString("%1:ROI:%2:highChannel").arg(baseName_).arg(x+1), this, 0.5);

		roiLowIndices_.append(oneLowIndex);
		roiHighIndices_.append(oneHighIndex);

		lowIndicesSignalMapper_->setMapping(oneLowIndex, x);
		highIndicesSignalMapper_->setMapping(oneHighIndex, x);

		connect(oneLowIndex, SIGNAL(valueChanged(double)), lowIndicesSignalMapper_, SLOT(map()));
		connect(oneHighIndex, SIGNAL(valueChanged(double)), highIndicesSignalMapper_, SLOT(map()));
	}


	accumulationTimeControl_ = new AMReadOnlyPVControl("AccumulationTime", QString("%1:spectrum:accumulationTime").arg(baseName_), this);
	averageAccumulationTimeControl_ = new AMReadOnlyPVControl("AverageAccumulationTime", QString("%1:spectrum:average:accumulationTime").arg(baseName_), this);
	liveTimeControl_ = new AMReadOnlyPVControl("LiveTime", QString("%1:spectrum:liveTime").arg(baseName_), this);
	averageLiveTimeControl_ = new AMReadOnlyPVControl("AverageLiveTime", QString("%1:spectrum:average:liveTime").arg(baseName_), this);
	realTimeControl_ = new AMReadOnlyPVControl("RealTime", QString("%1:spectrum:realTime").arg(baseName_), this);
	averageRealTimeControl_ = new AMReadOnlyPVControl("AverageRealTime", QString("%1:spectrum:average:realTime").arg(baseName_), this);

	mcaChannelsControl_ = new AMReadOnlyPVControl(name+"MCAChannels", baseName+":parameters:MCAChannelCount", this);
	detectorTemperatureControl_ = new AMReadOnlyPVControl(name+"DetectorTemperature", baseName+":parameters:temperature", this);

	totalGainControl_ = new AMSinglePVControl(QString("TotalGain"), QString("%1:parameters:totalGain").arg(baseName_), this, 0.1);
	pileUpRejectionControl_ = new AMSinglePVControl(QString("PileUpRejection"), QString("%1:parameters:pileUpRejection").arg(baseName_), this, 0.5);
	thermoelectricCoolerControl_ = new AMSinglePVControl(QString("ThermoelectricCooler"), QString("%1:parameters:thermoelectricCooler").arg(baseName_), this, 0.1);
	fastThresholdControl_ = new AMSinglePVControl(QString("FastThreshold"), QString("%1:parameters:fastThreshold").arg(baseName_), this, 0.1);
	slowThresholdControl_ = new AMSinglePVControl(QString("SlowThreshold"), QString("%1:parameters:slowThreshold").arg(baseName_), this, 0.1);
	peakingTimeControl_ = new AMSinglePVControl(QString("PeakingTime"), QString("%1:parameters:peakingTime").arg(baseName_), this, 0.1);
	fastChannelPeakingTimeControl_ = new AMSinglePVControl(QString("FastChannelPeakingTime"), QString("%1:parameters:fastChannelPeakingTime").arg(baseName_), this, 0.1);

	isRequestedControl_ = new AMPVControl(name+"IsRequested", baseName+":spectrum:isRequested", baseName+":spectrum:isRequested", QString(), this, 0.5);

	connect(lowIndicesSignalMapper_, SIGNAL(mapped(int)), this, SLOT(onLowIndexValueChanged(int)));
	connect(highIndicesSignalMapper_, SIGNAL(mapped(int)), this, SLOT(onHighIndexValueChanged(int)));

	connect(detectorTemperatureControl_, SIGNAL(valueChanged(double)), this, SIGNAL(detectorTemperatureChanged(double)));

	allControlsCreated();	// This is very important.  It does a lot of work for you building data sources and the like.

	((AM1DProcessVariableDataSource *)rawSpectraSources_.first())->setScale(ai.increment);
	primarySpectrumDataSource_ = rawSpectraSources_.first();
}

QString CLSAmptekSDD123DetectorNew::synchronizedDwellKey() const{
	return "amptek:sdd:all:spectrum:start NPP NMS";
}

bool CLSAmptekSDD123DetectorNew::sharesDetectorTriggerSource() const{
	return currentlySynchronizedDwell();
}

#include "beamline/AMBeamline.h"
AMDetectorTriggerSource* CLSAmptekSDD123DetectorNew::detectorTriggerSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();
	return 0;
}

AMDetectorDwellTimeSource* CLSAmptekSDD123DetectorNew::detectorDwellTimeSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();
	return 0;
}

bool CLSAmptekSDD123DetectorNew::lastContinuousReading(double *outputValues) const{
	Q_UNUSED(outputValues)

	return false;
}

#include "actions3/actions/AMControlMoveAction3.h"
AMAction3* CLSAmptekSDD123DetectorNew::createEnableAction3(bool setEnabled){
	if(!isConnected())
		return 0; //NULL

	AMControlInfo setpoint = isRequestedControl_->toInfo();
	setpoint.setValue(setEnabled ? 1 : 0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, isRequestedControl_);

	if(!action)
		return 0; //NULL

	return action;
}

bool CLSAmptekSDD123DetectorNew::isEnabled() const{
	if(!isConnected())
		return false;

	return isRequestedControl_->withinTolerance(1);
}

double CLSAmptekSDD123DetectorNew::detectorTemperature() const{
	if(!isConnected())
		return -1;
	return detectorTemperatureControl_->value();
}

double CLSAmptekSDD123DetectorNew::detectorTotalGain() const{
	if (!isConnected())
		return -1;
	return totalGainControl_->value();
}

double CLSAmptekSDD123DetectorNew::mcaChannels() const{
	if(!isConnected())
		return -1;
	return mcaChannelsControl_->value();
}

double CLSAmptekSDD123DetectorNew::pileUpRejection() const{
	if(!isConnected())
		return -1;
	return pileUpRejectionControl_->value();
}

double CLSAmptekSDD123DetectorNew::thermoelectricCooler() const{
	if(!isConnected())
		return -1;
	return thermoelectricCoolerControl_->value();
}

double CLSAmptekSDD123DetectorNew::fastThreshold() const{
	if(!isConnected())
		return -1;
	return fastThresholdControl_->value();
}

double CLSAmptekSDD123DetectorNew::slowThreshold()  const{
	if(!isConnected())
		return -1;
	return slowThresholdControl_->value();
}

double CLSAmptekSDD123DetectorNew::peakingTime() const{
	if(!isConnected())
		return -1;
	return peakingTimeControl_->value();
}

double CLSAmptekSDD123DetectorNew::fastChannelPeakingTime() const{
	if(!isConnected())
		return -1;
	return fastChannelPeakingTimeControl_->value();
}

double CLSAmptekSDD123DetectorNew::eVPerBin() const{
	if(!isConnected())
		return -1;
	return eVPerBin_;
}

int CLSAmptekSDD123DetectorNew::amptekLowROI(int index){
	if(!isConnected())
		return -1;
	return int(roiLowIndices_.at(index)->value());
}

int CLSAmptekSDD123DetectorNew::amptekHighROI(int index){
	if(!isConnected())
		return -1;
	return int(roiHighIndices_.at(index)->value());
}


double CLSAmptekSDD123DetectorNew::amptekLowROIEv(int index){

	int binValue = amptekLowROI(index);
	if(binValue == -1)
		return binValue;

	return this->convertBinToEv(binValue);
}

double CLSAmptekSDD123DetectorNew::amptekHighROIEv(int index){

	int binValue = amptekHighROI(index);
	if(binValue == -1)
		return binValue;

	return this->convertBinToEv(binValue);
}


AMControl* CLSAmptekSDD123DetectorNew::fastCountsControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return fastCountsControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::averageFastCountsControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return averageFastCountsControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::slowCountsControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return slowCountsControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::averageSlowCountsControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return averageSlowCountsControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::accumulationTimeControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return accumulationTimeControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::averageAccumulationTimeControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return averageAccumulationTimeControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::liveTimeControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return liveTimeControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::averageLiveTimeControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return averageLiveTimeControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::realTimeControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return realTimeControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::averageRealTimeControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return averageRealTimeControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::detectorTemperatureControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return detectorTemperatureControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::detectorHightVoltageControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return detectorHightVoltageControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::totalGainControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return totalGainControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::mcaChannelsControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return mcaChannelsControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::pileUpRejectionControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return pileUpRejectionControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::thermoelectricCoolerControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return thermoelectricCoolerControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::fastThresholdControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return fastThresholdControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::slowThresholdControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return slowThresholdControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::peakingTimeControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return peakingTimeControl_;
}

AMControl* CLSAmptekSDD123DetectorNew::fastChannelPeakingTimeControl(const QObject *privelegedCaller) const{
	if(!isPrivelegedType(privelegedCaller))
		return 0; //NULL

	return fastChannelPeakingTimeControl_;
}

void CLSAmptekSDD123DetectorNew::onLowIndexValueChanged(int index){
	emit lowIndexValueChanged(index);
}

void CLSAmptekSDD123DetectorNew::onHighIndexValueChanged(int index){
	emit highIndexValueChanged(index);
}

bool CLSAmptekSDD123DetectorNew::isPrivelegedType(const QObject *privelegedCaller) const{
	const CLSAmptekDetailedDetectorView *detectorViewCaller = qobject_cast<const CLSAmptekDetailedDetectorView*>(privelegedCaller);
	if(detectorViewCaller)
		return true;

	const CLSAmptekDetectorConfigurationView *detectorConfigurationViewCaller = qobject_cast<const CLSAmptekDetectorConfigurationView*>(privelegedCaller);
	if(detectorConfigurationViewCaller)
		return true;

	return false;
}

double CLSAmptekSDD123DetectorNew::convertBinToEv(int binValue){
	return (binValue * eVPerBin());
}

int CLSAmptekSDD123DetectorNew::convertEvToBin(double eVValue){
	if(eVPerBin() == 0)
		return 0;

	return (int)(eVValue / eVPerBin());
}

bool CLSAmptekSDD123DetectorNew::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	Q_UNUSED(readMode)

	return false;
}

void CLSAmptekSDD123DetectorNew::setAmptekROI(int index, int lowChannel, int highChannel){
	if(index < 0 || index >= 8)
		return;
	if(lowChannel > highChannel)
		return;

	if(!roiLowIndices_.at(index)->withinTolerance(lowChannel))
		roiLowIndices_.at(index)->move(lowChannel);
	if(!roiHighIndices_.at(index)->withinTolerance(highChannel))
		roiHighIndices_.at(index)->move(highChannel);
}

void CLSAmptekSDD123DetectorNew::setAmptekROIbyEv(int index, double lowEv, double highEv){

	// Convert passed eV values into bin channel values
	int lowChannel = convertEvToBin(lowEv);
	int highChannel = convertEvToBin(highEv);
	// Pass set resposibility off to setAmptekROI
	setAmptekROI(index, lowChannel, highChannel);
}

void CLSAmptekSDD123DetectorNew::setEVPerBin(double eVPerBin){
	if(eVPerBin_ != eVPerBin){
		eVPerBin_ = eVPerBin;

		AMNumber eVPerBinNumber(eVPerBin_);
		axes_[0].increment = eVPerBinNumber;
		((AM1DProcessVariableDataSource *)rawSpectraSources_.first())->setScale(eVPerBin_);

		emit eVPerBinChanged(eVPerBin_);
	}
}
